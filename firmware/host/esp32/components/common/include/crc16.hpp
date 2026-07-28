//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_CRC16_HPP
#define ESP32_CRC16_HPP

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace Utils
{

	class CRC16
	{

	public:

		static inline uint16_t calc(const uint8_t* data, size_t length) {
			uint16_t crc = 0xFFFF;

			for(size_t i = 0; i < length; i++)
			{
				crc ^= data[i];


				for(uint8_t j = 0; j < 8; j++)
				{
					if(crc & 0x0001)
					{
						crc =
							(crc >> 1)
							^ 0xA001;
					}
					else
					{
						crc >>= 1;
					}
				}
			}


			return crc;
		}



		static inline bool verify(
			const uint8_t* data,
			size_t length)
		{
			if(length < 3)
			{
				return false;
			}
			// 接收到的CRC
			uint16_t recvCRC = data[length - 2] | (data[length - 1] << 8);
			// 计算CRC(不包含最后两个CRC字节)
			uint16_t calcCRC = Utils::CRC16::calc(data, length - 2);

			return recvCRC == calcCRC;
		}

	};

}

#endif // ESP32_CRC16_HPP
