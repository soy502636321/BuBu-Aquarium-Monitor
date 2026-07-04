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
    lv_obj_t *home_menu_btn;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj1__obj2;
    lv_obj_t *obj1__obj2__obj0;
    lv_obj_t *obj2;
    lv_obj_t *obj2__obj0;
    lv_obj_t *obj2__obj1;
    lv_obj_t *obj3;
    lv_obj_t *obj3__obj0;
    lv_obj_t *obj3__obj1;
    lv_obj_t *obj4;
    lv_obj_t *obj4__obj0;
    lv_obj_t *obj4__obj1;
    lv_obj_t *obj5;
    lv_obj_t *obj5__obj0;
    lv_obj_t *obj5__obj1;
    lv_obj_t *obj6;
    lv_obj_t *obj6__home_content;
    lv_obj_t *obj6__obj3;
    lv_obj_t *obj6__obj4;
    lv_obj_t *obj7;
    lv_obj_t *obj7__obj8;
    lv_obj_t *obj7__obj8__obj0;
    lv_obj_t *obj7__obj8__obj1;
    lv_obj_t *obj7__obj9;
    lv_obj_t *obj7__obj9__obj0;
    lv_obj_t *obj7__obj9__obj1;
    lv_obj_t *obj7__obj10;
    lv_obj_t *obj7__obj10__obj0;
    lv_obj_t *obj7__obj10__obj1;
    lv_obj_t *obj7__obj11;
    lv_obj_t *obj7__obj11__obj0;
    lv_obj_t *obj7__obj11__obj1;
    lv_obj_t *obj7__obj12;
    lv_obj_t *obj7__obj12__obj0;
    lv_obj_t *obj7__obj12__obj1;
    lv_obj_t *obj7__obj13;
    lv_obj_t *obj7__obj13__obj0;
    lv_obj_t *obj7__obj13__obj1;
    lv_obj_t *obj7__obj14;
    lv_obj_t *obj7__obj14__obj0;
    lv_obj_t *obj7__obj14__obj1;
    lv_obj_t *obj7__obj15;
    lv_obj_t *obj7__obj15__obj0;
    lv_obj_t *obj7__obj15__obj1;
    lv_obj_t *obj7__obj16;
    lv_obj_t *obj7__obj16__obj0;
    lv_obj_t *obj7__obj16__obj1;
    lv_obj_t *obj7__obj17;
    lv_obj_t *obj7__obj17__obj0;
    lv_obj_t *obj7__obj17__obj1;
    lv_obj_t *obj7__obj18;
    lv_obj_t *obj7__obj18__obj0;
    lv_obj_t *obj7__obj18__obj1;
    lv_obj_t *obj7__obj19;
    lv_obj_t *obj7__obj19__obj0;
    lv_obj_t *obj7__obj19__obj1;
    lv_obj_t *obj7__obj20;
    lv_obj_t *obj7__obj20__obj0;
    lv_obj_t *obj7__obj20__obj1;
    lv_obj_t *obj7__obj21;
    lv_obj_t *obj7__obj21__obj0;
    lv_obj_t *obj7__obj21__obj1;
    lv_obj_t *obj7__obj22;
    lv_obj_t *obj7__obj22__obj0;
    lv_obj_t *obj7__obj22__obj1;
    lv_obj_t *obj7__obj23;
    lv_obj_t *obj7__obj23__obj0;
    lv_obj_t *obj7__obj23__obj1;
    lv_obj_t *obj7__obj23__obj2;
    lv_obj_t *obj7__device_content_shortcut;
    lv_obj_t *obj7__device_content_setting;
    lv_obj_t *obj8;
    lv_obj_t *obj8__obj5;
    lv_obj_t *obj8__obj5__obj0;
    lv_obj_t *obj8__obj5__obj1;
    lv_obj_t *obj8__obj6;
    lv_obj_t *obj8__obj6__obj0;
    lv_obj_t *obj8__obj6__obj1;
    lv_obj_t *obj8__message_content_list;
    lv_obj_t *obj8__obj7;
    lv_obj_t *obj9;
    lv_obj_t *obj9__obj24;
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

void create_user_widget_home_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_home_content(void *flowState, int startWidgetIndex);

void create_user_widget_message_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_message_content(void *flowState, int startWidgetIndex);

void create_user_widget_device_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_content(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content(void *flowState, int startWidgetIndex);

void create_user_widget_device_item(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_item(void *flowState, int startWidgetIndex);

void create_user_widget_message_item(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_message_item(void *flowState, int startWidgetIndex);

void create_user_widget_device_card(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_card(void *flowState, int startWidgetIndex);

void create_user_widget_device_detail(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_detail(void *flowState, int startWidgetIndex);

void create_user_widget_device_setting(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_setting(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/