/*
 * pwm_device.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */
 #include "device.hpp"


// ===== PWM设备 =====
class PWMDevice : public DeviceBase {
public:
    PWMDevice(const std::string& id, const std::string& name)
        : DeviceBase(id, name, DeviceType::PWM), 
          duty_cycle(0), frequency(1000), gpio_pin(-1) {}
    
    // 执行命令
    bool executeCommand(const Command& cmd) override {
        if (cmd.type == CommandType::WRITE) {
            if (cmd.target == "duty_cycle") {
                int val = std::stoi(cmd.value);
                if (val >= 0 && val <= 100) {
                    duty_cycle = val;
                    // 实际设置PWM的代码
                    // ledc_set_duty(...);
                    return true;
                }
            } else if (cmd.target == "frequency") {
                uint32_t freq = std::stoul(cmd.value);
                if (freq >= 1 && freq <= 100000) {
                    frequency = freq;
                    return true;
                }
            }
        }
        return false;
    }
    
    DataPoint collectData() override {
        DataPoint dp;
        dp.device_id = device_id;
        dp.timestamp = getCurrentTimestamp();
        
        SensorValue duty_val;
        duty_val.type = "duty_cycle";
        duty_val.value = duty_cycle;
        duty_val.unit = "%";
        duty_val.quality = 0;
        dp.values.push_back(duty_val);
        
        SensorValue freq_val;
        freq_val.type = "frequency";
        freq_val.value = frequency;
        freq_val.unit = "Hz";
        freq_val.quality = 0;
        dp.values.push_back(freq_val);
        
        return dp;
    }
    
    uint8_t duty_cycle;      // 0-100%
    uint32_t frequency;      // Hz
    int gpio_pin;
    uint8_t resolution_bits = 10;  // PWM分辨率
};

