/*
 * data_types.h
 *
 *  Created on: 2026年7月18日
 *      Author: Hu
 */

#ifndef COMPONENTS_COMMON_INCLUDE_DATA_TYPES_H_
#define COMPONENTS_COMMON_INCLUDE_DATA_TYPES_H_

#include <string>
#include <variant>
#include <vector>
#include <cstdint>
#include "cJSON.h"

/**
 * @brief 传感器监测数据（完整版，包含上报所需所有字段）
 */
 // 定义支持的动态类型
using DynamicValue = std::variant<
    int,                // 整数
    float,              // 单精度浮点
    double,             // 双精度浮点
    std::string,        // 字符串
    bool,               // 布尔值
    uint64_t            // 无符号64位（时间戳）
>;
 
struct DeviceDataProperty {
	std::string name;           // 属性名称
    DynamicValue value;         // 动态类型值
    
    // -------- 构造函数（自动推导类型） --------
    DeviceDataProperty() = default;
    
    DeviceDataProperty(const std::string& n, int v) : name(n), value(v) {}
    DeviceDataProperty(const std::string& n, float v) : name(n), value(v) {}
    DeviceDataProperty(const std::string& n, double v) : name(n), value(v) {}
    DeviceDataProperty(const std::string& n, const std::string& v) : name(n), value(v) {}
    DeviceDataProperty(const std::string& n, bool v) : name(n), value(v) {}
    DeviceDataProperty(const std::string& n, uint64_t v) : name(n), value(v) {}
    
    bool isInt() const { return std::holds_alternative<int>(value); }
    bool isFloat() const { return std::holds_alternative<float>(value); }
    bool isDouble() const { return std::holds_alternative<double>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isBool() const { return std::holds_alternative<bool>(value); }
    bool isUint64() const { return std::holds_alternative<uint64_t>(value); }
    
    int asInt(int def = 0) const {
        if (isInt()) return std::get<int>(value);
        if (isFloat()) return static_cast<int>(std::get<float>(value));
        if (isDouble()) return static_cast<int>(std::get<double>(value));
        return def;
    }
    
    float asFloat(float def = 0.0f) const {
        if (isFloat()) return std::get<float>(value);
        if (isInt()) return static_cast<float>(std::get<int>(value));
        if (isDouble()) return static_cast<float>(std::get<double>(value));
        return def;
    }
    
    double asDouble(double def = 0.0) const {
        if (isDouble()) return std::get<double>(value);
        if (isFloat()) return static_cast<double>(std::get<float>(value));
        if (isInt()) return static_cast<double>(std::get<int>(value));
        return def;
    }
    
    std::string asString(const std::string& def = "") const {
        if (isString()) return std::get<std::string>(value);
        if (isInt()) return std::to_string(std::get<int>(value));
        if (isFloat()) return std::to_string(std::get<float>(value));
        if (isDouble()) return std::to_string(std::get<double>(value));
        if (isBool()) return std::get<bool>(value) ? "true" : "false";
        if (isUint64()) return std::to_string(std::get<uint64_t>(value));
        return def;
    }
    
    bool asBool(bool def = false) const {
        if (isBool()) return std::get<bool>(value);
        if (isInt()) return std::get<int>(value) != 0;
        if (isFloat()) return std::get<float>(value) != 0.0f;
        return def;
    }
    
    uint64_t asUint64(uint64_t def = 0) const {
        if (isUint64()) return std::get<uint64_t>(value);
        if (isInt()) return static_cast<uint64_t>(std::get<int>(value));
        return def;
    }
    
    const char* getTypeName() const {
        if (isInt()) return "int";
        if (isFloat()) return "float";
        if (isDouble()) return "double";
        if (isString()) return "string";
        if (isBool()) return "bool";
        if (isUint64()) return "uint64";
        return "unknown";
    }
    
    // -------- 单个属性转 JSON --------
    cJSON* toJson() const {
        cJSON* item = cJSON_CreateObject();
        if (!item) return nullptr;
        
        cJSON_AddStringToObject(item, "name", name.c_str());
        cJSON_AddStringToObject(item, "type", getTypeName());
        
        if (isInt()) {
            cJSON_AddNumberToObject(item, "value", asInt());
        } else if (isFloat()) {
            cJSON_AddNumberToObject(item, "value", asFloat());
        } else if (isDouble()) {
            cJSON_AddNumberToObject(item, "value", asDouble());
        } else if (isString()) {
            cJSON_AddStringToObject(item, "value", asString().c_str());
        } else if (isBool()) {
            cJSON_AddBoolToObject(item, "value", asBool());
        } else if (isUint64()) {
            cJSON_AddNumberToObject(item, "value", static_cast<double>(asUint64()));
        } else {
            cJSON_AddStringToObject(item, "value", "");
        }
        
        return item;
    }
};
 
struct DeviceData {
    // -------- 必填：设备标识 --------
    std::string device_id;
    uint64_t timestamp;
    std::string data_type;
    std::vector<DeviceDataProperty> properties;
    
DeviceData() : timestamp(0) {}
    
    // -------- 添加属性 --------
    DeviceData& add(const std::string& name, int value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    DeviceData& add(const std::string& name, float value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    DeviceData& add(const std::string& name, double value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    DeviceData& add(const std::string& name, const std::string& value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    DeviceData& add(const std::string& name, bool value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    DeviceData& add(const std::string& name, uint64_t value) {
        properties.emplace_back(name, value);
        return *this;
    }
    
    // -------- 查找属性 --------
    const DeviceDataProperty* find(const std::string& name) const {
        for (const auto& prop : properties) {
            if (prop.name == name) {
                return &prop;
            }
        }
        return nullptr;
    }
    
    // -------- 便捷获取 --------
    int getInt(const std::string& name, int def = 0) const {
        const auto* prop = find(name);
        return prop ? prop->asInt(def) : def;
    }
    
    float getFloat(const std::string& name, float def = 0.0f) const {
        const auto* prop = find(name);
        return prop ? prop->asFloat(def) : def;
    }
    
    double getDouble(const std::string& name, double def = 0.0) const {
        const auto* prop = find(name);
        return prop ? prop->asDouble(def) : def;
    }
    
    std::string getString(const std::string& name, const std::string& def = "") const {
        const auto* prop = find(name);
        return prop ? prop->asString(def) : def;
    }
    
    bool getBool(const std::string& name, bool def = false) const {
        const auto* prop = find(name);
        return prop ? prop->asBool(def) : def;
    }
    
    uint64_t getUint64(const std::string& name, uint64_t def = 0) const {
        const auto* prop = find(name);
        return prop ? prop->asUint64(def) : def;
    }
    
    // ================================================================
    // ★★★ toJson：序列化为 JSON 字符串 ★★★
    // ================================================================
    
    std::string toJson() const {
        cJSON* root = cJSON_CreateObject();
        if (!root) return "";
        
        // 元数据
        cJSON_AddStringToObject(root, "device_id", device_id.c_str());
        cJSON_AddNumberToObject(root, "timestamp", static_cast<double>(timestamp));
        cJSON_AddStringToObject(root, "data_type", data_type.c_str());
        
        // 属性数组
        cJSON* arr = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "properties", arr);
        
        for (const auto& prop : properties) {
            cJSON* item = prop.toJson();
            if (item) {
                cJSON_AddItemToArray(arr, item);
            }
        }
        
        char* json_str = cJSON_PrintUnformatted(root);
        if (!json_str) {
            cJSON_Delete(root);
            return "";
        }
        
        std::string result(json_str);
        cJSON_free(json_str);
        cJSON_Delete(root);
        
        return result;
    }
};

/**
 * @brief 控制命令结构体（从云端接收）
 */
struct DeviceControlCommand {
    // -------- 必填 --------
    std::string command;         // 命令名称：RELAY_ON, RELAY_OFF, SET_THRESHOLD, REBOOT, OTA
    
    // -------- 可选 --------
    std::string target;          // 目标设备（如果是广播命令）
    uint64_t timestamp;          // 命令时间戳（由云端生成）
    
    // -------- 参数（根据命令类型使用不同字段） --------
    float value;                 // 数值参数（如阈值：0.5）
    std::string params;          // 复杂参数（JSON字符串，如 {"key":"value"}）
    
    // -------- 构造函数 --------
    DeviceControlCommand()
        : timestamp(0), value(0) {}
};

#endif /* COMPONENTS_COMMON_INCLUDE_DATA_TYPES_H_ */
