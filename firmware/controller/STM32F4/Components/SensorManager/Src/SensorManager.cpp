// SensorManager.cpp
#include "SensorManager.hpp"

#include "DHT11Sensor.hpp"
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
    // 1. 清空所有条目
    m_sensorCount = 0;
    m_configCount = 0;
    memset(m_entries, 0, sizeof(m_entries));
    memset(m_currentConfig, 0, sizeof(m_currentConfig));

    // 2. ★ ★ 加载默认配置 ★ ★
    loadDefaultConfig();

    m_collectQueue = xQueueCreate(8, sizeof(CollectRequest));
    if (m_collectQueue == nullptr) {
        LOG_ERROR("Collect queue creation failed!");
        return;
    }

    BaseType_t ret;
    ret = xTaskCreate(monitorTaskEntry, "SensorMonitor", 256, this, 1, &m_monitorTask);
    if (ret != pdPASS) {
        LOG_ERROR("Sensor Monitor task creation failed!");
        return;
    }
    LOG_INFO("Sensor Monitor task created OK");

    ret = xTaskCreate(collectTaskEntry, "SensorCollect", 512, this, 1, &m_collectTask);
    if (ret != pdPASS) {
        LOG_ERROR("Sensor Collect task creation failed!");
        return;
    }
    LOG_INFO("Sensor Collect task created OK");
    ret = xTaskCreate(collectWorkerEntry, "CollectWorker", 512, this, 2, &m_workerTask);
    if (ret != pdPASS) {
        LOG_ERROR("Collect Worker task creation failed!");
        return;
    }
    LOG_INFO("Collect Worker task created OK");
    LOG_INFO("Collect Worker task created OK, handle: 0x%p", m_workerTask);

    m_initialized = true;
    LOG_INFO("SensorManager initialized");
}

// ============================================================
// ★ ★ 加载默认配置 ★ ★
// ============================================================
void SensorManager::loadDefaultConfig() {
    m_configCount = SENSOR_CONFIG_COUNT;
    memcpy(m_currentConfig, DEFAULT_SENSOR_CONFIG, m_configCount * sizeof(SensorPinConfig));
    LOG_INFO("Default config loaded, %d pins configured", m_configCount);
}

// ============================================================
// ★ 注册传感器 ★
// ============================================================
bool SensorManager::registerSensor(SensorDevice* sensor,
                                   GPIO_TypeDef* detectPort,
                                   uint16_t detectPin,
                                   bool activeHigh) {
    if (sensor == nullptr) return false;
    if (m_sensorCount >= MAX_SENSORS) {
        LOG_ERROR("Too many sensors! Max: %d", MAX_SENSORS);
        return false;
    }

    if (!sensor->init()) {
        LOG_ERROR("Sensor %s init failed!", sensor->getName());
        return false;
    }

    // ★ 设置采集完成回调 ★
    sensor->setDataCallback(onSensorDataReady, this);

    SensorEntry* entry = &m_entries[m_sensorCount];
    entry->sensor = sensor;
    entry->detector = nullptr;
    entry->lastCollectTick = 0;
    entry->lastOnlineState = false;
    entry->isCollecting = false;

    if (detectPort != nullptr && detectPin != 0) {
        auto* detector = new GpioDetector();
        if (detector->init(sensor->getName(), detectPort, detectPin, activeHigh)) {
            entry->detector = detector;
            LOG_INFO("Sensor %s: GPIO detector enabled", sensor->getName());
        } else {
            delete detector;
        }
    }

    // entry->lastOnlineState = sensor->isOnline();

    LOG_INFO("Sensor %s registered", sensor->getName());
    m_sensorCount++;
    return true;
}

// ============================================================
// ★ 监控任务 ★
// ============================================================
void SensorManager::monitorTaskEntry(void* pvParameters) {
    auto* self = static_cast<SensorManager*>(pvParameters);
    self->monitorTaskLoop();
}

void SensorManager::monitorTaskLoop() {
    while (1) {
        LOG_INFO("那我调用了也没用呀\r\n");
        checkAllGpio();
        vTaskDelay(pdMS_TO_TICKS(MONITOR_INTERVAL_MS));
    }
}

// ============================================================
// ★ 采集调度任务 ★
// ============================================================
void SensorManager::collectTaskEntry(void *pvParameters) {
    auto* self = static_cast<SensorManager*>(pvParameters);
    self->collectTaskLoop();
}

void SensorManager::collectTaskLoop() {
    while (1) {
        checkAutoCollect();
        vTaskDelay(pdMS_TO_TICKS(COLLECT_CHECK_INTERVAL_MS));
    }
}

void SensorManager::collectWorkerEntry(void *pvParameters) {
    printf("!!! 是我collectWorkerEntry ENTERED !!!\r\n");  // ★ 第一行
    auto* self = static_cast<SensorManager*>(pvParameters);
    self->collectWorkerLoop();
}

void SensorManager::collectWorkerLoop() {
    LOG_INFO("CollectWorker task STARTED!\r\n");
    CollectRequest request;
    while (1) {
        // ★ ★ 阻塞等待采集请求 ★ ★
        if (xQueueReceive(m_collectQueue, &request, portMAX_DELAY) == pdTRUE) {
            SensorEntry* entry = &m_entries[request.sensorIndex];
            SensorDevice* sensor = entry->sensor;
            LOG_INFO("开始传感器采集!\r\n");
            if (!entry->lastOnlineState) {
                LOG_WARN("Sensor %s offline", sensor->getName());
                entry->isCollecting = false;
                continue;
            }

            if (entry->isCollecting) {
                LOG_WARN("Sensor %s already collecting", sensor->getName());
                continue;
            }

            entry->isCollecting = true;
            LOG_DEBUG("Sensor %s: collecting (manual=%d)", sensor->getName(), request.isManual);

            // ★ ★ 执行采集（异步，完成后回调） ★ ★
            sensor->readAsync();
        }
    }
}

// ============================================================
// ★ ★ 检测 GPIO 是否插入 ★ ★
// ============================================================
bool SensorManager::isPinInserted(GPIO_TypeDef* port, uint16_t pin, bool activeHigh) {
    if (port == nullptr) return false;

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = pin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = activeHigh ? GPIO_PULLDOWN : GPIO_PULLUP;
    HAL_GPIO_Init(port, &gpio);

    uint8_t level = HAL_GPIO_ReadPin(port, pin);
    return activeHigh ? (level == GPIO_PIN_SET) : (level == GPIO_PIN_RESET);
}


// ============================================================
// ★ GPIO 检测 ★
// ============================================================
void SensorManager::checkAllGpio() {
    LOG_INFO("SensorManager::checkAllGpio()\r\n");
    for (uint8_t i = 0; i < m_configCount; i++) {
        SensorPinConfig* cfg = &m_currentConfig[i];
        if (cfg->type == SensorType::NONE || !cfg->enabled) continue;
        bool currentOnline = isPinInserted(cfg->port, cfg->pin, cfg->activeHigh);

        // 检查是否已有对应的传感器
        bool hasEntry = false;
        uint8_t entryIndex = 0;
        for (uint8_t j = 0; j < m_sensorCount; j++) {
            if (m_entries[j].sensor != nullptr) {
                if (strcmp(m_entries[j].sensor->getName(), cfg->name) == 0) {
                    hasEntry = true;
                    entryIndex = j;
                    break;
                }
            }
        }

        // ★ 插入 → 创建 ★
        if (currentOnline && !hasEntry) {
            LOG_INFO("GPIO: %s INSERTED", cfg->name);
            createSensor(i);
        }
        // ★ 拔出 → 销毁 ★
        else if (!currentOnline && hasEntry) {
            LOG_INFO("GPIO: %s REMOVED", cfg->name);
            // destroySensor(entryIndex);
        }
    }
}

// ============================================================
// ★ 自动采集检查 ★
// ============================================================
void SensorManager::checkAutoCollect() {
    uint32_t now = xTaskGetTickCount();

    // LOG_INFO("检查所有的传感器[%d]\r\n", m_sensorCount);
    for (uint8_t i = 0; i < m_sensorCount; i++) {
        SensorEntry* entry = &m_entries[i];
        SensorDevice* sensor = entry->sensor;

        // if (!sensor->isEnabled()) continue;
        // if (!entry->lastOnlineState) continue;
        // if (entry->isCollecting) continue;
        uint32_t interval_ticks = pdMS_TO_TICKS(sensor->getInterval());

        // LOG_DEBUG("Sensor %s: interval = %lu ms, ticks = %lu, now = %lu",sensor->getName(), sensor->getInterval(), interval_ticks, now);
        if (now - entry->lastCollectTick >= interval_ticks) {
                entry->lastCollectTick = now;
            LOG_INFO("提交采集需求\r\n");
                doCollect(i, false);
        }
    }
}

// ============================================================
// ★ ★ 执行采集 ★ ★
// ============================================================
void SensorManager::doCollect(uint8_t index, bool isManual) {
    printf("SensorManager::doCollect - 1\r\n");
    if (index >= m_sensorCount) return;
    printf("SensorManager::doCollect - 2\r\n");
    if (m_collectQueue == nullptr) return;
    printf("SensorManager::doCollect - 3\r\n");

    SensorEntry* entry = &m_entries[index];
    printf("SensorManager::doCollect - 4\r\n");
    if (entry->isCollecting) {
        printf("SensorManager::doCollect - 5\r\n");
        LOG_DEBUG("Sensor %s already collecting, skip", entry->sensor->getName());
        return;
    }
    printf("SensorManager::doCollect - 6\r\n");
    CollectRequest req;
    req.sensorIndex = index;
    req.isManual = isManual;
    printf("SensorManager::doCollect - 7\r\n");
    // ★ ★ 非阻塞入队 ★ ★
    if (xQueueSend(m_collectQueue, &req, pdMS_TO_TICKS(100)) != pdPASS) {
        printf("SensorManager::doCollect - 8\r\n");
        LOG_WARN("Collect queue full! Sensor: %s", entry->sensor->getName());
    }
    printf("SensorManager::doCollect - 9\r\n");
    // LOG_INFO("传感器采集\r\n");
    // if (index >= m_sensorCount) return;
    //
    // SensorEntry* entry = &m_entries[index];
    // SensorDevice* sensor = entry->sensor;
    //
    // // if (!entry->lastOnlineState) {
    // // LOG_WARN("Sensor %s offline", sensor->getName());
    // // return;
    // // }
    // //
    // // if (entry->isCollecting) {
    // //     LOG_WARN("Sensor %s is already collecting", sensor->getName());
    // //     return;
    // // }
    // //
    // entry->isCollecting = true;
    //
    // LOG_DEBUG("Sensor %s: %s collecting", sensor->getName(), isManual ? "manual" : "auto");
    // // ★ 执行异步采集 ★
    // sensor->readAsync();
}

// ============================================================
// ★ ★ 采集完成回调 ★ ★
// ============================================================
void SensorManager::onSensorDataReady(void* arg, const DeviceRecord* data) {
    printf("回调了吗\r\n");
    auto* self = static_cast<SensorManager*>(arg);
    if (self == nullptr || data == nullptr) return;

    self->handleSensorData(data);
}

// ============================================================
// ★ ★ 处理采集完成的数据 ★ ★
// ============================================================
void SensorManager::handleSensorData(const DeviceRecord* data) {
    // ★ 清除采集中标志 ★
    for (uint8_t i = 0; i < m_sensorCount; i++) {
        if (strcmp(m_entries[i].sensor->getDeviceId(), data->getDeviceId()) == 0) {
            m_entries[i].isCollecting = false;
            m_entries[i].lastCollectTick = xTaskGetTickCount();
            break;
        }
    }

    // ★ ★ 转发给 DataGateway 发送 ★ ★
    // ★ 只有有效数据才转发
    if (data->isValid()) {
        LOG_DEBUG("数据有效，发送给上位机\r\n");
        DataContext* ctx = g_tx_data_context_pool.allocate();
        ctx->setData(*data);
        DataGateway::getInstance().transmitFromISR(ctx);
    } else {
        LOG_DEBUG("Sensor %s: invalid data, skip", data->getDeviceName());
    }
}

// ============================================================
// ★ ★ 上位机主动采集 ★ ★
// ============================================================
bool SensorManager::manualCollect(const char* name) {
    int index = findSensor(name);
    if (index < 0) {
        LOG_WARN("Sensor %s not found", name);
        return false;
    }

    LOG_INFO("Manual collect: %s", name);
    doCollect(index, true);
    return true;
}

// ============================================================
// ★ 上位机 API ★
// ============================================================
bool SensorManager::setInterval(const char* name, uint32_t interval_ms) {
    int index = findSensor(name);
    if (index < 0) return false;

    m_entries[index].sensor->setInterval(interval_ms);
    LOG_INFO("Sensor %s interval set to %lu ms", name, interval_ms);
    return true;
}

bool SensorManager::setEnabled(const char* name, bool enabled) {
    int index = findSensor(name);
    if (index < 0) return false;

    m_entries[index].sensor->setEnabled(enabled);
    LOG_INFO("Sensor %s %s", name, enabled ? "enabled" : "disabled");
    return true;
}

SensorDevice* SensorManager::getSensor(const char* name) const {
    int index = findSensor(name);
    return (index >= 0) ? m_entries[index].sensor : nullptr;
}

int SensorManager::findSensor(const char* name) const {
    for (int i = 0; i < m_sensorCount; i++) {
        if (strcmp(m_entries[i].sensor->getName(), name) == 0) {
            return i;
        }
    }
    return -1;
}

// ★ ★ 创建传感器（根据配置索引）★ ★
// ============================================================
void SensorManager::createSensor(uint8_t pinIndex) {
    if (pinIndex >= m_configCount) return;

    SensorPinConfig* cfg = &m_currentConfig[pinIndex];
    if (cfg->type == SensorType::NONE || !cfg->enabled) return;

    // 检查是否已存在
    for (uint8_t i = 0; i < m_sensorCount; i++) {
        if (m_entries[i].sensor != nullptr) {
            if (strcmp(m_entries[i].sensor->getName(), cfg->name) == 0) {
                LOG_WARN("Sensor %s already exists", cfg->name);
                return;
            }
        }
    }

    if (m_sensorCount >= MAX_SENSORS) {
        LOG_ERROR("Too many sensors! Max: %d", MAX_SENSORS);
        return;
    }

    // ★ 创建传感器 ★
    SensorDevice* sensor = createSensorByType(cfg->type, cfg->name, cfg->port, cfg->pin);
    if (sensor == nullptr) {
        LOG_ERROR("Failed to create sensor %s", cfg->name);
        return;
    }

    sensor->setInterval(cfg->interval_ms);
    sensor->setDataCallback(onSensorDataReady, this);

    // ★ 创建 GPIO 检测器 ★
    SensorEntry* entry = &m_entries[m_sensorCount];
    entry->sensor = sensor;
    entry->detector = new GpioDetector();
    entry->detector->init(cfg->name, cfg->port, cfg->pin, cfg->activeHigh);
    entry->lastOnlineState = isPinInserted(cfg->port, cfg->pin, cfg->activeHigh);
    entry->lastCollectTick = 0;
    entry->isCollecting = false;
    sensor->init();

    m_sensorCount++;

    LOG_INFO("Sensor created: %s (%s), total: %d", cfg->name, entry->sensor->getName(), m_sensorCount);
}

// ============================================================
// ★ ★ 根据类型创建传感器实例 ★ ★
// ============================================================
SensorDevice* SensorManager::createSensorByType(SensorType type, const char* name, GPIO_TypeDef* port, uint16_t pin) {
    LOG_INFO("Creating sensor %s", name);
    switch (type) {
        case SensorType::DHT11:
            return new DHT11Sensor(name, port, pin);
        // case SensorType::DS18B20:
        //     return new DS18B20Sensor(name, port, pin);
        // case SensorType::BH1750:
        //     return new BH1750Sensor(name, port, pin);
        default:
            LOG_WARN("Unknown sensor type: %d", (int)type);
            return nullptr;
    }
}