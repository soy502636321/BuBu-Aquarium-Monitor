//
// Created by Hu on 2026/8/9.
//

#ifndef STM32F103C8TX_SENSORMANAGER_HPP
#define STM32F103C8TX_SENSORMANAGER_HPP

#include "ISensor.hpp"
#include "SensorData.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <cstdint>
#include <cstring>

#define MAX_SENSORS          8
#define SENSOR_DATA_QUEUE_LEN  16

// ★ 传感器配置（可被上位机修改）★
struct SensorConfig {
    char name[16];
    uint32_t interval_ms;        // 采集周期（毫秒）
    bool enabled;                // 是否启用
    uint32_t last_run_tick;      // 上次执行时间
};

class SensorManager {
public:
    static SensorManager& getInstance();

    // -------- 初始化 --------
    void init();

    // -------- 注册传感器（使用默认周期） --------
    bool registerSensor(ISensor* sensor, uint32_t default_interval_ms);

    // -------- 启动采集任务 --------
    void start();

    // -------- 获取数据队列 --------
    QueueHandle_t getDataQueue() const { return m_dataQueue; }

    // ============================================================
    // ★ 上位机可调用的 API ★
    // ============================================================

    // 设置传感器采集周期
    bool setInterval(const char* name, uint32_t interval_ms);

    // 启用/禁用传感器
    bool setEnabled(const char* name, bool enabled);

    // 获取传感器配置（用于上报上位机）
    bool getConfig(const char* name, SensorConfig& outConfig);

    // 获取所有传感器配置（用于上报上位机）
    void getAllConfigs(SensorConfig* outConfigs, uint8_t* count);

private:
    SensorManager() = default;
    ~SensorManager() = default;
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;

    // -------- 采集任务 --------
    static void collectionTaskEntry(void* pvParameters);
    void collectionTaskLoop();

    // -------- 采集单个传感器 --------
    void readSensor(uint8_t index);

    // -------- 查找传感器 --------
    int findSensor(const char* name) const;

private:
    // 传感器实例指针
    ISensor* m_sensors[MAX_SENSORS];
    uint8_t m_sensorCount = 0;

    // ★ 传感器配置（独立于传感器实例）★
    SensorConfig m_configs[MAX_SENSORS];

    // 数据队列
    QueueHandle_t m_dataQueue = nullptr;
    TaskHandle_t m_taskHandle = nullptr;
    bool m_initialized = false;
};

#endif //STM32F103C8TX_SENSORMANAGER_HPP