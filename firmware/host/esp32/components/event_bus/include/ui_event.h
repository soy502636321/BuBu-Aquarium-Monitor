/*
 * ui_event.h
 *
 *  Created on: 2026年7月20日
 *      Author: Hu
 */

#ifndef COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_
#define COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_

#include "esp_event.h"
#include "event_bus.h"
#include <stdint.h>
#include <string>

ESP_EVENT_DECLARE_BASE(UI_EVENT);

enum UIEvent : uint8_t
{
    UI_SHOW_LOADING = 0,
    UI_HIDE_LOADING,
    
    UI_SHOW_SUCCESS_TOAST,
    UI_SHOW_WARNING_TOAST,
    UI_SHOW_DANGER_TOAST,
    UI_SHOW_INFO_TOAST,
    // -------------------------
    UI_EVENT_COUNT  // 计数标记
};

struct UILoadingData : public EventData {
    std::string text;
    uint32_t autoCloseSeconds;
    
    UILoadingData() {
		text = "正在加载...";
		autoCloseSeconds = 0;
	}
    
    /**
     * @brief 带参数的构造函数
     * @param t 提示文字
     * @param seconds 自动关闭秒数
     * @param cb 完成回调
     * @param ud 用户数据
     */
    UILoadingData(const std::string& t, uint32_t seconds = 0, EventCallback cb = nullptr, void* ud = nullptr) 
        : EventData(cb, ud)  // 调用基类构造函数设置回调
        , text(t)
        , autoCloseSeconds(seconds) {}
    
    /**
     * @brief 便捷构造：只设置文字和回调
     */
    UILoadingData(const std::string& t, EventCallback cb)
        : EventData(cb)
        , text(t)
        , autoCloseSeconds(0) {}
};

struct UIToastData : public EventData {
    std::string message;
    uint32_t autoCloseSeconds = 3;
    
    UIToastData() {
		message = "";
		autoCloseSeconds = 0;
	}
    
    /**
     * @brief 便捷构造：只设置文字和回调
     */
    UIToastData(const std::string& t)
        : EventData(nullptr)
        , message(t)
        , autoCloseSeconds(3) {}
};

#endif /* COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_ */
