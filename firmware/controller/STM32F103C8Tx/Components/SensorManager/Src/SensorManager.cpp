// SensorManager.cpp
#include "SensorManager.hpp"
#include "Logger.hpp"

SensorManager& SensorManager::getInstance() {
    static SensorManager instance;
    return instance;
}

// ============================================================
// ★ 初始化 ★
// ============================================================
void SensorManager::init() {
    if (m_initialized) return;

    // 清空传感器列表
    m_sensorCount = 0;
    memset(m_sensors, 0, sizeof(m_sensors));
    memset(m_configs, 0, sizeof(m_configs));

    // 创建数据队列
    m_dataQueue = xQueueCreate(SENSOR_DATA_QUEUE_LEN, sizeof(SensorData));
    if (m_dataQueue == nullptr) {
        LOG_ERROR("Sensor data queue creation failed!");
        return;
    }

    m_initialized = true;
    LOG_INFO("SensorManager initialized");
}

// ============================================================
// ★ 注册传感器 ★
// ============================================================
bool SensorManager::registerSensor(ISensor* sensor, uint32_t default_interval_ms) {
    if (sensor == nullptr) return false;
    if (m_sensorCount >= MAX_SENSORS) {
        LOG_ERROR("Too many sensors! Max: %d", MAX_SENSORS);
        return false;
    }

    // 初始化传感器
    if (!sensor->init()) {
        LOG_ERROR("Sensor %s init failed!", sensor->getName());
        return false;
    }

    // 保存传感器
    m_sensors[m_sensorCount] = sensor;

    // ★ 保存配置 ★
    strncpy(m_configs[m_sensorCount].name, sensor->getName(), 15);
    m_configs[m_sensorCount].name[15] = '\0';
    m_configs[m_sensorCount].interval_ms = default_interval_ms;
    m_configs[m_sensorCount].enabled = true;
    m_configs[m_sensorCount].last_run_tick = 0;

    LOG_INFO("Sensor registered: %s, interval: %lu ms",
             sensor->getName(), default_interval_ms);

    m_sensorCount++;
    return true;
}

// ============================================================
// ★ 启动采集任务 ★
// ============================================================
void SensorManager::start() {
    if (m_taskHandle == nullptr) {
        BaseType_t ret = xTaskCreate(
            collectionTaskEntry,
            "SensorCollect",
            512,
            this,
            1,  // 低优先级
            &m_taskHandle
        );
        if (ret != pdPASS) {
            LOG_ERROR("Sensor collection task creation failed!");
            return;
        }
    }
    LOG_INFO("Sensor collection started");
}

// ============================================================
// ★ 采集任务循环 ★
// ============================================================
void SensorManager::collectionTaskLoop() {
    LOG_INFO("Sensor collection task running");

    while (1) {
        // ★ 遍历所有传感器 ★
        for (uint8_t i = 0; i < m_sensorCount; i++) {
            if (m_configs[i].enabled) {
                readSensor(i);
            }
        }

        // ★ 每 100ms 检查一次 ★
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ============================================================
// ★ 采集单个传感器 ★
// ============================================================
void SensorManager::readSensor(uint8_t index) {
    uint32_t now = xTaskGetTickCount();
    uint32_t interval_ticks = pdMS_TO_TICKS(m_configs[index].interval_ms);

    // 检查是否到达采集时间
    if (now - m_configs[index].last_run_tick >= interval_ticks) {
        m_configs[index].last_run_tick = now;

        ISensor* sensor = m_sensors[index];
        SensorData data;

        if (sensor->read(data)) {
            // ★ 数据入队 ★
            if (xQueueSend(m_dataQueue, &data, 0) != pdPASS) {
                LOG_WARN("Data queue full: %s", sensor->getName());
            }
            LOG_DEBUG("Sensor %s collected", sensor->getName());
        } else {
            LOG_WARN("Sensor %s read failed", sensor->getName());
        }
    }
}

// ============================================================
// ★ 任务入口（静态） ★
// ============================================================
void SensorManager::collectionTaskEntry(void* pvParameters) {
    auto* self = static_cast<SensorManager*>(pvParameters);
    self->collectionTaskLoop();
}

// ============================================================
// ★ 查找传感器 ★
// ============================================================
int SensorManager::findSensor(const char* name) const {
    for (int i = 0; i < m_sensorCount; i++) {
        if (strcmp(m_configs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// ============================================================
// ★ 上位机 API：设置采集周期 ★
// ============================================================
bool SensorManager::setInterval(const char* name, uint32_t interval_ms) {
    int index = findSensor(name);
    if (index < 0) {
        LOG_WARN("Sensor %s not found", name);
        return false;
    }

    // ★ 周期不能太短（防止频繁采集）★
    if (interval_ms < 100) {
        LOG_WARN("Interval too short: %lu ms, min 100ms", interval_ms);
        interval_ms = 100;
    }

    m_configs[index].interval_ms = interval_ms;
    LOG_INFO("Sensor %s interval set to %lu ms", name, interval_ms);
    return true;
}

// ============================================================
// ★ 上位机 API：启用/禁用 ★
// ============================================================
bool SensorManager::setEnabled(const char* name, bool enabled) {
    int index = findSensor(name);
    if (index < 0) {
        LOG_WARN("Sensor %s not found", name);
        return false;
    }

    m_configs[index].enabled = enabled;
    LOG_INFO("Sensor %s %s", name, enabled ? "enabled" : "disabled");
    return true;
}

// ============================================================
// ★ 上位机 API：获取配置 ★
// ============================================================
bool SensorManager::getConfig(const char* name, SensorConfig& outConfig) {
    int index = findSensor(name);
    if (index < 0) return false;

    outConfig = m_configs[index];
    return true;
}

void SensorManager::getAllConfigs(SensorConfig* outConfigs, uint8_t* count) {
    *count = m_sensorCount;
    for (int i = 0; i < m_sensorCount; i++) {
        outConfigs[i] = m_configs[i];
    }
}