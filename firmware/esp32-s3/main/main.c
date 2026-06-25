#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "lcd_st77922.h"
#include "lvgl_ui.h"

static const char *TAG = "APP";

void app_main(void)
{
    ESP_LOGI(TAG, "System start, heap=%lu", (unsigned long)esp_get_free_heap_size());

    // =========================
    // 1. LCD INIT
    // =========================
    esp_lcd_panel_handle_t panel = lcd_init_st77922();

    // =========================
    // 2. LVGL INIT
    // =========================
    lvgl_port_cfg_t cfg = ESP_LVGL_PORT_INIT_CONFIG();
    cfg.task_priority = 4;   // ⭐稳定性提升
    ESP_ERROR_CHECK(lvgl_port_init(&cfg));

    // =========================
    // 3. DISPLAY CONFIG
    // =========================
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io,          // OK（esp_lvgl_port v1兼容）
        .panel_handle = panel,

        // ⭐关键：建议PSRAM分配
        .buffer_size = 320 * 40,

        .hres = 320,
        .vres = 480,

        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
    };

    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    if (!disp) {
        ESP_LOGE(TAG, "LVGL display init failed");
        return;
    }

    // =========================
    // 4. UI INIT（必须加锁）
    // =========================
    lvgl_port_lock(0);
    ui_init();
    lvgl_port_unlock();

    ESP_LOGI(TAG, "UI started, heap=%lu", (unsigned long)esp_get_free_heap_size());

    // =========================
    // 5. MAIN LOOP
    // =========================
    while (true) {
        ESP_LOGI(TAG, "running heap=%lu", (unsigned long)esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}