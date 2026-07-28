//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_DATA_PACKET_HPP
#define ESP32_DATA_PACKET_HPP

#include "device.hpp"

// 数据来源
enum class DataSource
{
	UART,
	BLE
};

// 数据类型
enum class DataType
{
	DATA,
	STATUS,
	COMMAND,
	ACK
};

struct DataPacket
{
	DataSource source;

	DataType type;

	uint32_t device_id;

	uint32_t timestamp;

	bool valid;

	union
	{
		DeviceRecord* record;

		DeviceCommand* command;
	} data;

	//去掉头尾后的原始数据
	std::vector<uint8_t> payload;
};

#endif // ESP32_DATA_PACKET_HPP
