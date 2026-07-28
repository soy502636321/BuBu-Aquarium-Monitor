//
// Created by Hu on 2026-07-28.
// 本地存储
//

#ifndef ESP32_STORAGEOUTPUT_HPP
#define ESP32_STORAGEOUTPUT_HPP

#define TAG "BuBu-Aquarium-Monitor[storage_output]"

#include "data_output.hpp"
#include "esp_log.h"

class StorageOutput : public IDataOutput
{

public:

	void handle(const DataContext& ctx) override {
		ESP_LOGI(TAG, "输出采集数据");
		// if(ctx.packet.type == DataType::DEVICE_RECORD)
		// {
		// 	ESP_LOGI(
		// 		tag,
		// 		"Device Record id=%lu",
		// 		ctx.packet.device_id
		// 	);
		//
		//
		// 	ESP_LOGI(
		// 		tag,
		// 		"Payload size=%d",
		// 		ctx.packet.payload.size()
		// 	);
		// }
		//
		//
		// else if(ctx.packet.type == DataType::DEVICE_COMMAND)
		// {
		// 	ESP_LOGI(
		// 		tag,
		// 		"Device Command id=%lu",
		// 		ctx.packet.device_id
		// 	);
		// }
		//
		//
		// if(ctx.event.type != EventType::NONE)
		// {
		// 	ESP_LOGW(
		// 		tag,
		// 		"Event=%d",
		// 		(int)ctx.event.type
		// 	);
		// }

	}
};

#endif // ESP32_STORAGEOUTPUT_HPP
