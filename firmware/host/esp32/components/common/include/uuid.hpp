//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_UUID_HPP
#define ESP32_UUID_HPP
#pragma once

#include <stddef.h>
#include "host/ble_uuid.h"

#pragma once

#include <stdio.h>
#include <string.h>

namespace Utils
{

class UUID
{

public:

	static inline void uuid128_to_str(const uint8_t uuid[16], char* buffer, bool standard = true) {
		if (uuid == nullptr || buffer == nullptr) {
			return;
		}

		if (standard) {
			// 标准格式: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
			snprintf(buffer, 37,
					"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
					uuid[0], uuid[1], uuid[2], uuid[3],
					uuid[4], uuid[5],
					uuid[6], uuid[7],
					uuid[8], uuid[9],
					uuid[10], uuid[11], uuid[12], uuid[13],
					uuid[14], uuid[15]);
		} else {
			// 紧凑格式: 32 位十六进制字符串
			char* p = buffer;
			for (int i = 0; i < 16; i++) {
				p += sprintf(p, "%02X", uuid[i]);
			}
		}
	}


};

}
#endif // ESP32_UUID_HPP
