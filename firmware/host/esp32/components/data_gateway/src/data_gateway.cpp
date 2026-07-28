/*
 * data_gateway.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */
#include "data_gateway.hpp"
#include "data_context.hpp"
#include "processors/hex_to_packet_processor.hpp"
#include "processors/packet_validator_processor.hpp"
#include "processors/output_dispatcher_processor.hpp"

#include "esp_log.h"
#include "esp_timer.h"

#define TAG "BuBu-Aquarium-Monitor[data_gateway]"

DataGateway& DataGateway::instance()
{
	static DataGateway instance;

	return instance;
}

void DataGateway::init() {
	if (initialized) return;

	setupPipeline();

	initialized = true;
}

void DataGateway::setupPipeline() {
	pipeline
		.addProcessor(new PacketValidatorProcessor())  // 第一步先验证数据是否正确
		.addProcessor(new HexToPacketProcessor())
		.addProcessor(new OutputDispatcherProcessor())
	;
}

void DataGateway::receive(const uint8_t* data, size_t length, DataSource source) {
	if(data == nullptr || length == 0)
	{
		ESP_LOGW(TAG, "错误的数据");
		return;
	}

	if(pipeline.size() <= 0)
	{
		ESP_LOGW(TAG, "数据处理管道没添加");
		return;
	}

	DataContext context;

	/*
	 * 创建 DataPacket
	 */
	context.packet.source = source;
	context.packet.timestamp = esp_timer_get_time() / 1000;
	// 保存原始HEX
	context.packet.payload.assign(data, data + length);
	/*
	 * 默认状态
	 */
	context.packet.valid = false;
	/*
	 * 进入处理管道
	 */
	pipeline.process(context);
}




