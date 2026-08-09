//
// Created by Hu on 2026-07-28.
//

#ifndef UTILS_CRC16_HPP
#define UTILS_CRC16_HPP

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "Logger.hpp"

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
		static inline bool verify(const std::vector<uint8_t> data,size_t length) {
			return verify(data.data(), length);
		}


		static inline bool verify(const uint8_t* data,size_t length)
		{
			printf("%d bytes received\r\n", length);
			if(length < 3)
			{
				return false;
			}
			printf("OK 1 \r\n");
			// 接收到的CRC
			uint16_t recvCRC = data[length - 2] | (data[length - 1] << 8);
			printf("OK 2 \r\n");
			// 计算CRC(不包含最后两个CRC字节)
			uint16_t calcCRC = CRC16::calc(data, length - 2);
			printf("OK 3 \r\n");
			printf("Recv CRC: 0x%04X, Calc CRC: 0x%04X, %s\r\n",
				   recvCRC,
				   calcCRC,
				   (recvCRC == calcCRC) ? "YES" : "NO");
			return recvCRC == calcCRC;
		}

	};

#endif // UTILS_CRC16_HPP
