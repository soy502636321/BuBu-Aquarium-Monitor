#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

typedef enum {
    message_level_MSG_LEVEL_DEBUG = 0,
    message_level_MSG_LEVEL_FATAL = 1,
    message_level_MSG_LEVEL_ALARM = 2,
    message_level_MSG_LEVEL_ERROR = 3,
    message_level_MSG_LEVEL_WARN = 4,
    message_level_MSG_LEVEL_INFO = 5
} message_level;

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CONTENT_PAGE = 0,
    FLOW_GLOBAL_VARIABLE_HOME_HIDDEN = 1,
    FLOW_GLOBAL_VARIABLE_HOME_CONTNET_HIDDEN = 2,
    FLOW_GLOBAL_VARIABLE_MESSAGE_CONTENT_HIDDEN = 3,
    FLOW_GLOBAL_VARIABLE_DEVICE_CONTENT_HIDDEN = 4,
    FLOW_GLOBAL_VARIABLE_SETTING_CONTENT_HIDDEN = 5
};

// Native global variables

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/