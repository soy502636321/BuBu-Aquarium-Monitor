//
// Created by Hu on 2026-07-28.
// esp 日志
//

#ifndef ESP32_ESP_LOG_OUTPUT_HPP
#define ESP32_ESP_LOG_OUTPUT_HPP

#pragma once

#define TAG "BuBu-Aquarium-Monitor[esp_log_output]"

#include "data_output.hpp"
#include "esp_log.h"

class EspLogOutput : public IDataOutput
{

public:

	void handle(const DataContext& ctx) override {
		ESP_LOGI(TAG, "输出采集数据");
	}
};
#endif // ESP32_ESP_LOG_OUTPUT_HPP
