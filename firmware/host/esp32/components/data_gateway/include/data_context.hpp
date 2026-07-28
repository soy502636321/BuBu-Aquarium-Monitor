//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_DATA_CONTEXT_HPP
#define ESP32_DATA_CONTEXT_HPP

#include "data_packet.hpp"

		struct DataContext
{
	// 协议解析结果
		DataPacket packet;

	// 当前产生的事件
	//Event event;
};

#endif // ESP32_DATA_CONTEXT_HPP
