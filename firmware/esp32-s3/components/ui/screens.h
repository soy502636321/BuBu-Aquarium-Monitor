#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN_SCREEN = 1,
    _SCREEN_ID_LAST = 1
};

typedef struct _objects_t {
    lv_obj_t *main_screen;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj2__obj0;
    lv_obj_t *obj2__obj0__obj0;
    lv_obj_t *obj2__header_state_led;
    lv_obj_t *obj3;
    lv_obj_t *header_container;
    lv_obj_t *header_logo_container;
    lv_obj_t *header_clock_container;
    lv_obj_t *header_btn_container;
    lv_obj_t *body_container;
    lv_obj_t *menu_container;
    lv_obj_t *view_container;
} objects_t;

extern objects_t objects;

void create_screen_main_screen();
void tick_screen_main_screen();

void create_user_widget_menu_fcuntion_btn(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_menu_fcuntion_btn(void *flowState, int startWidgetIndex);

void create_user_widget_header_btn_group(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_btn_group(void *flowState, int startWidgetIndex);

void create_user_widget_header_clock(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_clock(void *flowState, int startWidgetIndex);

void create_user_widget_header_logo(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_logo(void *flowState, int startWidgetIndex);

void create_user_widget_header_btn(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_btn(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/