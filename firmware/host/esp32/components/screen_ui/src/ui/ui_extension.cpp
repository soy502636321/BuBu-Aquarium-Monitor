/*
 * ui_extension.cpp
 *
 *  Created on: 2026年7月19日
 *      Author: Hu
 */

#include "ui_extension.h"
#include "lvgl.h"
#include "screens.h"
#include "esp_log.h"

static constexpr const char* TAG = "BuBu-Aquarium-Monitor[ui_extension]";

extern "C" void ui_extension_init() {
	ESP_LOGI(TAG, "UI拓展初始化...");
		
	lv_obj_t *loading_obj = objects.loading;
	// 获取顶层图层（在屏幕之上，但在系统层之下）
	lv_obj_t *top_layer = lv_layer_top();
	// 重新父级到顶层图层
	lv_obj_set_parent(loading_obj, top_layer);
	lv_obj_move_foreground(loading_obj);
	ESP_LOGI(TAG, "UI拓展初始化完成...");
}

extern "C" void ui_extension_tick() {
}
