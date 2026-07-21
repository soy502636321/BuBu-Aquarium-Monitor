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
#include "event_bus.h"
#include "ui_event.h"

static constexpr const char* TAG = "BuBu-Aquarium-Monitor[ui_extension]";

extern "C" void on_show_loading(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	ESP_LOGI(TAG, "UI事件调用");
	lv_obj_clear_flag(objects.loading, LV_OBJ_FLAG_HIDDEN);     // 显示加载
}


extern "C" void on_hide_loading(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	ESP_LOGI(TAG, "UI事件调用");
	lv_obj_add_flag(objects.loading, LV_OBJ_FLAG_HIDDEN);     // 隐藏
}


extern "C" void ui_extension_init() {
	ESP_LOGI(TAG, "UI拓展初始化...");
		
	lv_obj_t *loading_obj = objects.loading;
	// 获取顶层图层（在屏幕之上，但在系统层之下）
	lv_obj_t *top_layer = lv_layer_top();
	// 重新父级到顶层图层
	lv_obj_set_parent(loading_obj, top_layer);
	lv_obj_move_foreground(loading_obj);
	ESP_LOGI(TAG, "UI拓展初始化完成...");
	
	// 监听自动事件
        EventBus::instance().subscribe(
            UI_EVENT,
            UI_SHOW_LOADING,  // ✅ 直接使用，不需要 static_cast
            on_show_loading,
            nullptr
        );
        
        EventBus::instance().subscribe(
            UI_EVENT,
            UI_HIDE_LOADING,  // ✅ 直接使用，不需要 static_cast
            on_hide_loading,
            nullptr
        );
}

extern "C" void ui_extension_tick() {
}
