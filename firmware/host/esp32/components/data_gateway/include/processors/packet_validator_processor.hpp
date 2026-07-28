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

	bool execute(DataContext& ctx) override
	{
		ESP_LOGI(TAG, "验证数据是否合格");
		uint8_t test_data[] =
		{
			0xAA, 0x55,           // 帧头
			0x01,                 // 版本
			0x02,                 // 类型
			0x00, 0x08,           // 长度
			0x02,                 // 序号
			0x01, 0x02, 0x02, 0x01, 0xF4,  // 数据体
			0x02, 0x01, 0x01, 0x01,        // 数据体
			0xD6, 0x9C            // CRC16 (低字节在前)
		};

		size_t len = sizeof(test_data);
		bool verify = Utils::CRC16::verify(test_data, len);
		// 计算CRC(不包含最后两个CRC字节)
		uint16_t calcCRC = Utils::CRC16::calc(test_data, len - 2);

		ESP_LOGI(TAG, "验证结果%s", verify ? "true" : "false");
		ESP_LOGI(
			"CRC",
			"CRC=0x%04X",
			calcCRC
		);
		// --- 新增测试2：使用你原有的完整数据，但不包含CRC字节 ---
		uint8_t test_data_without_crc[] = {
			0xAA, 0x55,           // 帧头
			0x01,                 // 版本
			0x02,                 // 类型
			0x00, 0x08,           // 长度
			0x02,                 // 序号
			0x01, 0x02, 0x02, 0x01, 0xF4,  // 数据体
			0x02, 0x01, 0x01, 0x01,        // 数据体
		};
		uint16_t crc_data = Utils::CRC16::calc(test_data_without_crc, sizeof(test_data_without_crc));
		ESP_LOGI(TAG, "完整数据（不含CRC）的CRC: 0x%04X", crc_data);
		return true;
	}
};



#endif