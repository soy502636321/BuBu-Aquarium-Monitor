/*
 * ui_extension.cpp
 *
 *  Created on: 2026年7月19日
 *      Author: Hu
 */

#include "ui_extension.h"
#include "lvgl.h"
#include "esp_timer.h"
#include <functional>
#include <vector>
#include "screens.h"
#include "esp_log.h"
#include "event_bus.h"
#include "ui_event.h"

static constexpr const char* TAG = "BuBu-Aquarium-Monitor[ui_extension]";

esp_timer_handle_t toast_timer;

extern "C" void on_show_loading(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	UILoadingData* data = static_cast<UILoadingData*>(event_data);
	ESP_LOGI(TAG, "UI事件调用【%s】", data->text.c_str());
	lv_label_set_text(objects.loading__text, data->text.c_str());
	lv_obj_clear_flag(objects.loading, LV_OBJ_FLAG_HIDDEN);     // 显示加载
}


extern "C" void on_hide_loading(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	ESP_LOGI(TAG, "UI事件调用");
	lv_obj_add_flag(objects.loading, LV_OBJ_FLAG_HIDDEN);     // 隐藏
	
	if (event_data != nullptr) {
		UILoadingData* data = static_cast<UILoadingData*>(event_data);
		if (data->has_callback()) {
			data->execute_callback(true);		
		}		
	}
}

extern "C" void on_show_success_toast(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	UIToastData* data = static_cast<UIToastData*>(event_data);
	ESP_LOGI(TAG, "显示成功Toast【%s】", data ? data->message.c_str() : "");
	lv_obj_clear_flag(objects.success_toast, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(objects.success_toast__message, data ? data->message.c_str() : "");
	esp_timer_start_once(toast_timer, 5 * 1000 * 1000);  // 5秒 
}

extern "C" void on_show_warning_toast(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	UIToastData* data = static_cast<UIToastData*>(event_data);
	ESP_LOGI(TAG, "显示警告Toast【%s】", data ? data->message.c_str() : "");
	lv_obj_clear_flag(objects.warning_toast, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(objects.warning_toast__message, data ? data->message.c_str() : "");
	esp_timer_start_once(toast_timer, 5 * 1000 * 1000);  // 5秒 
}

extern "C" void on_show_info_toast(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	UIToastData* data = static_cast<UIToastData*>(event_data);
	ESP_LOGI(TAG, "显示信息Toast【%s】", data ? data->message.c_str() : "");
	lv_obj_clear_flag(objects.info_toast, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(objects.info_toast__message, data ? data->message.c_str() : "");
	esp_timer_start_once(toast_timer, 5 * 1000 * 1000);  // 5秒 
}

extern "C" void on_show_danger_toast(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	UIToastData* data = static_cast<UIToastData*>(event_data);
	ESP_LOGI(TAG, "显示错误Toast【%s】", data ? data->message.c_str() : "");
	lv_obj_clear_flag(objects.danger_toast, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(objects.danger_toast__message, data ? data->message.c_str() : "");
	esp_timer_start_once(toast_timer, 5 * 1000 * 1000);  // 5秒 
}

extern "C" void on_toast_timeout_close(void* arg) {
	ESP_LOGI(TAG, "自定关闭Toast");
	lv_obj_add_flag(objects.success_toast, LV_OBJ_FLAG_HIDDEN);  
	lv_obj_add_flag(objects.warning_toast, LV_OBJ_FLAG_HIDDEN); 
	lv_obj_add_flag(objects.danger_toast, LV_OBJ_FLAG_HIDDEN);  
	lv_obj_add_flag(objects.info_toast, LV_OBJ_FLAG_HIDDEN);  
}

extern "C" void ui_extension_init() {
	ESP_LOGI(TAG, "UI拓展初始化...");
	// 声明外部字体

	// ✅ 关键：指定自定义字体
	//lv_obj_set_style_text_font(objects.obj0__obj60__test_font, &extern_symbols, 0);
	
	// ✅ 设置文本（使用 UTF-8 编码）
	//lv_label_set_text(objects.obj0__obj60__test_font, "\xEF\x83\x81");  // 0xEA2E 的 UTF-8
    // ✅ 使用正确的字体名称
    //lv_obj_set_style_text_font(objects.obj0__obj60__test_font, &ui_font_fontawesome_symbol_expand, LV_STATE_DEFAULT);
    
    // 0xE600 的 UTF-8 编码: EE 98 80
    //lv_label_set_text(objects.obj0__obj60__test_font, "\xEE\x98\x80");
	//lv_label_set_text(objects.obj0__obj60__test_font, LV_SYMBOL_SETTINGS);
	
	if (toast_timer == NULL) {
		esp_timer_create_args_t args = {
                .callback = on_toast_timeout_close,
                .arg = 0,
                .dispatch_method = ESP_TIMER_TASK,
                .name = "toast_timer",
                .skip_unhandled_events = false
		};
		esp_err_t ret = esp_timer_create(&args, &toast_timer);
		if (ret != ESP_OK) {
			ESP_LOGE("ToastTimer", "❌ 创建定时器失败: %s", esp_err_to_name(ret));
			return;
		}
		ESP_LOGI("ToastTimer", "✅ 定时器创建成功");
	} 
		
	// 获取顶层图层（在屏幕之上，但在系统层之下）
	lv_obj_t *top_layer = lv_layer_top();
	// 重新父级到顶层图层
	lv_obj_set_parent(objects.loading, top_layer);
	lv_obj_move_foreground(objects.loading);
	
	lv_obj_set_parent(objects.success_toast, top_layer);
	lv_obj_move_foreground(objects.success_toast);
	
	lv_obj_set_parent(objects.warning_toast, top_layer);
	lv_obj_move_foreground(objects.warning_toast);
	
	lv_obj_set_parent(objects.danger_toast, top_layer);
	lv_obj_move_foreground(objects.danger_toast);
	
	lv_obj_set_parent(objects.info_toast, top_layer);
	lv_obj_move_foreground(objects.info_toast);
	
	ESP_LOGI(TAG, "UI拓展初始化完成...");
	
	// 监听自动事件
	EventBus::instance().subscribe(UI_EVENT, UI_SHOW_LOADING, on_show_loading, nullptr); // 显示加载页面
	EventBus::instance().subscribe(UI_EVENT, UI_HIDE_LOADING, on_hide_loading, nullptr); // 关闭加载页面
	
	EventBus::instance().subscribe(UI_EVENT, UI_SHOW_SUCCESS_TOAST, on_show_success_toast, nullptr); // 显示成功toast
	EventBus::instance().subscribe(UI_EVENT, UI_SHOW_WARNING_TOAST, on_show_warning_toast, nullptr); // 显示成功toast
	EventBus::instance().subscribe(UI_EVENT, UI_SHOW_INFO_TOAST, on_show_info_toast, nullptr); // 显示成功toast
	EventBus::instance().subscribe(UI_EVENT, UI_SHOW_DANGER_TOAST, on_show_danger_toast, nullptr); // 显示成功toast
	
}

extern "C" void ui_extension_tick() {
}
