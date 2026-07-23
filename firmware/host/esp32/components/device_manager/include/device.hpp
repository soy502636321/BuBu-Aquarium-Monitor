/*
 * device.h
 *
 *  Created on: 2026年7月14日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_HPP_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_HPP_

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <ctime>
#include <sys/time.h>
#include "esp_random.h"

enum class DeviceType
{
    Unknown = 0,

    Sensor, 	// 传感器
    Switch, 	// 开关
    PWM   		// PWM调光/调速
};

// 设备状态枚举
enum class DeviceStatus : uint8_t {
    OFFLINE = 0,
    ONLINE = 1,
    ERROR = 2,
    CALIBRATING = 3
};

// 命令类型枚举
enum class CommandType : uint8_t {
    READ = 0,           // 读取数据
    WRITE = 1,          // 写入控制值
    CALIBRATE = 2,      // 校准
    RESET = 3,          // 复位/重启
    ENABLE = 4,         // 启用设备
    DISABLE = 5,        // 禁用设备
    CONFIG = 6,         // 修改配置参数
    QUERY = 7           // 查询状态
};

// 命令状态枚举
enum class CommandStatus : uint8_t {
    PENDING = 0,        // 等待执行
    EXECUTING = 1,      // 执行中
    SUCCESS = 2,        // 成功
    FAILED = 3,         // 失败
    TIMEOUT = 4,        // 超时
    CANCELLED = 5,      // 已取消
    UNKNOWN = 6         // 未知状态
};

// ===== 命令目标类型 =====
enum class CommandTarget : uint8_t {
    STATE = 0,          // 开关状态 (ON/OFF)
    DUTY_CYCLE = 1,     // PWM占空比 (0-100)
    FREQUENCY = 2,      // PWM频率 (Hz)
    THRESHOLD = 3,      // 阈值设置
    INTERVAL = 4,       // 采样间隔
    PARAM = 5,          // 通用参数
    FIRMWARE = 6        // 固件升级
};

struct DataPoint {
    std::string type;        // 数据类型："temperature", "humidity", "state", "duty_cycle"
    float value;             // 数值
    std::string unit;        // 单位："°C", "%", "pH", "mg/L", "" (开关无单位)
    uint8_t quality = 0;     // 0=有效, 1=可疑, 2=无效
    
    // 辅助方法
    bool isValid() const { return quality == 0; }
    bool isSuspect() const { return quality == 1; }
    bool isInvalid() const { return quality >= 2; }
    
    // 转换为字符串（便于调试）
    std::string toString() const {
        return type + "=" + std::to_string(value) + (unit.empty() ? "" : unit);
    }
};

struct DeviceRecord {
    // ---------- 标识字段 ----------
    std::string device_id;          // 设备唯一标识，如 "sensor_01"
    std::string device_name;        // 设备名称，如 "水温传感器"（冗余存储，便于离线分析）
    DeviceType device_type;         // 设备类型：SENSOR/SWITCH/PWM
    
    // ---------- 时间字段 ----------
    std::string timestamp;          // 采集时间 "2026-07-23 14:30:25"
    
	std::vector<DataPoint> points;       // ✅ 采集的数据点列表
    
    // ---------- 扩展字段 ----------
    uint8_t data_quality = 0;         // 整条记录的数据质量：0=正常, 1=部分可疑, 2=全部无效
    std::map<std::string, std::string> metadata;  // 元数据: {"calibration_date":"2026-07-01"}
    std::string checksum;                         // 数据校验和 (MD5/SHA256)
};

struct DeviceCommand {
    // ---------- 1. 标识字段 ----------
    std::string command_id;          // 命令唯一ID (UUID或自增序列号)
    std::string device_id;           // 目标设备ID
    
    // ---------- 2. 命令内容 ----------
    CommandType type;                // 命令类型：READ/WRITE/CALIBRATE...
    CommandTarget target;            // 目标属性：STATE/DUTY_CYCLE...
    std::string value;               // 命令值 (字符串形式，灵活通用)
    std::map<std::string, std::string> params;  // 扩展参数
    
    // ---------- 3. 执行控制 ----------
    uint32_t timeout_ms = 5000;      // 超时时间 (毫秒)
    uint8_t max_retry = 3;           // 最大重试次数
    uint8_t retry_count = 0;         // 当前已重试次数
    bool require_ack = true;         // 是否需要设备确认
    
    // ---------- 4. 时间字段 ----------
    uint64_t created_at_ms;          // 命令创建时间 (毫秒时间戳)
    uint64_t executed_at_ms = 0;     // 命令执行时间
    uint64_t completed_at_ms = 0;    // 命令完成时间
    
    // ---------- 5. 状态与结果 ----------
    CommandStatus status;            // 命令状态
    std::string result;              // 执行结果描述
    std::string error_code;          // 错误码 (便于分类处理)
    
    // ---------- 6. 来源与优先级 ----------
    std::string source;              // 命令来源: "app", "mqtt", "schedule", "auto"
    uint8_t priority = 5;            // 优先级 (0-10, 0最高)
    uint32_t sequence = 0;           // 序列号 (用于排序)
    
    // ===== 构造函数 =====
    DeviceCommand() 
        : type(CommandType::READ), 
          target(CommandTarget::STATE),
          timeout_ms(5000), 
          max_retry(3), 
          retry_count(0),
          require_ack(true),
          created_at_ms(getCurrentTimestampMs()),
          status(CommandStatus::PENDING),
          priority(5),
          sequence(0) {}
    
    // 便捷构造：开关控制
    static DeviceCommand makeSwitch(const std::string& device_id, bool on) {
        DeviceCommand cmd;
        cmd.command_id = generateUUID();
        cmd.device_id = device_id;
        cmd.type = CommandType::WRITE;
        cmd.target = CommandTarget::STATE;
        cmd.value = on ? "ON" : "OFF";
        return cmd;
    }
    
    // 便捷构造：PWM占空比控制
    static DeviceCommand makePWM(const std::string& device_id, uint8_t duty) {
        DeviceCommand cmd;
        cmd.command_id = generateUUID();
        cmd.device_id = device_id;
        cmd.type = CommandType::WRITE;
        cmd.target = CommandTarget::DUTY_CYCLE;
        cmd.value = std::to_string(duty);
        return cmd;
    }
    
    // 便捷构造：读取数据
    static DeviceCommand makeRead(const std::string& device_id) {
        DeviceCommand cmd;
        cmd.command_id = generateUUID();
        cmd.device_id = device_id;
        cmd.type = CommandType::READ;
        return cmd;
    }
    
    // 便捷构造：校准
    static DeviceCommand makeCalibrate(const std::string& device_id, 
                                        const std::string& param = "") {
        DeviceCommand cmd;
        cmd.command_id = generateUUID();
        cmd.device_id = device_id;
        cmd.type = CommandType::CALIBRATE;
        if (!param.empty()) {
            cmd.params["param"] = param;
        }
        return cmd;
    }
    
    // ===== 辅助方法 =====
    
    // 检查命令是否已完成
    bool isCompleted() const {
        return status == CommandStatus::SUCCESS || 
               status == CommandStatus::FAILED || 
               status == CommandStatus::TIMEOUT ||
               status == CommandStatus::CANCELLED;
    }
    
    // 检查命令是否成功
    bool isSuccess() const {
        return status == CommandStatus::SUCCESS;
    }
    
    // 检查命令是否可重试
    bool canRetry() const {
        return (status == CommandStatus::FAILED || status == CommandStatus::TIMEOUT) &&
               retry_count < max_retry;
    }
    
    // 增加重试计数
    void incrementRetry() {
        retry_count++;
    }
    
    // 获取数值 (将字符串转为整数)
    int getIntValue() const {
        return std::stoi(value);
    }
    
    // 获取数值 (将字符串转为浮点数)
    float getFloatValue() const {
        return std::stof(value);
    }
    
    // 获取布尔值 (ON/OFF, 1/0, true/false)
    bool getBoolValue() const {
        return value == "ON" || value == "1" || value == "true" || value == "TRUE";
    }
    
    // 转JSON (用于网络传输)
    std::string toJSON() const {
        std::string json = "{";
        json += "\"command_id\":\"" + command_id + "\",";
        json += "\"device_id\":\"" + device_id + "\",";
        json += "\"type\":" + std::to_string((int)type) + ",";
        json += "\"target\":" + std::to_string((int)target) + ",";
        json += "\"value\":\"" + value + "\",";
        json += "\"timeout\":" + std::to_string(timeout_ms) + ",";
        json += "\"priority\":" + std::to_string(priority);
        json += "}";
        return json;
    }
    
    // ===== 辅助函数 =====
private:
    static std::string generateUUID() {
        // 简化实现：使用时间戳+随机数
        uint64_t ts = getCurrentTimestampMs();
        uint32_t rand = esp_random();
        char buf[32];
        snprintf(buf, sizeof(buf), "cmd_%llx_%lx", (unsigned long long)ts, rand);
        return std::string(buf);
    }
    
    static uint64_t getCurrentTimestampMs() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
};

// 设备配置结构
struct DeviceConfig {
    std::string firmware_version = "1.0.0";
    uint32_t report_interval_ms = 60000;  // 上报间隔（毫秒）
    uint8_t retry_count = 3;              // 重试次数
    bool auto_report = true;              // 是否自动上报
    std::map<std::string, std::string> extra_params;  // 扩展参数
    
    // 转JSON（用于存储和传输）
    std::string toJson() const;
    static DeviceConfig fromJson(const std::string& json);
};

class DeviceBase {
public:
    DeviceBase(const std::string& id, const std::string& name, DeviceType type)
        : device_id(id), name(name), type(type), is_online(false) {}
    
    virtual ~DeviceBase() = default;
    
    // 纯虚函数：子类必须实现
    virtual bool executeCommand(const DeviceCommand& cmd) = 0;
    virtual DataPoint collectData() = 0;
    
    // 公共方法
    std::string getDeviceId() const { return device_id; }
    std::string getName() const { return name; }
    DeviceType getType() const { return type; }
    bool isOnline() const { return is_online; }
    
    // 设置/获取配置
    void setConfig(const DeviceConfig& config) { this->config = config; }
    const DeviceConfig& getConfig() const { return config; }
    
    // 获取设备信息（用于注册和发现）
    virtual std::string getDeviceInfo() const {
        return "Device: " + name + " (" + device_id + ") Type: " + std::to_string((int)type);
    }
    
protected:
    std::string device_id;
    std::string name;
    DeviceType type;
    DeviceStatus status = DeviceStatus::OFFLINE;
    bool is_online = false;
    DeviceConfig config;
    uint64_t last_heartbeat = 0;
    std::string location;  // 安装位置
};



#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_HPP_ */
