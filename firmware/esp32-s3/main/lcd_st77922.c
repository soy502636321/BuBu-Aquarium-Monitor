#include "lcd_st77922.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

static const char *TAG = "LCD";

esp_lcd_panel_handle_t lcd_init_st77922(void)
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_LOGI(TAG, "LCD init start, free heap=%lu", (unsigned long)esp_get_free_heap_size());

    // =========================
    // 1. SPI BUS CONFIG（关键修复）
    // =========================
    spi_bus_config_t buscfg = {
        .mosi_io_num = 11,
        .miso_io_num = -1,
        .sclk_io_num = 12,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,

        // ⭐关键：必须限制，否则 queue + DMA 爆内存
        .max_transfer_sz = 64 * 1024,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // =========================
    // 2. LCD IO CONFIG（关键修复）
    // =========================
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = 10,
        .dc_gpio_num = 9,

        // ⭐建议先降频稳定
        .pclk_hz = 20 * 1000 * 1000,

        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,

        // ⭐关键：避免 queue 默认过大
        .trans_queue_depth = 2,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &io_handle));

    // =========================
    // 3. PANEL CONFIG
    // =========================
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 8,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };

    // ST77922 用 ST7789 驱动兼容
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // =========================
    // 4. INIT SEQUENCE
    // =========================
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "LCD init done, free heap=%lu", (unsigned long)esp_get_free_heap_size());

    return panel_handle;
}