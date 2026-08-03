//
// Created by Hu on 2026-07-29.
//

#ifndef APP_TEMPLATE_VALUE_TYPE_HPP
#define APP_TEMPLATE_VALUE_TYPE_HPP

#include <variant>

enum class ValueType : uint8_t {
	FLOAT = 0,
	INT8 = 1,
	UINT8 = 2,
	INT16 = 3,
	UINT16 = 4,
	INT32 = 5,
	UINT32 = 6,
	BOOL = 7,
	STRING = 8
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

#endif // APP_TEMPLATE_VALUE_TYPE_HPP
