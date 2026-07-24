/*
 * wifi_event.h
 *
 *  Created on: 2026年7月15日
 *      Author: Hu
 */

#ifndef COMPONENTS_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_EVENT_H_
#define COMPONENTS_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_EVENT_H_

#include "esp_event.h"
#include <stdint.h>

ESP_EVENT_DECLARE_BASE(BLUETOOTH_USER_EVENT);

enum BluetoothUserEvent : uint8_t
{
    BLUETOOTH_CONNECT = 0,
    // -------------------------
    BLUETOOTH_EVENT_COUNT  // 计数标记
};


#endif /* COMPONENTS_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_EVENT_H_ */
