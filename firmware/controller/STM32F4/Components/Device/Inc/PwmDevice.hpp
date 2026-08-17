// PwmDevice.hpp
#ifndef PWM_DEVICE_HPP
#define PWM_DEVICE_HPP

#include "DeviceBase.hpp"
#include "stm32f1xx_hal.h"

// ★ PWM 设备 ★
class PwmDevice : public DeviceBase {
public:
    PwmDevice(const std::string& id, const std::string& name,
              TIM_HandleTypeDef* htim, uint32_t channel,
              uint32_t period = 1000)
        : DeviceBase(id, name, DeviceType::PWM)
        , m_htim(htim)
        , m_channel(channel)
        , m_period(period)
        , m_duty(0)
        , m_running(false) {}

    // -------- 实现 DeviceBase --------
    bool init() override {
        // 启动 PWM
        HAL_TIM_PWM_Start(m_htim, m_channel);
        m_running = true;
        m_isOnline = true;
        m_status = DeviceStatus::ONLINE;
        return true;
    }

    std::string getDeviceInfo() const override {
        return "PWM: " + m_name + " (" + m_deviceId + ") Duty: " + std::to_string(m_duty) + "%";
    }

    // -------- PWM 专用方法 --------
    // 设置占空比（0-100 百分比）
    void setDuty(uint32_t duty_percent) {
        if (duty_percent > 100) duty_percent = 100;
        m_duty = duty_percent;

        uint32_t compare = (duty_percent * m_period) / 100;
        __HAL_TIM_SET_COMPARE(m_htim, m_channel, compare);
    }

    uint32_t getDuty() const { return m_duty; }

    void start() {
        if (!m_running) {
            HAL_TIM_PWM_Start(m_htim, m_channel);
            m_running = true;
        }
    }

    void stop() {
        if (m_running) {
            HAL_TIM_PWM_Stop(m_htim, m_channel);
            m_running = false;
        }
    }

    bool isRunning() const { return m_running; }

private:
    TIM_HandleTypeDef* m_htim;
    uint32_t m_channel;
    uint32_t m_period;
    uint32_t m_duty;
    bool m_running;
};

#endif