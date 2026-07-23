#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void action_home_click(lv_event_t * e);

enum {
    ACTION_ROUTER_PUSH_PROPERTY_PAGE,
};
extern void action_router_push(lv_event_t * e);

extern void action_router_back(lv_event_t * e);
extern void action_device_firmware_ota(lv_event_t * e);
extern void action_wifi_scan_start(lv_event_t * e);
extern void action_printf_hw(lv_event_t * e);
extern void action_switch_wifi_enabled(lv_event_t * e);

enum {
    ACTION_ON_CONNECT_BLUETOOTH_PROPERTY_RECORD_INDEX,
};
extern void action_on_connect_bluetooth(lv_event_t * e);

extern void action_bluetooth_record_update_event(lv_event_t * e);
extern void action_bluetooth_start_scan(lv_event_t * e);
extern void action_bluetooth_stop_scan(lv_event_t * e);

enum {
    ACTION_ON_PWM_VALUE_CHANGED_PROPERTY_PWD_INDEX,
    ACTION_ON_PWM_VALUE_CHANGED_PROPERTY_PWM_VALUE,
};
extern void action_on_pwm_value_changed(lv_event_t * e);

enum {
    ACTION_ON_SWITCH_VALUE_CHANGED_PROPERTY_SWITCH_CHECKED,
};
extern void action_on_switch_value_changed(lv_event_t * e);

extern void action_on_wifi_enabled(lv_event_t * e);
extern void action_on_wifi_disabled(lv_event_t * e);
extern void action_on_wifi_status_switch(lv_event_t * e);

enum {
    ACTION_ON_WIFI_CONNECT_PROPERTY_RECORD_INDEX,
    ACTION_ON_WIFI_CONNECT_PROPERTY_PASSWORD,
};
extern void action_on_wifi_connect(lv_event_t * e);

extern void action_printf(lv_event_t * e);
extern void action_show_loading(lv_event_t * e);
extern void action_close_loading(lv_event_t * e);
extern void action_update_loading(lv_event_t * e);

void actions_init();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/