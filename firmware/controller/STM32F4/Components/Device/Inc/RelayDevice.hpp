// RelayDevice.hpp
#ifndef RELAY_DEVICE_HPP
#define RELAY_DEVICE_HPP

#include "DeviceBase.hpp"
#include "stm32f1xx_hal.h"

// ★ 继电器设备 ★
class RelayDevice : public DeviceBase {
public:
    RelayDevice(const std::string& id, const std::string& name,
                GPIO_TypeDef* port, uint16_t pin, bool activeHigh = true)
        : DeviceBase(id, name, DeviceType::RELAY)
        , m_port(port)
        , m_pin(pin)
        , m_activeHigh(activeHigh)
        , m_state(false)
        , m_initialized(false) {}

    // -------- 实现 DeviceBase --------
    bool init() override {
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin = m_pin;
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        gpio.Pull = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(m_port, &gpio);

        // 默认关闭
        turnOff();
        m_initialized = true;
        m_isOnline = true;
        m_status = DeviceStatus::ONLINE;
        return true;
    }

    std::string getDeviceInfo() const override {
        return "Relay: " + m_name + " (" + m_deviceId + ") State: " + (m_state ? "ON" : "OFF");
    }

    // -------- 继电器专用方法 --------
    void turnOn() {
        HAL_GPIO_WritePin(m_port, m_pin, m_activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET);
        m_state = true;
    }

    void turnOff() {
        HAL_GPIO_WritePin(m_port, m_pin, m_activeHigh ? GPIO_PIN_RESET : GPIO_PIN_SET);
        m_state = false;
    }

    void toggle() {
        if (m_state) turnOff();
        else turnOn();
    }

    bool getState() const { return m_state; }

private:
    GPIO_TypeDef* m_port;
    uint16_t m_pin;
    bool m_activeHigh;
    bool m_state;
    bool m_initialized;
};

#endif