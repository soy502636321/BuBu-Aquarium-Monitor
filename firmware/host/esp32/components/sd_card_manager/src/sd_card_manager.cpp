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

// ==================== 文件操作 ====================

std::string SDCardManager::getFullPath(const std::string& relative_path) const {
    if (relative_path.empty()) {
        return SD_MOUNT_POINT;
    }
    
    // 如果路径已经以挂载点开头，直接返回
    if (relative_path.find(SD_MOUNT_POINT) == 0) {
        return relative_path;
    }
    
    // 确保路径格式正确
    std::string full_path = SD_MOUNT_POINT;
    if (relative_path[0] != '/') {
        full_path += "/";
    }
    full_path += relative_path;
    
    return full_path;
}

bool SDCardManager::writeFile(const std::string& filepath, const std::string& data) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    
    // 确保目录存在
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string dir = full_path.substr(0, last_slash);
        createDirectory(dir);
    }

    FILE* file = fopen(full_path.c_str(), "w");
    if (file == nullptr) {
        m_last_error = "打开文件写入失败：" + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    size_t written = fwrite(data.c_str(), 1, data.length(), file);
    fclose(file);

    if (written != data.length()) {
		m_last_error = "数据未完全写入";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "File written: %s (%zu bytes)", full_path.c_str(), written);
    return true;
}

bool SDCardManager::writeFile(const std::string& filepath, const std::vector<uint8_t>& data) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    
    // 确保目录存在
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string dir = full_path.substr(0, last_slash);
        createDirectory(dir);
    }

    FILE* file = fopen(full_path.c_str(), "wb");
    if (file == nullptr) {
        m_last_error = "Failed to open file for writing: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    size_t written = fwrite(data.data(), 1, data.size(), file);
    fclose(file);

    if (written != data.size()) {
        m_last_error = "Failed to write complete data";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Binary file written: %s (%zu bytes)", full_path.c_str(), written);
    return true;
}

bool SDCardManager::appendFile(const std::string& filepath, const std::string& data) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    
    // 确保目录存在
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string dir = full_path.substr(0, last_slash);
        createDirectory(dir);
    }

    FILE* file = fopen(full_path.c_str(), "a");
    if (file == nullptr) {
        m_last_error = "Failed to open file for append: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    size_t written = fwrite(data.c_str(), 1, data.length(), file);
    fclose(file);

    if (written != data.length()) {
        m_last_error = "Failed to append complete data";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "File appended: %s (%zu bytes)", full_path.c_str(), written);
    return true;
}

bool SDCardManager::readFile(const std::string& filepath, std::string& out_data) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    
    FILE* file = fopen(full_path.c_str(), "r");
    if (file == nullptr) {
        m_last_error = "Failed to open file for reading: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0) {
        fclose(file);
        out_data.clear();
        return true; // 空文件也算成功
    }

    // 分配内存并读取
    out_data.resize(size);
    size_t read_bytes = fread(&out_data[0], 1, size, file);
    fclose(file);

    if (read_bytes != (size_t)size) {
        m_last_error = "Failed to read complete file";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "File read: %s (%zu bytes)", full_path.c_str(), read_bytes);
    return true;
}

bool SDCardManager::readFileBinary(const std::string& filepath, std::vector<uint8_t>& out_data) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    
    FILE* file = fopen(full_path.c_str(), "rb");
    if (file == nullptr) {
        m_last_error = "Failed to open file for reading: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0) {
        fclose(file);
        out_data.clear();
        return true;
    }

    // 分配内存并读取
    out_data.resize(size);
    size_t read_bytes = fread(out_data.data(), 1, size, file);
    fclose(file);

    if (read_bytes != (size_t)size) {
        m_last_error = "Failed to read complete binary file";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Binary file read: %s (%zu bytes)", full_path.c_str(), read_bytes);
    return true;
}

bool SDCardManager::fileExists(const std::string& filepath) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    struct stat st;
    return (stat(full_path.c_str(), &st) == 0);
}

bool SDCardManager::deleteFile(const std::string& filepath) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(filepath);
    if (unlink(full_path.c_str()) != 0) {
        m_last_error = "Failed to delete file: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "File deleted: %s", full_path.c_str());
    return true;
}

// ==================== 目录操作 ====================

bool SDCardManager::createDirectory(const std::string& dirpath) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(dirpath);
    
    // 递归创建目录
    std::string path = "";
    size_t pos = 0;
    while ((pos = full_path.find('/', pos + 1)) != std::string::npos) {
        std::string sub_path = full_path.substr(0, pos);
        struct stat st;
        if (stat(sub_path.c_str(), &st) != 0) {
            if (mkdir(sub_path.c_str(), 0777) != 0) {
                m_last_error = "Failed to create directory: " + sub_path;
                ESP_LOGE(TAG, "%s", m_last_error.c_str());
                return false;
            }
            ESP_LOGD(TAG, "Directory created: %s", sub_path.c_str());
        }
    }

    // 创建最后一级目录
    struct stat st;
    if (stat(full_path.c_str(), &st) != 0) {
        if (mkdir(full_path.c_str(), 0777) != 0) {
            m_last_error = "Failed to create directory: " + full_path;
            ESP_LOGE(TAG, "%s", m_last_error.c_str());
            return false;
        }
        ESP_LOGI(TAG, "Directory created: %s", full_path.c_str());
    }

    return true;
}

std::vector<std::string> SDCardManager::listFiles(const std::string& dirpath) {
    std::vector<std::string> files;
    
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return files;
    }

    std::string full_path = getFullPath(dirpath);
    
    DIR* dir = opendir(full_path.c_str());
    if (dir == nullptr) {
        m_last_error = "Failed to open directory: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        files.push_back(entry->d_name);
        ESP_LOGD(TAG, "Found file: %s", entry->d_name);
    }

    closedir(dir);
    ESP_LOGI(TAG, "Listed %zu files in %s", files.size(), full_path.c_str());
    return files;
}

bool SDCardManager::deleteDirectory(const std::string& dirpath) {
    if (!m_is_mounted) {
        m_last_error = "SD card not mounted";
        return false;
    }

    std::string full_path = getFullPath(dirpath);
    
    // 先删除所有子文件和子目录
    DIR* dir = opendir(full_path.c_str());
    if (dir == nullptr) {
        m_last_error = "Failed to open directory: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string item_path = full_path + "/" + entry->d_name;
        struct stat st;
        if (stat(item_path.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                // 递归删除子目录
                deleteDirectory(item_path);
            } else {
                // 删除文件
                unlink(item_path.c_str());
            }
        }
    }
    closedir(dir);

    // 删除空目录
    if (rmdir(full_path.c_str()) != 0) {
        m_last_error = "Failed to delete directory: " + full_path;
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Directory deleted: %s", full_path.c_str());
    return true;
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

