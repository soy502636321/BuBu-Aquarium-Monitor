// SensorDevice.hpp
#ifndef SENSOR_DEVICE_HPP
#define SENSOR_DEVICE_HPP

#include "Device.hpp"

typedef void (*SensorDataCallback)(void* arg, const DeviceRecord* data);

// ★ 传感器设备基类（本身定义了完整的传感器接口）★
class SensorDevice : public DeviceBase {
public:
    SensorDevice(const char* id, const char* name, SensorType sensorType)
        : DeviceBase(id, name, DeviceType::Sensor)
        , m_sensorType(sensorType)
        , m_interval_ms(1000) {}

    ~SensorDevice() override = default;

    // ============================================================
    // ★ 实现 DeviceBase ★
    // ============================================================

    bool init() override {
        // 子类可以重写
        // m_isOnline = true;
        // m_status = DeviceStatus::ONLINE;
        return true;
    }

    // ============================================================
    // ★ 传感器专用接口（纯虚函数，子类必须实现）★
    // ============================================================

    // 读取传感器数据
    virtual bool read(DeviceRecord& outData) = 0;

    void readAsync() {
        printf("传感器：readAsync - 1\r\n");
        DeviceRecord data;
        memset(&data, 0, sizeof(data));
        data.setDeviceId(m_deviceId);
        // data.deviceName[sizeof(data.deviceName) - 1] = '\0';

        if (read(data)) {
            data.setValid(true);
            printf("Sensor %s: read success\n", m_DeviceName);
        } else {
            data.setValid(false);
            LOG_WARN("Sensor %s read failed", m_DeviceName);
        }

        // ★ 无论成功失败都回调
        if (m_callback != nullptr) {
            m_callback(m_callbackArg, &data);
        }
    }
    // 获取传感器类型
    SensorType getSensorType() const { return m_sensorType; }

    // ============================================================
    // ★ 采集周期配置 ★
    // ============================================================

    // -------- ★ 设置回调 ★ --------
    void setDataCallback(SensorDataCallback callback, void* arg) {
        m_callback = callback;
        m_callbackArg = arg;
    }

    virtual uint32_t getDefaultInterval() const = 0;

    void setInterval(uint32_t interval_ms) { m_interval_ms = interval_ms; }
    uint32_t getInterval() const { return m_interval_ms; }

    // ============================================================
    // ★ 缓存最后数据 ★
    // ============================================================
    const DeviceRecord& getLastData() const { return m_lastData; }

protected:
    // 更新最后一次采集到的合理数据
    void updateLastData(const DeviceRecord& data) {
        m_lastData = data;
    }

    // ★ ★ 通用 ID 生成函数 ★ ★
    static const char* generateId(SensorType type, GPIO_TypeDef* port, uint16_t pin) {
        static char id[32];
        const char* typeName = "Unknown";
        const char* portName = "Unknown";
        // 传感器类型名称
        switch (type) {
            case SensorType::DHT11: typeName = "DHT11"; break;
            default: typeName = "Sensor"; break;
        }
        // 端口名称
        if (port == GPIOA) portName = "A";
        else if (port == GPIOB) portName = "B";
        else if (port == GPIOC) portName = "C";
        else if (port == GPIOD) portName = "D";
        else if (port == GPIOE) portName = "E";

        snprintf(id, sizeof(id), "%s_%s%u", typeName, portName, pin);
        return id;
    }

protected:
    SensorType m_sensorType;
    DeviceRecord m_lastData;
    uint32_t m_interval_ms = 10 * 1000;

    bool m_enabled;

    SensorDataCallback m_callback;
    void* m_callbackArg;
};

#endif