/*
 * switch_device.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */
#include "device.hpp"

// ===== 开关设备 =====
class SwitchDevice : public DeviceBase {
public:
    SwitchDevice(const std::string& id, const std::string& name)
        : DeviceBase(id, name, DeviceType::SWITCH), state(false), gpio_pin(-1) {}
    
    // 执行命令
    bool executeCommand(const Command& cmd) override {
        if (cmd.type == CommandType::WRITE) {
            if (cmd.target == "state") {
                if (cmd.value == "ON" || cmd.value == "1" || cmd.value == "true") {
                    state = true;
                } else if (cmd.value == "OFF" || cmd.value == "0" || cmd.value == "false") {
                    state = false;
                } else {
                    return false;
                }
                // 实际控制GPIO的代码在这里
                // gpio_set_level(gpio_pin, state ? 1 : 0);
                return true;
            }
        } else if (cmd.type == CommandType::READ) {
            // 读取状态通过返回值传递
            return true;
        }
        return false;
    }
    
    DataPoint collectData() override {
        DataPoint dp;
        dp.device_id = device_id;
        dp.timestamp = getCurrentTimestamp();
        
        SensorValue state_val;
        state_val.type = "state";
        state_val.value = state ? 1.0f : 0.0f;
        state_val.unit = "";
        state_val.quality = 0;
        dp.values.push_back(state_val);
        
        return dp;
    }
    
    bool state = false;
    int gpio_pin;
    bool is_pull_up = false;
    uint32_t debounce_ms = 50;
};


