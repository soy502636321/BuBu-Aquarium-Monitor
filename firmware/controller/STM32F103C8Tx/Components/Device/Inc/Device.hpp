/*
 * device.h
 *
 *  Created on: 2026年7月14日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_HPP_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_HPP_

#pragma once

#include "value_type.hpp"
#include <cstdint>
#include <ctime>
#include <map>
#include <string>
#include <sys/time.h>
#include <variant>
#include <vector>

#define MAX_SWITCH_SIZE 12

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
enum class DeviceDataType : uint8_t {
	DATA = 0,		// 数据上报
	SWITCH,			// 开关
	PWM,			// PWM控制
	SETUP,			// 配置、设置
	COLLECTION,		// 立即采集
    READ,           // 读取数据
    WRITE,          // 写入控制值
    CALIBRATE,      // 校准
    RESET,          // 复位/重启
    ENABLE,         // 启用设备
    DISABLE,        // 禁用设备
    CONFIG,         // 修改配置参数
    QUERY           // 查询状态
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

enum class DataPointType : uint8_t {
    // ===== 水质参数 (0-19) =====
    TEMPERATURE = 0,      // 温度
    PH = 1,               // pH值
    TDS = 2,              // 总溶解固体
    TURBIDITY = 3,        // 浊度
    OXYGEN = 4,           // 溶解氧
    AMMONIA = 5,          // 氨氮
    NITRITE = 6,          // 亚硝酸盐
    NITRATE = 7,          // 硝酸盐
    ORP = 8,              // 氧化还原电位
    SALINITY = 9,         // 盐度
    CO2 = 10,             // 二氧化碳
    GH = 11,              // 总硬度
    KH = 12,              // 碳酸盐硬度
    CALCIUM = 13,         // 钙
    MAGNESIUM = 14,       // 镁
    CHLORINE = 15,        // 余氯
    OZONE = 16,           // 臭氧
    REDOX = 17,           // 氧化还原

    // ===== 液位与水量 (20-29) =====
    WATER_LEVEL = 20,     // 水位
    WATER_VOLUME = 21,    // 水量
    EVAPORATION = 22,     // 蒸发量

    // ===== 设备状态 (30-49) =====
    HEATER_STATE = 30,    // 加热器状态
    HEATER_POWER = 31,    // 加热器功率
    PUMP_STATE = 32,      // 水泵状态
    PUMP_SPEED = 33,      // 水泵转速
    LIGHT_STATE = 34,     // 灯光状态
    LIGHT_INTENSITY = 35, // 灯光强度
    LIGHT_COLOR_TEMP = 36,// 灯光色温
    AIR_PUMP_STATE = 37,  // 气泵状态
    FEEDER_STATE = 38,    // 喂食器状态
    UV_STATE = 39,        // UV杀菌灯状态
    CHILLER_STATE = 40,   // 冷水机状态
    SOLENOID_STATE = 41,  // 电磁阀状态
    FAN_STATE = 42,       // 风扇状态
    POWER = 43,           // 总功率
    CURRENT = 44,         // 电流
    VOLTAGE = 45,         // 电压

    // ===== 环境参数 (50-59) =====
    ROOM_TEMP = 50,       // 室温
    ROOM_HUMIDITY = 51,   // 环境湿度
    ATM_PRESSURE = 52,    // 大气压
    AMBIENT_LIGHT = 53,   // 环境光照

    // ===== 倾角传感器 (60-69) =====
    AXIS_X = 60,          // X轴倾角
    AXIS_Y = 61,          // Y轴倾角
    AXIS_Z = 62,          // Z轴倾角

    // ===== 时间与统计 (70-79) =====
    LAST_FEED_TIME = 70,  // 上次喂食时间
    LAST_WATER_CHANGE = 71,// 上次换水时间
    PUMP_RUNTIME = 72,    // 水泵运行时长
    HEATER_RUNTIME = 73,  // 加热器运行时长
    LIGHT_RUNTIME = 74,   // 灯光运行时长
    UPTIME = 75,          // 系统运行时间

    COUNT = 76            // 总数量（必须放最后）
};

struct DataPoint {
    DataPointType type;			// 数据类型："temperature", "humidity", "state", "duty_cycle"
	DataValue value;			// 属性值
    std::string unit;           // 单位："°C", "%", "pH", "mg/L", "" (开关无单位)
    uint8_t quality = 0;		// 0=有效, 1=可疑, 2=无效

	// ---------- setValue 方法（核心） ----------

	void setValue(DataValue v) {
		value = v;
	}

	/**
	 * @brief 设置数值（float 类型）
	 */
	void setValue(float v) {
		value = v;
	}

	/**
	 * @brief 设置数值（int 类型）
	 */
	void setValue(int v) {
		value = static_cast<int32_t>(v);
	}

	/**
	 * @brief 设置数值（bool 类型）
	 */
	void setValue(bool v) {
		value = v;
	}

	/**
	 * @brief 设置数值（const char* 字符串）
	 */
	void setValue(const char* v) {
		value = std::string(v);
	}

	/**
	 * @brief 设置数值（std::string 类型）
	 */
	void setValue(const std::string& v) {
		value = v;
	}

	/**
	 * @brief 设置数值（int8_t 类型）
	 */
	void setValue(int8_t v) {
		value = v;
	}

	/**
	 * @brief 设置数值（uint8_t 类型）
	 */
	void setValue(uint8_t v) {
		value = v;
	}

	/**
	 * @brief 设置数值（int16_t 类型）
	 */
	void setValue(int16_t v) {
		value = v;
	}

	/**
	 * @brief 设置数值（uint16_t 类型）
	 */
	void setValue(uint16_t v) {
		value = v;
	}

	/**
	 * @brief 设置数值（int32_t 类型）
	 */
	void setValue(int32_t v) {
		value = v;
	}

	/**
	 * @brief 设置数值（uint32_t 类型）
	 */
	void setValue(uint32_t v) {
		value = v;
	}

	// ---------- 模板版本（更简洁） ----------

	/**
	 * @brief 通用 setValue（模板方式）
	 */
	template<typename T>
	void setValue(const T& v) {
		value = v;
	}

    // 辅助方法
    bool isValid() const { return quality == 0; }
    bool isSuspect() const { return quality == 1; }
    bool isInvalid() const { return quality >= 2; }

	std::string getValueString(int precision = 2) const {
        return std::visit([precision](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, float>) {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.*f", precision, arg);
                return std::string(buffer);
            }
            else if constexpr (std::is_same_v<T, double>) {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.*f", precision, arg);
                return std::string(buffer);
            }
            else if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, long>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, long long>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, unsigned int>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, unsigned long>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, unsigned long long>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, int8_t>) {
                return std::to_string(static_cast<int>(arg));
            }
            else if constexpr (std::is_same_v<T, uint8_t>) {
                return std::to_string(static_cast<unsigned int>(arg));
            }
            else if constexpr (std::is_same_v<T, int16_t>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, uint16_t>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, int32_t>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, uint32_t>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, const char*>) {
                return std::string(arg);
            }
            else if constexpr (std::is_same_v<T, char>) {
                return std::string(1, arg);
            }
            else {
                return "";
            }
        }, value);
    }
};

// ============================================================================
// 宏定义
// ============================================================================

#define DECLARE_TYPE(ClassName, TypeId) \
public: \
static constexpr uint32_t TYPE_ID = TypeId; \
static constexpr const char* CLASS_NAME = #ClassName; \
uint32_t getTypeId() const { return TYPE_ID; } \
const char* getTypeName() const { return CLASS_NAME; }

class IDeviceData {
public:

	 uint32_t getTypeId() const {return 0;};
	 const char* getTypeName() const {return "";};

	// ========== Getter（非虚，所有子类共用） ==========
	std::string getDeviceId() const { return m_deviceId; }
	std::string getDeviceName() const { return m_deviceName; }
	uint32_t getTimestamp() const { return m_timestamp; }
	DeviceType getDeviceType() const { return m_deviceType; }
	const std::map<std::string, std::string>& getMetadata() const {
		return m_metadata;
	}
	DeviceDataType getDataType() const { return m_data_type; }

	// ========== Setter（非虚，所有子类共用） ==========
	void setDeviceId(const std::string& id) { m_deviceId = id; }
	void setDeviceName(const std::string& name) { m_deviceName = name; }
	void setTimestamp(uint32_t ts) { m_timestamp = ts; }
	void setDeviceType(DeviceType type) { m_deviceType = type; }
	void setMetadata(const std::string& key, const std::string& value) {
		m_metadata[key] = value;
	}
	bool hasMetadata(const std::string& key) const {
		return m_metadata.find(key) != m_metadata.end();
	}
	void setDataType(DeviceDataType dataType) { m_data_type = dataType; }

	template<typename T>
		bool isType() {
		return getTypeId() == T::TYPE_ID;
	}

private:
	std::string m_deviceId; // 设备唯一标识，如 "sensor_01"
	std::string m_deviceName; //设备名称
	uint32_t m_timestamp; // 采集时间 "2026-07-23 14:30:25"
	DeviceType m_deviceType;
	DeviceDataType m_data_type;
	std::map<std::string, std::string> m_metadata;  // 元数据
};

class DeviceRecord : public IDeviceData {
	DECLARE_TYPE(DeviceRecord, 0x0001)
public:
	std::vector<DataPoint> points; // ✅ 采集的数据点列表
	/**
		 * @brief 添加数据点，并自动更新 data_quality
		 * @param point 要添加的数据点
		 * @param autoUpdateQuality 是否自动更新 data_quality（默认 true）
		 * @return 添加成功返回 true，失败返回 false
		 */
	bool addDataPoint(const DataPoint &point, bool autoUpdateQuality = true) {
		// 1. 基本的有效性检查
		if (point.unit.empty()) {
			// 对于非开关类型，建议有单位，但不强制
			// 可以记录警告日志，这里仅做示例
		}
		// 2. 添加数据点到列表
		points.push_back(point);
		// 3. 自动更新数据质量
		// if (autoUpdateQuality) {
		// 	updateDataQuality();
		// }

		return true;
	}
	template<typename Callback>
	void forEachDataPoint(Callback&& callback) const {
		for (size_t i = 0; i < points.size(); i++) {
			callback(i, points[i]);
		}
	}
};

class DeviceSwitch: public IDeviceData {
	DECLARE_TYPE(DeviceCommand, 0x0002)
public:
	void addRelay(uint8_t channel, bool state) {
		relays[m_relay_count].channel = channel;
		relays[m_relay_count].state = state;
		m_relay_count++;
	}

private:
	// 通道号（0-8）
	struct RelayEntry {
		uint8_t channel;
		bool state = false;
	};
	RelayEntry relays[MAX_SWITCH_SIZE] = {};
	uint8_t m_relay_count = 0;
};

class DevicePwm : public IDeviceData {
	DECLARE_TYPE(DeviceCommand, 0x0003)
	// 通道号（0-15）
	uint8_t channel;
	// 占空比（0-10000，表示0.00%-100.00%）
	uint16_t duty;
	// 频率（Hz，0表示使用默认频率）
	uint32_t frequency;
	// 使能状态
	bool enable;

	// 构造函数
	DevicePwm() : channel(0), duty(0), frequency(0), enable(false) {}

	DevicePwm(uint8_t ch, uint16_t d, uint32_t freq = 0, bool en = true)
		: channel(ch), duty(d), frequency(freq), enable(en) {}

	// 校验函数
	bool isValid() const {
		return channel <= 15 && duty <= 10000;
	}

	// 占空比百分比（0-100%）
	float getDutyPercent() const {
		return duty / 100.0f;
	}

	// 设置百分比占空比
	void setDutyPercent(float percent) {
		if (percent < 0) percent = 0;
		if (percent > 100) percent = 100;
		duty = static_cast<uint16_t>(percent * 100);
	}
};

class DeviceSetup : public IDeviceData {
	DECLARE_TYPE(DeviceCommand, 0x0004)
	uint32_t interval{};   // 采集间隔
};

class DeviceCollection : public IDeviceData {
	DECLARE_TYPE(DeviceCommand, 0x0005)
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
    virtual bool executeCommand(const DevicePwm& cmd) = 0;
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
