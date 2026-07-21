/*
 * ui_event.h
 *
 *  Created on: 2026年7月20日
 *      Author: Hu
 */

#ifndef COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_
#define COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_

#include "esp_event.h"
#include <stdint.h>
#include <string>

ESP_EVENT_DECLARE_BASE(UI_EVENT);

enum UIEvent : uint8_t
{
    UI_SHOW_LOADING = 0,
    UI_HIDE_LOADING,
    // -------------------------
    UI_EVENT_COUNT  // 计数标记
};

struct UILoadingData {
    std::string text;
    uint32_t autoCloseSeconds;
    
    UILoadingData() {
		text = "正在加载...";
		autoCloseSeconds = 0;
	}
    
    UILoadingData (const std::string& t, uint32_t seconds = 0): text(t), autoCloseSeconds(seconds) {}
};

#endif /* COMPONENTS_EVENT_BUS_INCLUDE_UI_EVENT_H_ */
