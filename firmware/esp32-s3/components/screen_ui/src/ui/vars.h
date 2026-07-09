#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

typedef enum {
    Device_Type_t_device_unknown = 0,
    Device_Type_t_sensor_water_temp = 1,
    Device_Type_t_sensor_water_level = 2
} Device_Type_t;

typedef enum {
    content_page_t_HOME_PAGE = 0,
    content_page_t_DEVICE_SHORTCUT_PAGE = 1,
    content_page_t_DEVICE_SETTING_PAGE = 2,
    content_page_t_MESSAGE_PAGE = 3,
    content_page_t_SETTING_PAGE = 4
} content_page_t;

typedef enum {
    Setting_Content_Page_Basic = 0,
    Setting_Content_Page_Network = 1,
    Setting_Content_Page_Data = 2,
    Setting_Content_Page_Display = 3
} Setting_Content_Page;

typedef enum {
    Setting_Network_Page_t_Default = 0,
    Setting_Network_Page_t_WiFi = 1,
    Setting_Network_Page_t_Bluetooth = 2
} Setting_Network_Page_t;

typedef enum {
    WiFi_Status_IDLE = 0,
    WiFi_Status_INIT = 1,
    WiFi_Status_SCANNING = 2,
    WiFi_Status_CONNECTING = 3,
    WiFi_Status_CONNECTED = 4,
    WiFi_Status_DISCONNECTED = 5,
    WiFi_Status_FAILED = 6
} WiFi_Status;

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_HOME_HIDDEN = 0,
    FLOW_GLOBAL_VARIABLE_HOME_CONTNET_HIDDEN = 1,
    FLOW_GLOBAL_VARIABLE_MESSAGE_CONTENT_HIDDEN = 2,
    FLOW_GLOBAL_VARIABLE_DEVICE_CONTENT_HIDDEN = 3,
    FLOW_GLOBAL_VARIABLE_SETTING_CONTENT_HIDDEN = 4,
    FLOW_GLOBAL_VARIABLE_MESSAGE_QUEUE_INDEX = 5,
    FLOW_GLOBAL_VARIABLE_MESSAGE_QUEUE = 6,
    FLOW_GLOBAL_VARIABLE_DEVICE_QUEUE = 7,
    FLOW_GLOBAL_VARIABLE_TXT_UNKNOWN = 8,
    FLOW_GLOBAL_VARIABLE_TXT_WATER_TEMP = 9,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_NAME = 10,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_NAME_FIELD = 11,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_VERSION = 12,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_VERION_FIELD = 13,
    FLOW_GLOBAL_VARIABLE_TXT_SETTING_BASIC_FIELD = 14,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_ID_FIELD = 15,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_ID = 16,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_FIRMWARE_VERSION = 17,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_FIRMWARE_VERSION_FIELD = 18,
    FLOW_GLOBAL_VARIABLE_TXT_DEVICE_OTA_FIELD = 19,
    FLOW_GLOBAL_VARIABLE_TXT_SETTING_DATA_FIELD = 20,
    FLOW_GLOBAL_VARIABLE_TXT_SETTING_NETWORK_FIELD = 21,
    FLOW_GLOBAL_VARIABLE_TXT_SETTING_DISPLAY_FIELD = 22,
    FLOW_GLOBAL_VARIABLE_TXT_CLOSE = 23,
    FLOW_GLOBAL_VARIABLE_TXT_DISCONNECT = 24,
    FLOW_GLOBAL_VARIABLE_TXT_BLUETOOTH = 25,
    FLOW_GLOBAL_VARIABLE_TXT_BACK = 26,
    FLOW_GLOBAL_VARIABLE_TXT_RETENTION_PERIOD = 27,
    FLOW_GLOBAL_VARIABLE_TXT_SCREEN_BRIGHTNESS = 28,
    FLOW_GLOBAL_VARIABLE_TXT_AUTO_BRIGHTNESS = 29,
    FLOW_GLOBAL_VARIABLE_WIFI_RECORD_LIST = 30,
    FLOW_GLOBAL_VARIABLE_WIFI_RECORD_CONNECTED = 31,
    FLOW_GLOBAL_VARIABLE_BLUETOOTH_RECORD_LIST = 32
};

// Native global variables

extern int32_t get_var_content_page();
extern void set_var_content_page(int32_t value);
extern WiFi_Status get_var_wifi_status();
extern void set_var_wifi_status(WiFi_Status value);
extern bool get_var_wifi_enabled();
extern void set_var_wifi_enabled(bool value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/