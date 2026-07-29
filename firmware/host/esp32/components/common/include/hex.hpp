//
// Created by Hu on 2026-07-29.
//

#ifndef APP_TEMPLATE_HEX_HPP
#define APP_TEMPLATE_HEX_HPP

#include <cstring>
#include <string>
#include <cstdint>
#include <type_traits>
#include "value_type.hpp"

namespace Utils {
	namespace  HEX {
		template<typename T>
		inline T hex_to_value(const uint8_t* data, size_t offset = 0, bool littleEndian = true) {
			// 1. 空指针检查
			if (data == nullptr) {
				return T{};
			}

			// 2. 获取类型大小
			constexpr size_t size = sizeof(T);

			// 3. 如果类型是 std::string，特殊处理
			if constexpr (std::is_same_v<T, std::string>) {
				const char* str = reinterpret_cast<const char*>(data + offset);
				return std::string(str);
			}

			// 4. 如果是 bool 类型，特殊处理
			if constexpr (std::is_same_v<T, bool>) {
				return data[offset] != 0;
			}

			// 5. 如果是浮点数类型，使用 memcpy 处理
			if constexpr (std::is_floating_point_v<T>) {
				uint8_t buffer[size];
				if (littleEndian) {
					for (size_t i = 0; i < size; i++) {
						buffer[i] = data[offset + i];
					}
				} else {
					for (size_t i = 0; i < size; i++) {
						buffer[i] = data[offset + (size - 1 - i)];
					}
				}
				T value;
				std::memcpy(&value, buffer, size);
				return value;
			}

			// 6. 对于整数类型，按字节序组装
			// 使用 else 确保只有整数类型才执行这里
			else {
				T value = T{};
				if (littleEndian) {
					for (size_t i = 0; i < size; i++) {
						uint8_t byte = data[offset + i];
						value |= (static_cast<T>(byte) << (i * 8));
					}
				} else {
					for (size_t i = 0; i < size; i++) {
						uint8_t byte = data[offset + i];
						value |= (static_cast<T>(byte) << ((size - 1 - i) * 8));
					}
				}
				return value;
			}
		}

		inline DataValue hex_to_value(ValueType VT, const uint8_t* data, size_t offset = 0, bool littleEndian = true) {
			switch (VT) {
				case ValueType::FLOAT:
					return hex_to_value<float>(data, offset, littleEndian);
			case ValueType::INT8:
				return hex_to_value<int8_t>(data, offset, littleEndian);
			case ValueType::UINT8:
				return hex_to_value<uint8_t>(data, offset, littleEndian);
			case ValueType::INT16:
				return hex_to_value<int16_t>(data, offset, littleEndian);
			case ValueType::UINT16:
				return hex_to_value<uint16_t>(data, offset, littleEndian);
			case ValueType::INT32:
				return hex_to_value<int32_t>(data, offset, littleEndian);
			case ValueType::UINT32:
				return hex_to_value<uint32_t>(data, offset, littleEndian);
			case ValueType::BOOL:
				return hex_to_value<bool>(data, offset, littleEndian);
				default:
					return float{0.0F};
			}
		}
	};
};

#endif // APP_TEMPLATE_HEX_HPP
