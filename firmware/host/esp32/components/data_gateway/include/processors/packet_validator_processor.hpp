//
// Created by Hu on 2026-07-28.
//
#ifndef PACKET_VALIDATOR_PROCESSOR
#define PACKET_VALIDATOR_PROCESSOR

#include "data_processor.hpp"
#include "utils.hpp"
#include "esp_crc.h"
#include "esp_log.h"

#define TAG "BuBu-Aquarium-Monitor[packet_validator_processor]"

class PacketValidatorProcessor : public IDataProcessor
{

public:

	/**
	 *		0xAA, 0x55,           // 帧头
			0x01,                 // 版本
			0x02,                 // 类型
			0x00, 0x08,           // 长度
			0x02,                 // 序号
			0x01, 0x02, 0x02, 0x01, 0xF4,  // 数据体
			0x02, 0x01, 0x01, 0x01,        // 数据体
			HEX格式：AA 55 01 02 00 08 02 01 02 02 01 F4 02 01 01 01 D6 9C
	 */
	bool execute(DataContext& ctx) override
	{
		uint8_t* payload = ctx.packet.payload.data();  // 获取 vector 底层指针
		size_t length = ctx.packet.payload.size();     // 获取数据长度
		// 验证数据是否完整
		bool verify = Utils::CRC16::verify(payload, length);
		if (!verify) {
			ESP_LOGI(TAG, "数据已损坏，无法继续下一步解析处理");
		}
		return verify;
	}
};

#endif