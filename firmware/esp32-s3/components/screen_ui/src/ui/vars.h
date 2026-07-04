#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

typedef enum {
    device_type_t_device_unknown = 0,
    device_type_t_sensor_water_temp = 1,
    device_type_t_sensor_water_level = 2
} device_type_t;

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CONTENT_PAGE_INDEX = 0,
    FLOW_GLOBAL_VARIABLE_HOME_HIDDEN = 1,
    FLOW_GLOBAL_VARIABLE_HOME_CONTNET_HIDDEN = 2,
    FLOW_GLOBAL_VARIABLE_MESSAGE_CONTENT_HIDDEN = 3,
    FLOW_GLOBAL_VARIABLE_DEVICE_CONTENT_HIDDEN = 4,
    FLOW_GLOBAL_VARIABLE_SETTING_CONTENT_HIDDEN = 5,
    FLOW_GLOBAL_VARIABLE_MESSAGE_QUEUE_INDEX = 6,
    FLOW_GLOBAL_VARIABLE_MESSAGE_QUEUE = 7,
    FLOW_GLOBAL_VARIABLE_DEVICE_QUEUE = 8,
    FLOW_GLOBAL_VARIABLE_TXT_UNKNOWN = 9,
    FLOW_GLOBAL_VARIABLE_TXT_WATER_TEMP = 10
};

// Native global variables

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/