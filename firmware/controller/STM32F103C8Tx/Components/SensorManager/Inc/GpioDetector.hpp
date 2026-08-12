//
// Created by Hu on 2026-08-10.
//

#ifndef STM32F103C8TX_SENSORGPIODETECTOR_HPP
#define STM32F103C8TX_SENSORGPIODETECTOR_HPP

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <cstring>
#include "Logger.hpp"

// ★ 回调函数类型 ★
typedef void (*GpioDetectCallback)(void* arg, const char* name, bool inserted);

// ★ GPIO 检测器（不继承 ISensor）★
class GpioDetector {
public:
    GpioDetector()
        : m_port(nullptr)
        , m_pin(0)
        , m_activeHigh(true)
        , m_callback(nullptr)
        , m_arg(nullptr)
        , m_debounceTimer(nullptr)
        , m_pendingInserted(false)
        , m_pending(false) {
        memset(m_name, 0, sizeof(m_name));
    }

    // ★ 初始化 ★
    bool init(const char* name,
              GPIO_TypeDef* port,
              uint16_t pin,
              bool activeHigh = true,
              uint32_t debounce_ms = 50) {
        if (port == nullptr) return false;

        strncpy(m_name, name, sizeof(m_name) - 1);
        m_name[sizeof(m_name) - 1] = '\0';
        m_port = port;
        m_pin = pin;
        m_activeHigh = activeHigh;

        // 初始化 GPIO 为中断模式
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin = pin;
        gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
        gpio.Pull = activeHigh ? GPIO_PULLDOWN : GPIO_PULLUP;
        HAL_GPIO_Init(port, &gpio);

        // ★ 创建防抖定时器 ★
        m_debounceTimer = xTimerCreate(
            name,
            pdMS_TO_TICKS(debounce_ms),
            pdFALSE,
            this,
            debounceTimerCallback
        );

        if (m_debounceTimer == nullptr) {
            LOG_ERROR("Debounce timer creation failed for %s", name);
            return false;
        }

        LOG_INFO("GpioDetector %s initialized", name);
        return true;
    }

    // ★ 设置回调 ★
    void setCallback(GpioDetectCallback callback, void* arg) {
        m_callback = callback;
        m_arg = arg;
    }

    // ★ 使能中断 ★
    void enableInterrupt(uint32_t irq, uint32_t priority = 3) {
        HAL_NVIC_SetPriority((IRQn_Type)irq, priority, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);
    }

    // ★ 获取当前插入状态 ★
    bool isInserted() const {
        if (m_port == nullptr) return false;
        uint8_t expected = m_activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET;
        return HAL_GPIO_ReadPin(m_port, m_pin) == expected;
    }

    // ★ 获取名称 ★
    const char* getName() const { return m_name; }

    // ★ 中断中调用 ★
    void onInterrupt() {
        if (m_debounceTimer == nullptr) return;

        // 保存状态，启动防抖
        m_pendingInserted = isInserted();
        m_pending = true;

        xTimerStop(m_debounceTimer, 0);
        xTimerStart(m_debounceTimer, 0);
    }

    // ★ 重置（拔出时清理）★
    void reset() {
        m_pending = false;
        m_pendingInserted = false;
        xTimerStop(m_debounceTimer, 0);
    }

private:
    // ★ 防抖定时器回调 ★
    static void debounceTimerCallback(TimerHandle_t xTimer) {
        auto* self = static_cast<GpioDetector*>(pvTimerGetTimerID(xTimer));
        if (self != nullptr) {
            self->processDebounce();
        }
    }

    void processDebounce() {
        if (!m_pending) return;
        m_pending = false;

        // ★ 确认状态 ★
        bool confirmed = isInserted();

        if (confirmed == m_pendingInserted && m_callback != nullptr) {
            LOG_INFO("GPIO detect confirmed: %s -> %s",
                     m_name, confirmed ? "INSERT" : "REMOVE");
            m_callback(m_arg, m_name, confirmed);
        } else {
            LOG_WARN("GPIO detect mismatch: %s (pending=%d, actual=%d)",
                     m_name, m_pendingInserted, confirmed);
        }
    }

private:
    char m_name[32];
    GPIO_TypeDef* m_port;
    uint16_t m_pin;
    bool m_activeHigh;

    GpioDetectCallback m_callback;
    void* m_arg;

    TimerHandle_t m_debounceTimer;

    bool m_pendingInserted;
    bool m_pending;
};
#endif //STM32F103C8TX_SENSORGPIODETECTOR_HPP