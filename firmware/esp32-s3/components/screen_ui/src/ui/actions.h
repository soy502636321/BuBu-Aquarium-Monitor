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
extern void action_connect_bluetooth(lv_event_t * e);
extern void action_bluetooth_record_update_event(lv_event_t * e);
extern void action_bluetooth_start_scan(lv_event_t * e);
extern void action_bluetooth_stop_scan(lv_event_t * e);

void actions_init();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/