/*
 * sd_card_manager.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_SD_CARD_MANAGER_INCLUDE_SD_CARD_MANAGER_HPP_
#define COMPONENTS_SD_CARD_MANAGER_INCLUDE_SD_CARD_MANAGER_HPP_

#include <string>
#include <vector>
#include "esp_err.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

// 挂载点路径
#define SD_MOUNT_POINT "/sdcard"

// SD卡工作模式
enum class SDMode {
    SPI_MODE,      // SPI模式（兼容性更好）
    SDMMC_MODE     // SDMMC模式（速度更快，需要特定引脚）
};

class SDCardManager {
public:
    // 获取单例实例
    static SDCardManager& getInstance();

    // 禁止拷贝和赋值
    SDCardManager(const SDCardManager&) = delete;
    SDCardManager& operator=(const SDCardManager&) = delete;

    // 初始化SD卡
    bool init(SDMode mode = SDMode::SDMMC_MODE, int max_freq_khz = 20000);
    
    // 反初始化，卸载SD卡
    void deinit();

    // 检查SD卡是否已挂载
    bool isMounted() const { return m_is_mounted; }

    // ========== 文件读写操作 ==========

    // 写入文件（覆盖写入）
    // bool writeFile(const std::string& filepath, const std::string& data);
    // bool writeFile(const std::string& filepath, const std::vector<uint8_t>& data);
    
    // 追加写入
    // bool appendFile(const std::string& filepath, const std::string& data);
    
    // 读取文件
    // bool readFile(const std::string& filepath, std::string& out_data);
    // bool readFileBinary(const std::string& filepath, std::vector<uint8_t>& out_data);
    
    // 检查文件是否存在
    // bool fileExists(const std::string& filepath);
    
    // 删除文件
    // bool deleteFile(const std::string& filepath);

    // ========== 目录操作 ==========
    
    // 创建目录（支持多级）
    // bool createDirectory(const std::string& dirpath);
    
    // 列出目录下的所有文件
    // std::vector<std::string> listFiles(const std::string& dirpath);
    
    // 删除目录（递归删除）
    // bool deleteDirectory(const std::string& dirpath);

    // ========== 存储信息 ==========
    
    // 获取总容量（字节）
    uint64_t getTotalBytes();
    
    // 获取剩余空间（字节）
    uint64_t getFreeBytes();

    // 获取错误信息
    std::string getLastError() const { return m_last_error; }

private:
    // 私有构造函数
    SDCardManager();
    ~SDCardManager();

    // 挂载SD卡
    bool mountSDCard(SDMode mode, int max_freq_khz);
    
    // 卸载SD卡
    void unmountSDCard();

private:
    static SDCardManager* m_instance;
    
    sdmmc_card_t* m_card;          // SD卡句柄
    bool m_is_mounted;             // 挂载状态
    std::string m_last_error;      // 最后错误信息
    
    // 挂载配置
    sdmmc_host_t m_host;           // 主机配置
    sdmmc_slot_config_t m_slot_config; // 槽配置
};

#endif /* COMPONENTS_SD_CARD_MANAGER_INCLUDE_SD_CARD_MANAGER_HPP_ */
