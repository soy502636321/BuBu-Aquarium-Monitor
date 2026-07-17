/*
 * 只读，提供硬件信息
 *
 *  Created on: 2026年7月17日
 *      Author: Hu
 */

#ifndef COMPONENTS_HOST_INFO_INCLUDE_HOST_INFO_H_
#define COMPONENTS_HOST_INFO_INCLUDE_HOST_INFO_H_

// Host_info.h
#pragma once

#include <string>
#include <cstdint>
#include "esp_mac.h"
#include "esp_chip_info.h"
#include "esp_system.h"

// ============================================================================
// ★★★ 设备信息类（单例，只读）★★★
// ============================================================================

class HostInfo {
public:
    // -------- 单例访问 --------
    static HostInfo& instance() {
        static HostInfo instance;
        return instance;
    }

    // 禁止拷贝
    HostInfo(const HostInfo&) = delete;
    HostInfo& operator=(const HostInfo&) = delete;

    // ========================================================================
    // ★★★ MAC 地址 ★★★
    // ========================================================================

    /**
     * @brief 获取 MAC 地址（不带冒号）
     * @return 格式: A1B2C3D4E5F6
     */
    const std::string& getMacAddress() const {
        return macAddress;
    }

    /**
     * @brief 获取 MAC 地址（带冒号）
     * @return 格式: A1:B2:C3:D4:E5:F6
     */
    const std::string& getMacAddressWithColon() const {
        return macAddressWithColon;
    }

    // ========================================================================
    // ★★★ 设备标识 ★★★
    // ========================================================================

    /**
     * @brief 获取设备唯一 ID
     * @return 格式: BAM_A1B2C3D4E5F6
     */
    const std::string getHostId() const {
        return macAddress;
    }

    /**
     * @brief 获取短设备 ID（MAC 后 6 位）
     * @return 格式: A1B2C3
     */
    std::string getShortHostId() const {
        return macAddress.substr(6, 6);
    }

    /**
     * @brief 获取设备型号
     */
    const std::string getHostModel() const {
        return "ESP32-S3";
    }

    /**
     * @brief 获取产品名称
     */
    const std::string getProductName() const {
        return "BuBu-Aquarium-Monitor";
    }

    // ========================================================================
    // ★★★ 芯片信息 ★★★
    // ========================================================================

    /**
     * @brief 获取芯片型号
     */
    const std::string& getChipModel() const {
        return m_chipModel;
    }

    /**
     * @brief 获取芯片核心数
     */
    int getCpuCores() const {
        return m_cpuCores;
    }

    /**
     * @brief 获取 Flash 大小（MB）
     */
    int getFlashSizeMB() const {
        return m_flashSizeMB;
    }

    /**
     * @brief 获取 FreeRTOS 堆栈大小
     */
    size_t getFreeHeapSize() const {
        return esp_get_free_heap_size();
    }

    /**
     * @brief 获取最小空闲堆栈大小
     */
    size_t getMinimumFreeHeapSize() const {
        return esp_get_minimum_free_heap_size();
    }

    // ========================================================================
    // ★★★ 固件信息 ★★★
    // ========================================================================

    /**
     * @brief 获取固件版本
     */
    const std::string& getFirmwareVersion() const {
        return m_firmwareVersion;
    }

    /**
     * @brief 设置固件版本（由构建系统传入）
     */
    void setFirmwareVersion(const std::string& version) {
        m_firmwareVersion = version;
    }

    // ========================================================================
    // ★★★ 格式化输出 ★★★
    // ========================================================================

    /**
     * @brief 获取设备信息摘要（多行）
     */
    std::string getInfo() const;

private:
    // -------- 私有构造函数（自动初始化）--------
    HostInfo();

    // -------- 内部初始化方法 --------
    void initMacAddress();
    void initChipInfo();

    // -------- 成员变量 --------
    // MAC 地址
    std::string macAddress;
    std::string macAddressWithColon;

    // 芯片信息
    std::string m_chipModel;
    int m_cpuCores = 0;
    int m_flashSizeMB = 0;

    // 固件信息
    std::string m_firmwareVersion = "1.0.0";
};

#endif /* COMPONENTS_HOST_INFO_INCLUDE_HOST_INFO_H_ */
