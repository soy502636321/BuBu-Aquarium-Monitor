// SensorConfig.hpp
#ifndef SENSOR_CONFIG_HPP
#define SENSOR_CONFIG_HPP

#include "stm32f1xx_hal.h"
#include <cstdint>
#include <cstring>
#include "Device.hpp"
#include "main.h"

#define SENSOR_NAME_MAX_LEN  24
#define MAX_SENSOR_PINS      8

// ============================================================
// ★ ★ 传感器引脚配置结构 ★ ★
// ============================================================
struct SensorPinConfig {
    uint8_t pinIndex;
    GPIO_TypeDef* port;
    uint16_t pin;
    SensorType type;
    char name[SENSOR_NAME_MAX_LEN];
    uint32_t interval_ms;
    bool enabled;
    bool activeHigh;

    SensorPinConfig()
        : pinIndex(0)
        , port(nullptr)
        , pin(0)
        , type(SensorType::NONE)
        , interval_ms(1000)
        , enabled(false)
        , activeHigh(true) {
        memset(name, 0, sizeof(name));
    }

    SensorPinConfig(uint8_t idx, GPIO_TypeDef* p, uint16_t pinNum,
                    SensorType t, const char* n, uint32_t interval, bool en = true, bool ah = true)
        : pinIndex(idx)
        , port(p)
        , pin(pinNum)
        , type(t)
        , interval_ms(interval)
        , enabled(en)
        , activeHigh(ah) {
        strncpy(name, n, SENSOR_NAME_MAX_LEN - 1);
        name[SENSOR_NAME_MAX_LEN - 1] = '\0';
    }

    bool isValid() const {
        return port != nullptr && pin != 0 && type != SensorType::NONE;
    }

    bool isInserted() const {
        if (port == nullptr) return false;
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin = pin;
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = activeHigh ? GPIO_PULLDOWN : GPIO_PULLUP;
        HAL_GPIO_Init(port, &gpio);
        uint8_t level = HAL_GPIO_ReadPin(port, pin);
        return activeHigh ? (level == GPIO_PIN_SET) : (level == GPIO_PIN_RESET);
    }

    void copyFrom(const SensorPinConfig& other) {
        pinIndex = other.pinIndex;
        port = other.port;
        pin = other.pin;
        type = other.type;
        strncpy(name, other.name, SENSOR_NAME_MAX_LEN - 1);
        name[SENSOR_NAME_MAX_LEN - 1] = '\0';
        interval_ms = other.interval_ms;
        enabled = other.enabled;
        activeHigh = other.activeHigh;
    }
};

// ============================================================
// ★ ★ 默认配置表（使用 inline constexpr 避免多重定义） ★ ★
// ============================================================
inline const SensorPinConfig DEFAULT_SENSOR_CONFIG[] = {
    {0, DHT11_GPIO_Port, DHT11_Pin, SensorType::DHT11,    "DHT11",   10 * 1000, true, true},
    {1, GPIOA, GPIO_PIN_3, SensorType::NONE,  "",         1000, false, true},
    {2, GPIOA, GPIO_PIN_3, SensorType::NONE,  "",         1000, false, true},
    {3, GPIOA, GPIO_PIN_3, SensorType::NONE,  "",         1000, false, true},
    {4, GPIOA, GPIO_PIN_4, SensorType::NONE,  "",         1000, false, true},
    {5, GPIOA, GPIO_PIN_5, SensorType::NONE,  "",         1000, false, true},
    {6, GPIOA, GPIO_PIN_6, SensorType::NONE,  "",         1000, false, true},
    {7, GPIOA, GPIO_PIN_7, SensorType::NONE,  "",         1000, false, true},
};

inline constexpr uint8_t SENSOR_CONFIG_COUNT = sizeof(DEFAULT_SENSOR_CONFIG) / sizeof(DEFAULT_SENSOR_CONFIG[0]);

#endif