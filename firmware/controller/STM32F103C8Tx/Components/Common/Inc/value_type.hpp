//
// Created by Hu on 2026-07-29.
//

#ifndef APP_TEMPLATE_VALUE_TYPE_HPP
#define APP_TEMPLATE_VALUE_TYPE_HPP

#include <variant>

enum class DataValueType : uint8_t {
	FLOAT = 0,
	INT8 = 1,
	UINT8 = 2,
	INT16 = 3,
	UINT16 = 4,
	INT32 = 5,
	UINT32 = 6,
	BOOL = 7,
	STRING = 8,
	UNKNOWN
};

// 定义 DataValue 类型别名
using DataValue = std::variant<
	float,      // 浮点数（温度、湿度、pH等）
	int8_t,     // 有符号8位整数
	uint8_t,    // 无符号8位整数（状态码等）
	int16_t,    // 16位整数
	uint16_t,   // 16位无符号整数
	int32_t,    // 32位整数
	uint32_t,   // 32位无符号整数
	bool,       // 布尔值（开关状态）
	std::string // 字符串（特殊场景：设备ID、文本状态等）
>;

// ========== 获取类型 ==========
inline DataValueType getValueType(const DataValue& value) {
	return std::visit([](auto&& arg) -> DataValueType {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, float>) return DataValueType::FLOAT;
		if constexpr (std::is_same_v<T, int8_t>) return DataValueType::INT8;
		if constexpr (std::is_same_v<T, uint8_t>) return DataValueType::UINT8;
		if constexpr (std::is_same_v<T, int16_t>) return DataValueType::INT16;
		if constexpr (std::is_same_v<T, uint16_t>) return DataValueType::UINT16;
		if constexpr (std::is_same_v<T, int32_t>) return DataValueType::INT32;
		if constexpr (std::is_same_v<T, uint32_t>) return DataValueType::UINT32;
		if constexpr (std::is_same_v<T, bool>) return DataValueType::BOOL;
		if constexpr (std::is_same_v<T, std::string>) return DataValueType::STRING;
		return DataValueType::UNKNOWN;
	}, value);
}


// ========== 获取数据长度 ==========
inline size_t getValueSize(const DataValue& value) {
	return std::visit([](auto&& arg) -> size_t {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, std::string>) {
			return arg.length();
		} else {
			return sizeof(T);
		}
	}, value);
}

inline std::vector<uint8_t> getValueHex(const DataValue& value) {
	std::vector<uint8_t> hex;

	std::visit([&hex](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, std::string>) {
			// 字符串：逐个字符
			for (char c : arg) {
				hex.push_back(static_cast<uint8_t>(c));
			}
		} else {
			// 其他类型：根据 sizeof 动态循环
			const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&arg);
			for (size_t i = 0; i < sizeof(T); i++) {
				hex.push_back(ptr[i]);
			}
		}
	}, value);

	return hex;
};

#endif // APP_TEMPLATE_VALUE_TYPE_HPP
