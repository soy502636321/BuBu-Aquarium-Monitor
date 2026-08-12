// DHT11Sensor.hpp
#ifndef DHT11_SENSOR_HPP
#define DHT11_SENSOR_HPP

#include "SensorDevice.hpp"
#include "stm32f1xx_hal.h"

class DHT11Sensor : public SensorDevice {
public:
    DHT11Sensor(const char* name, GPIO_TypeDef* port, uint16_t pin)
        : SensorDevice(generateId(SensorType::DHT11, port, pin), name, SensorType::DHT11)
        , m_port(port)
        , m_pin(pin) {}

    // ============================================================
    // ★ 实现 SensorDevice ★
    // ============================================================

    bool init() override {
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin = m_pin;
        gpio.Mode = GPIO_MODE_OUTPUT_OD;
        gpio.Pull = GPIO_PULLUP;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(m_port, &gpio);

        m_initialized = true;
        m_status = DeviceStatus::ONLINE;
        return true;
    }

    bool read(DeviceRecord& record) override {
        LOG_INFO("DHT11 Sensor read start");
        if (!m_initialized) {
            LOG_WARN("DHT11 Sensor not initialized");
            return false;
        };
        LOG_INFO("读取传感器");
        uint8_t humi, temp;
        if (!readData(&humi, &temp)) {
            return false;
        }
        record.addDataPoint(DataPoint(DataPointType::ROOM_TEMP, 25.5f));
        record.addDataPoint(DataPoint(DataPointType::ROOM_HUMIDITY, 65.0f));

        updateLastData(record);
        return true;
    }

    uint32_t getDefaultInterval() const override { return 10 * 1000; }

private:
    bool readData(uint8_t* humidity, uint8_t* temperature) {
        // DHT11 时序代码
        *humidity = 50;
        *temperature = 25;
        return true;
    }

    GPIO_TypeDef* m_port;
    uint16_t m_pin;
};

#endif