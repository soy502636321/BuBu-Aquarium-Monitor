//
// Created by Hu on 2026-07-28.
// 本地存储
//

#ifndef ESP32_STORAGE_OUTPUT_HPP
#define ESP32_STORAGE_OUTPUT_HPP

#define TAG "BuBu-Aquarium-Monitor[storage_output]"

#include "data_output.hpp"
#include "esp_log.h"

class StorageOutput : public IDataOutput
{

public:

	void handle(const DataContext& ctx) override {
		ESP_LOGI(TAG, "输出采集数据");

	}
};

#endif // ESP32_STORAGE_OUTPUT_HPP
