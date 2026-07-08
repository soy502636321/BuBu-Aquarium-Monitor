#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_log.h"

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

void wifi_task() {
	/*
	ESP_LOGI("MEM", "free heap=%"  PRIu32, esp_get_free_heap_size());
	ESP_LOGI("MEM", "free internal=%zu", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
	ESP_LOGI(TAG, "Start Scan WiFi...");
	ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint16_t number = 10;
    wifi_ap_record_t ap_info[10];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_scan_start(NULL, true);
    ESP_LOGI(TAG, "End Scan WiFi...");
    
    ESP_LOGI(TAG, "Max AP number ap_info can hold = %u", number);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, number);
    for (int i = 0; i < number; i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        ESP_LOGI(TAG, "Channel \t\t%d", ap_info[i].primary);
    }
	*/    
}

extern "C" void app_main(void)
{

    i2c_master_bus_handle_t i2c_bus_handle = bsp_i2c_init();

    bsp_display_init(&io_handle, &panel_handle, LCD_BUFFER_SIZE);
	
    bsp_display_brightness_init();
    bsp_display_set_brightness(100);
    bsp_touch_init(i2c_bus_handle, EXAMPLE_LCD_V_RES,EXAMPLE_LCD_H_RES , 1);
    lv_port_init();
    
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    wifi_manager_init(); // 初始化WiFi
	
    if (lvgl_port_lock(0))
    {
        ui_init();
    }
    
    while (1) {
        lv_timer_handler();  // LVGL刷新
        ui_tick();           // ⭐ Flow核心驱动
        vTaskDelay(pdMS_TO_TICKS(5));        
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

