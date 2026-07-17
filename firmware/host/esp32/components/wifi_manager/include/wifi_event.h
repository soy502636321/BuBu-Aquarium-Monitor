/*
 * wifi_event.h
 *
 *  Created on: 2026年7月15日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_WIFI_EVENT_H_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_WIFI_EVENT_H_

#include "esp_event.h"
#include <stdint.h>

ESP_EVENT_DECLARE_BASE(WIFI_USER_EVENT);

enum WiFiUserEvent : uint8_t
{
    WIFI_CONNECT = 0,
    // -------------------------
    WIFI_EVENT_COUNT  // 计数标记
};


#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_WIFI_EVENT_H_ */
