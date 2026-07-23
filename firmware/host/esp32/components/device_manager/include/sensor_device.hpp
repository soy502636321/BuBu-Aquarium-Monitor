/*
 * sensor_device.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */
 
 #include "device.hpp"

class SensorDevice : public DeviceBase {
public:
    SensorDevice(const std::string& id, const std::string& name)
        : DeviceBase(id, name, DeviceType::SENSOR) {}
    
    // 添加/更新传感器数据点
    void addSensorValue(const SensorValue& value) {
        // 如果已存在同类型，则更新
        for (auto& v : data_points) {
            if (v.type == value.type) {
                v = value;
                return;
            }
        }
        data_points.push_back(value);
    }
    
    // 获取指定类型的传感器值
    float getValue(const std::string& type) const {
        for (const auto& v : data_points) {
            if (v.type == type) return v.value;
        }
        return 0.0f;
    }
    
    // 实现纯虚函数
    DataPoint collectData() override {
        DataPoint dp;
        dp.device_id = device_id;
        dp.timestamp = getCurrentTimestamp();
        dp.values = data_points;
        return dp;
    }
    
    bool executeCommand(const Command& cmd) override {
        if (cmd.type == CommandType::READ) {
            // 读传感器数据：通过命令返回值传递
            return true;
        } else if (cmd.type == CommandType::CALIBRATE) {
            // 校准传感器（具体实现由硬件驱动完成）
            return true;
        }
        return false;
    }
    
    std::vector<SensorValue> data_points;
    float sampling_interval_s = 60.0f;  // 采样间隔（秒）
};



