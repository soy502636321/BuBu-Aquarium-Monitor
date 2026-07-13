#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_touch.h"

#include "core/lv_obj.h"
#include "eez-flow.h"
#include "esp_lcd_panel_ops.h"
#include "lv_port.h"

#include "lvgl.h"
#include "screens.h"
#include "ui.h"
#include "actions.h"
#include "vars.h"

#include "wifi_manager.h"
#include "bluetooth_manager.h"

#define EXAMPLE_DISPLAY_ROTATION LV_DISP_ROT_90
#define EXAMPLE_LCD_H_RES 320
#define EXAMPLE_LCD_V_RES 480
#define LCD_BUFFER_SIZE EXAMPLE_LCD_H_RES *EXAMPLE_LCD_V_RES

esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;

lv_disp_drv_t disp_drv;

static const char *TAG = "BuBu-Aquarium-Monitor_main";

static lv_disp_t *lvgl_disp;
static lv_indev_t *lvgl_touch_indev = NULL;

void lv_port_init(void);

void monitor_memory(void)
{
    // 监控 PSRAM 使用情况
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t psram_used = psram_total - psram_free;
    
    ESP_LOGI("MEM", "PSRAM: Total=%u KB, Used=%u KB, Free=%u KB", 
             psram_total / 1024, psram_used / 1024, psram_free / 1024);
    
    // 监控内部 SRAM 使用情况
    size_t sram_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t sram_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t sram_used = sram_total - sram_free;
    
    ESP_LOGI("MEM", "SRAM: Total=%u KB, Used=%u KB, Free=%u KB", 
             sram_total / 1024, sram_used / 1024, sram_free / 1024);
}

extern "C" void app_main(void)
{
    ESP_LOGI("MEM - 前",
        "Internal RAM: %u bytes",
        heap_caps_get_free_size(
            MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
        ));
    i2c_master_bus_handle_t i2c_bus_handle = bsp_i2c_init();

    bsp_display_init(&io_handle, &panel_handle, LCD_BUFFER_SIZE);
	
    bsp_display_brightness_init();
    bsp_display_set_brightness(100);
    bsp_touch_init(i2c_bus_handle, EXAMPLE_LCD_V_RES,EXAMPLE_LCD_H_RES , 1);
    lv_port_init();
    
        ESP_LOGI("MEM - 后",
        "Internal RAM: %u bytes",
        heap_caps_get_free_size(
            MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
        ));
    
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

	// WiFi初始化
    // wifi_manager_init(); 
    // 蓝牙初始化
    BluetoothManager::instance().init("BuBu-Aquarium-Monitor");
	
    if (lvgl_port_lock(0))
    {
        ui_init();
        lvgl_port_unlock();
    }
    
    while (1) {
        if(lvgl_port_lock(100))
        {
            ui_tick();
            lvgl_port_unlock();
        }
	    vTaskDelay(pdMS_TO_TICKS(25));
	    // monitor_memory();        
	}
}



static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    touch_data_t touch_data;
    /*Save the pressed coordinates and the state*/
    bsp_touch_read();
    if (bsp_touch_get_coordinates(&touch_data))
    {
        last_x = touch_data.coords[0].x;
        last_y = touch_data.coords[0].y;
        data->state = LV_INDEV_STATE_PR;
        //printf("x: %d, y: %d\n", last_x, last_y);
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}

void lv_port_init(void)
{
    lvgl_port_cfg_t port_cfg = {};

    port_cfg.task_priority = 4;
    port_cfg.task_stack = 1024 * 5;
    port_cfg.task_affinity = 1;
    port_cfg.task_max_sleep_ms = 500;
    port_cfg.timer_period_ms = 5;
    lvgl_port_init(&port_cfg);

    lvgl_port_display_cfg_t disp_cfg = {};
    disp_cfg.io_handle = io_handle;
    disp_cfg.panel_handle = panel_handle;
    disp_cfg.buffer_size = LCD_BUFFER_SIZE;
    disp_cfg.sw_rotate = EXAMPLE_DISPLAY_ROTATION;
    disp_cfg.hres = EXAMPLE_LCD_H_RES;
    disp_cfg.vres = EXAMPLE_LCD_V_RES;
    disp_cfg.trans_size = LCD_BUFFER_SIZE / 10;
    disp_cfg.draw_wait_cb = NULL;
    disp_cfg.flags.buff_dma = false;
    disp_cfg.flags.buff_spiram = true;

    if (disp_cfg.sw_rotate == LV_DISP_ROT_180 || disp_cfg.sw_rotate == LV_DISP_ROT_NONE)
    {
        disp_cfg.hres = EXAMPLE_LCD_H_RES;
        disp_cfg.vres = EXAMPLE_LCD_V_RES;
    }
    else
    {
        disp_cfg.hres = EXAMPLE_LCD_V_RES;
        disp_cfg.vres = EXAMPLE_LCD_H_RES;
    }

    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    /* ⭐ 关键修复 */
    // lv_disp_set_rotation(lvgl_disp, LV_DISP_ROT_90);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;

    lvgl_touch_indev = lv_indev_drv_register(&indev_drv);
}

