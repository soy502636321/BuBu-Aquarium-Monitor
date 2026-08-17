// SensorManager.hpp
#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include "GpioDetector.hpp"
#include "SensorDevice.hpp"
#include "DataGateway.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <cstdint>
#include <cstring>
#include "SensorConfig.hpp"

#define MAX_SENSORS             4
#define MONITOR_INTERVAL_MS     (5 * 1000)
#define COLLECT_CHECK_INTERVAL_MS   500

class SensorManager {
public:
    static SensorManager& getInstance();

    // -------- 初始化 --------
    void init();

    void loadDefaultConfig();

    // -------- 注册传感器 --------
    bool registerSensor(SensorDevice* sensor,
                        GPIO_TypeDef* detectPort = nullptr,
                        uint16_t detectPin = 0,
                        bool activeHigh = true);

    // -------- ★ 上位机主动采集 ★ --------
    bool manualCollect(const char* name);

    // -------- 上位机配置 --------
    bool setInterval(const char* name, uint32_t interval_ms);
    bool setEnabled(const char* name, bool enabled);
    SensorDevice* getSensor(const char* name) const;

private:
    SensorManager() = default;
    ~SensorManager() = default;
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;

    // -------- 内部函数 --------
    int findSensor(const char* name) const;

    // ★ 监控任务 ★
    static void monitorTaskEntry(void* pvParameters);
    void monitorTaskLoop();

    // ★ 定时自动采集任务 ★
    static void collectTaskEntry(void* pvParameters);
    void collectTaskLoop();

    // ★ 手动采集任务 ★
    static void collectWorkerEntry(void* pvParameters);
    void collectWorkerLoop();

    bool isPinInserted(GPIO_TypeDef* port, uint16_t pin, bool activeHigh);

    // ★ GPIO 检测 ★
    void checkAllGpio();

    // ★ 自动采集检查 ★
    void checkAutoCollect();

    void createSensor(uint8_t pinIndex);
    SensorDevice* createSensorByType(SensorType type, const char* name, GPIO_TypeDef* port, uint16_t pin);

    // ★ ★ 执行采集 ★ ★
    void doCollect(uint8_t index, bool isManual);

    // ★ ★ 采集完成回调（由传感器调用）★ ★
    static void onSensorDataReady(void* arg, const DeviceRecord* data);
    void handleSensorData(const DeviceRecord* data);


private:
    struct SensorEntry {
        SensorDevice* sensor;
        GpioDetector* detector;
        uint32_t lastCollectTick;
        bool lastOnlineState;
        bool isCollecting;
    };

    // ★ ★ 采集请求 ★ ★
    struct CollectRequest {
        uint8_t sensorIndex;
        bool isManual;
    };

    // ★ 配置表 ★
    SensorPinConfig m_currentConfig[MAX_SENSORS];
    uint8_t m_configCount = 0;

    // ★ 传感器列表 ★
    SensorEntry m_entries[MAX_SENSORS];
    uint8_t m_sensorCount = 0;

    // ★ ★ 采集队列 ★ ★
    QueueHandle_t m_collectQueue = nullptr;

    // 定时检查GPIO插入任务句柄
    TaskHandle_t m_monitorTask = nullptr;
    // 定时传感器采集任务句柄
    TaskHandle_t m_collectTask = nullptr;
    // 主动采集传感器任务句柄
    TaskHandle_t m_workerTask = nullptr;
    bool m_initialized = false;
};

#endif