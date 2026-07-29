/*
 * sd_card_manager.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */


#include "sd_card_manager.hpp"
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/gpio.h"
#include "ff.h" // 需要包含 FATFS 库的头文件

static const char* TAG = "BuBu-Aquarium-Monitor[sd_card_manager]";

// 静态成员初始化
SDCardManager* SDCardManager::m_instance = nullptr;

// ==================== 单例管理 ====================

SDCardManager& SDCardManager::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new SDCardManager();
    }
    return *m_instance;
}

// ==================== 构造函数/析构函数 ====================
SDCardManager::SDCardManager() 
    : m_card(nullptr)
    , m_is_mounted(false)
    , m_last_error("") {
    ESP_LOGI(TAG, "SDCardManager created");
}

SDCardManager::~SDCardManager() {
    deinit();
    m_instance = nullptr;
}

// ==================== 初始化 ====================

bool SDCardManager::init(SDMode mode, int max_freq_khz) {
    if (m_is_mounted) {
        ESP_LOGW(TAG, "SD 卡已挂载");
        return true;
    }

    ESP_LOGI(TAG, "正在以 %s 模式初始化 SD 卡，频率：%d KHz", mode == SDMode::SPI_MODE ? "SPI" : "SDMMC", max_freq_khz);

    bool success = mountSDCard(mode, max_freq_khz);
    
    if (success) {
        m_is_mounted = true;
        m_last_error = "";
        ESP_LOGI(TAG, "SD 卡挂载成功");
        
        // 打印存储信息
        uint64_t total = getTotalBytes();
        uint64_t free_bytes = getFreeBytes();
        ESP_LOGI(TAG, "Total: %.2f MB, Free: %.2f MB", 
                 total / (1024.0 * 1024.0),
                 free_bytes / (1024.0 * 1024.0));
    } else {
        m_last_error = "SD 卡挂载失败";
        ESP_LOGE(TAG, "SD 卡挂载失败");
    }

    return success;
}

void SDCardManager::deinit() {
    if (m_is_mounted) {
        unmountSDCard();
        m_is_mounted = false;
        ESP_LOGI(TAG, "SD 卡已卸载");
    }
}

// ==================== 挂载/卸载实现 ====================

bool SDCardManager::mountSDCard(SDMode mode, int max_freq_khz) {
    esp_err_t ret;
    
    if (mode == SDMode::SPI_MODE) {
        // ========== SPI 模式配置（如果使用 SPI 接口） ==========
        // 注意：SPI 模式需要不同的引脚连接，如果你不使用 SPI 模式，这段可以忽略
#ifdef CONFIG_IDF_TARGET_ESP32
        // 仅 ESP32 支持这些引脚
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = GPIO_NUM_23,
            .miso_io_num = GPIO_NUM_19,
            .sclk_io_num = GPIO_NUM_18,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };
#else
        // 其他芯片（ESP32-S3/C3）使用不同的引脚
        // 根据你的实际 SPI 连接修改
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = GPIO_NUM_11,   // ESP32-S3 常用 MOSI
            .miso_io_num = GPIO_NUM_13,   // ESP32-S3 常用 MISO
            .sclk_io_num = GPIO_NUM_12,   // ESP32-S3 常用 SCK
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };
#endif
        
        ret = spi_bus_initialize((spi_host_device_t)SPI2_HOST, &bus_cfg, SDSPI_DEFAULT_DMA);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPI 总线初始化失败：%s", esp_err_to_name(ret));
            return false;
        }

        sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.gpio_cs = GPIO_NUM_5;    // 根据实际 CS 引脚修改
        slot_config.host_id = (spi_host_device_t)SPI2_HOST;
        
        m_slot_config = *(sdmmc_slot_config_t*)&slot_config;
        
        m_host = SDSPI_HOST_DEFAULT();
        m_host.max_freq_khz = max_freq_khz;
        
    } else {
        // ========== SDMMC 模式配置（你的硬件使用此模式） ==========
        
        // 配置 SDMMC 主机
        m_host = SDMMC_HOST_DEFAULT();
        m_host.max_freq_khz = max_freq_khz;  // 建议先使用较低频率（如 20000 = 20MHz）
        
        // 配置 SDMMC 槽位（使用 SLOT_1）
        // 注意：ESP32 有两个 SDMMC 控制器，SLOT_0 通常用于内部 Flash，SLOT_1 用于外部 SD 卡
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        
        // 根据你的硬件配置引脚
        slot_config.gpio_cd = SDMMC_SLOT_NO_CD;     // 不使用卡检测引脚
        slot_config.gpio_wp = SDMMC_SLOT_NO_WP;     // 不使用写保护引脚
        
        // 数据线宽度：4 线模式
        slot_config.width = 4;
        
        // 如果你使用的是 ESP32 的 SDMMC 控制器，引脚通常是固定的：
        // SLOT_1 的默认引脚是：CLK=IO6, CMD=IO11, DATA0=IO7, DATA1=IO8, DATA2=IO9, DATA3=IO10
        // 你的硬件引脚不同，所以需要手动配置
        
        // 手动配置引脚（根据你的硬件）
	    slot_config.clk = GPIO_NUM_5;    // CLK = IO5
	    slot_config.cmd = GPIO_NUM_4;    // CMD = IO4
	    slot_config.d0  = GPIO_NUM_6;    // DATA0 = IO6
	    slot_config.d1  = GPIO_NUM_7;    // DATA1 = IO7
	    slot_config.d2  = GPIO_NUM_2;    // DATA2 = IO2
	    slot_config.d3  = GPIO_NUM_3;    // DATA3 = IO3
        
        // 复制到成员变量
        m_slot_config = slot_config;
    }

    // ========== 挂载 SD 卡 ==========
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
	    .format_if_mount_failed = false,
	    .max_files = 5,
	    .allocation_unit_size = 16 * 1024,
	    .disk_status_check_enable = false,
	    .use_one_fat = false
    };

    ret = esp_vfs_fat_sdmmc_mount(
        SD_MOUNT_POINT,
        &m_host,
        &m_slot_config,
        &mount_config,
        &m_card
    );

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "文件系统挂载失败。如需格式化，请将 format_if_mount_failed 设置为 true");
        } else {
            ESP_LOGE(TAG, "SD 卡初始化失败：%s", esp_err_to_name(ret));
        }
        return false;
    }

    // 打印 SD 卡信息
    sdmmc_card_print_info(stdout, m_card);
    return true;
}

void SDCardManager::unmountSDCard() {
    if (m_card != nullptr) {
        // 卸载和反初始化
        esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, m_card);
        m_card = nullptr;
    }
}


// ==================== 存储信息 ====================
uint64_t SDCardManager::getTotalBytes() {
    if (!m_is_mounted || m_card == nullptr) {
        return 0;
    }
    // 直接使用 SD 卡 CSD 寄存器中的信息
    return (uint64_t)m_card->csd.capacity * m_card->csd.sector_size;
}

uint64_t SDCardManager::getFreeBytes() {
    if (!m_is_mounted) {
        return 0;
    }

	FATFS *fs;
    DWORD fre_clust;
    
    // 获取驱动 0（SD 卡挂载的驱动）的信息
    FRESULT res = f_getfree("0:", &fre_clust, &fs);
    if (res != FR_OK) {
        ESP_LOGE(TAG, "Failed to get free space: %d", res);
        return 0;
    }
    
    // 计算空闲扇区数
    DWORD fre_sect = fre_clust * fs->csize;
    
    // 返回字节数（每扇区 512 字节）
    return (uint64_t)fre_sect * 512;
}

