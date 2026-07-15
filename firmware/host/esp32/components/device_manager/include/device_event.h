/*
 * device_event.h
 *
 *  Created on: 2026年7月15日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_EVENT_H_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_EVENT_H_

#include "esp_event.h"
#include <stdint.h>

ESP_EVENT_DECLARE_BASE(DEVICE_EVENT);

enum DeviceEvent : uint8_t
{
    DEVICE_REGISTER = 0,
    DEVICE_UNREGISTER,
    DEVICE_DATA_UPDATE,       // 设备数据上报
    DEVICE_ALARM,             // 设备报警
    DEVICE_ERROR,             // 设备错误
    // -------------------------
    DEVICE_EVENT_COUNT  // 计数标记
};


#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_EVENT_H_ */
