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
    lv_obj_t *obj2__icon_image;
    lv_obj_t *obj2__obj1;
    lv_obj_t *obj3;
    lv_obj_t *obj3__obj0;
    lv_obj_t *obj3__icon_image;
    lv_obj_t *obj3__obj1;
    lv_obj_t *obj4;
    lv_obj_t *obj4__obj0;
    lv_obj_t *obj4__icon_image;
    lv_obj_t *obj4__obj1;
    lv_obj_t *obj5;
    lv_obj_t *obj5__obj0;
    lv_obj_t *obj5__icon_image;
    lv_obj_t *obj5__obj1;
    lv_obj_t *home_content;
    lv_obj_t *home_content__home_content;
    lv_obj_t *home_content__obj3;
    lv_obj_t *home_content__obj4;
    lv_obj_t *message_content;
    lv_obj_t *message_content__obj5;
    lv_obj_t *message_content__obj5__obj0;
    lv_obj_t *message_content__obj5__obj1;
    lv_obj_t *message_content__obj6;
    lv_obj_t *message_content__obj6__obj0;
    lv_obj_t *message_content__obj6__obj1;
    lv_obj_t *message_content__message_content_list;
    lv_obj_t *message_content__obj7;
    lv_obj_t *obj6;
    lv_obj_t *obj6__obj8;
    lv_obj_t *obj6__obj8__obj0;
    lv_obj_t *obj6__obj8__obj1;
    lv_obj_t *obj6__obj9;
    lv_obj_t *obj6__obj9__obj0;
    lv_obj_t *obj6__obj9__obj1;
    lv_obj_t *obj6__obj10;
    lv_obj_t *obj6__obj10__obj0;
    lv_obj_t *obj6__obj10__obj1;
    lv_obj_t *obj6__obj11;
    lv_obj_t *obj6__obj11__obj0;
    lv_obj_t *obj6__obj11__obj1;
    lv_obj_t *obj6__obj12;
    lv_obj_t *obj6__obj12__obj0;
    lv_obj_t *obj6__obj12__obj1;
    lv_obj_t *obj6__obj13;
    lv_obj_t *obj6__obj13__obj0;
    lv_obj_t *obj6__obj13__obj1;
    lv_obj_t *obj6__obj14;
    lv_obj_t *obj6__obj14__obj0;
    lv_obj_t *obj6__obj14__obj1;
    lv_obj_t *obj6__obj15;
    lv_obj_t *obj6__obj15__obj0;
    lv_obj_t *obj6__obj15__obj1;
    lv_obj_t *obj6__obj16;
    lv_obj_t *obj6__obj16__obj0;
    lv_obj_t *obj6__obj16__obj1;
    lv_obj_t *obj6__obj17;
    lv_obj_t *obj6__obj17__obj0;
    lv_obj_t *obj6__obj17__obj1;
    lv_obj_t *obj6__obj18;
    lv_obj_t *obj6__obj18__obj0;
    lv_obj_t *obj6__obj18__obj1;
    lv_obj_t *obj6__obj19;
    lv_obj_t *obj6__obj19__obj0;
    lv_obj_t *obj6__obj19__obj1;
    lv_obj_t *obj6__obj20;
    lv_obj_t *obj6__obj20__obj0;
    lv_obj_t *obj6__obj20__obj1;
    lv_obj_t *obj6__obj21;
    lv_obj_t *obj6__obj21__obj0;
    lv_obj_t *obj6__obj21__obj1;
    lv_obj_t *obj6__obj22;
    lv_obj_t *obj6__obj22__obj0;
    lv_obj_t *obj6__obj22__obj1;
    lv_obj_t *obj6__device_shortcut_content;
    lv_obj_t *obj7;
    lv_obj_t *obj7__obj23;
    lv_obj_t *obj7__obj23__basic_setting_container_1;
    lv_obj_t *obj7__obj23__obj0;
    lv_obj_t *obj7__obj23__obj1;
    lv_obj_t *obj7__obj23__obj2;
    lv_obj_t *obj7__obj23__obj3;
    lv_obj_t *obj7__obj23__obj4;
    lv_obj_t *obj7__obj23__obj5;
    lv_obj_t *obj7__obj23__obj6;
    lv_obj_t *obj7__obj23__obj7;
    lv_obj_t *obj7__obj23__obj8;
    lv_obj_t *obj7__obj24;
    lv_obj_t *obj7__obj24__obj0;
    lv_obj_t *obj7__obj24__obj0__setting_content_network_setting_wi_fi_obj;
    lv_obj_t *obj7__obj24__obj0__obj0;
    lv_obj_t *obj7__obj24__obj0__obj1;
    lv_obj_t *obj7__obj24__obj0__obj2;
    lv_obj_t *obj7__obj24__obj0__obj3;
    lv_obj_t *obj7__obj24__obj0__obj4;
    lv_obj_t *obj7__obj24__obj0__obj5;
    lv_obj_t *obj7__obj24__obj0__obj6;
    lv_obj_t *obj7__obj24__obj0__obj7;
    lv_obj_t *obj7__obj24__obj0__obj8;
    lv_obj_t *obj7__obj24__obj0__obj9;
    lv_obj_t *obj7__obj24__obj0__obj10;
    lv_obj_t *obj7__obj24__obj0__obj11;
    lv_obj_t *obj7__obj24__obj0__obj12;
    lv_obj_t *obj7__obj24__obj0__obj13;
    lv_obj_t *obj7__obj24__obj0__obj14;
    lv_obj_t *obj7__obj24__obj0__obj15;
    lv_obj_t *obj7__obj24__obj0__obj16;
    lv_obj_t *obj7__obj24__obj0__obj17;
    lv_obj_t *obj7__obj24__obj0__obj18;
    lv_obj_t *obj7__obj24__obj0__obj19;
    lv_obj_t *obj7__obj24__obj0__obj20;
    lv_obj_t *obj7__obj24__obj0__obj21;
    lv_obj_t *obj7__obj24__obj0__obj22;
    lv_obj_t *obj7__obj24__obj0__obj23;
    lv_obj_t *obj7__obj24__obj0__obj24;
    lv_obj_t *obj7__obj24__obj0__obj25;
    lv_obj_t *obj7__obj24__obj0__obj26;
    lv_obj_t *obj7__obj24__obj0__obj27;
    lv_obj_t *obj7__obj24__obj0__obj28;
    lv_obj_t *obj7__obj24__obj0__obj29;
    lv_obj_t *obj7__obj24__obj0__obj30;
    lv_obj_t *obj7__obj24__obj0__obj31;
    lv_obj_t *obj7__obj24__obj0__obj32;
    lv_obj_t *obj7__obj24__obj0__obj33;
    lv_obj_t *obj7__obj24__obj0__obj34;
    lv_obj_t *obj7__obj24__obj0__obj35;
    lv_obj_t *obj7__obj24__obj0__obj36;
    lv_obj_t *obj7__obj24__obj0__obj37;
    lv_obj_t *obj7__obj24__obj1;
    lv_obj_t *obj7__obj24__obj1__obj0;
    lv_obj_t *obj7__obj24__obj1__obj0__obj0;
    lv_obj_t *obj7__obj24__obj1__obj0__obj1;
    lv_obj_t *obj7__obj24__obj1__obj0__obj2;
    lv_obj_t *obj7__obj24__obj1__obj1;
    lv_obj_t *obj7__obj24__obj1__obj1__obj0;
    lv_obj_t *obj7__obj24__obj1__obj1__obj1;
    lv_obj_t *obj7__obj24__obj1__obj1__obj2;
    lv_obj_t *obj7__obj24__obj1__obj2;
    lv_obj_t *obj7__obj24__obj1__obj2__obj0;
    lv_obj_t *obj7__obj24__obj1__obj2__obj1;
    lv_obj_t *obj7__obj24__obj1__obj2__obj2;
    lv_obj_t *obj7__obj24__obj1__obj3;
    lv_obj_t *obj7__obj24__obj1__obj3__obj0;
    lv_obj_t *obj7__obj24__obj1__obj3__obj1;
    lv_obj_t *obj7__obj24__obj1__obj3__obj2;
    lv_obj_t *obj7__obj24__obj1__obj4;
    lv_obj_t *obj7__obj24__obj1__obj4__obj0;
    lv_obj_t *obj7__obj24__obj1__obj4__obj1;
    lv_obj_t *obj7__obj24__obj1__obj4__obj2;
    lv_obj_t *obj7__obj24__obj1__obj5;
    lv_obj_t *obj7__obj24__obj1__obj5__obj0;
    lv_obj_t *obj7__obj24__obj1__obj5__obj1;
    lv_obj_t *obj7__obj24__obj1__obj5__obj2;
    lv_obj_t *obj7__obj24__obj1__obj6;
    lv_obj_t *obj7__obj24__obj1__obj6__obj0;
    lv_obj_t *obj7__obj24__obj1__obj6__obj1;
    lv_obj_t *obj7__obj24__obj1__obj6__obj2;
    lv_obj_t *obj7__obj24__obj1__obj7;
    lv_obj_t *obj7__obj24__obj1__obj7__obj0;
    lv_obj_t *obj7__obj24__obj1__obj7__obj1;
    lv_obj_t *obj7__obj24__obj1__obj7__obj2;
    lv_obj_t *obj7__obj24__obj1__obj8;
    lv_obj_t *obj7__obj24__obj1__obj8__obj0;
    lv_obj_t *obj7__obj24__obj1__obj8__obj1;
    lv_obj_t *obj7__obj24__obj1__obj8__obj2;
    lv_obj_t *obj7__obj24__obj1__obj9;
    lv_obj_t *obj7__obj24__obj1__obj9__obj0;
    lv_obj_t *obj7__obj24__obj1__obj9__obj1;
    lv_obj_t *obj7__obj24__obj1__obj9__obj2;
    lv_obj_t *obj7__obj24__obj1__obj10;
    lv_obj_t *obj7__obj24__obj1__obj11;
    lv_obj_t *obj7__obj24__obj2;
    lv_obj_t *obj7__obj24__obj3;
    lv_obj_t *obj7__obj24__obj4;
    lv_obj_t *obj7__obj24__obj5;
    lv_obj_t *obj7__obj25;
    lv_obj_t *obj7__obj26;
    lv_obj_t *obj7__setting_sidebar_container;
    lv_obj_t *obj7__obj27;
    lv_obj_t *obj7__obj28;
    lv_obj_t *obj7__obj29;
    lv_obj_t *obj7__obj30;
    lv_obj_t *obj7__obj31;
    lv_obj_t *obj7__setting_body_container;
    lv_obj_t *obj7__obj32;
    lv_obj_t *obj8;
    lv_obj_t *obj8__back_container_trigger;
    lv_obj_t *obj8__obj35;
    lv_obj_t *obj8__obj36;
    lv_obj_t *obj8__obj37;
    lv_obj_t *obj8__obj38;
    lv_obj_t *header_container;
    lv_obj_t *header_logo_container;
    lv_obj_t *header_clock_container;
    lv_obj_t *header_btn_container;
    lv_obj_t *body_container;
    lv_obj_t *sidebar_container;
    lv_obj_t *content_container;
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

void create_user_widget_device_shortcut_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_shortcut_content(void *flowState, int startWidgetIndex);

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

void create_user_widget_device_setting_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_setting_content(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_basic_setting_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_basic_setting_widget(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_data_setting_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_data_setting_widget(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_display_setting_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_display_setting_widget(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_network_setting_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_network_setting_widget(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_network_setting_wi_fi_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_network_setting_wi_fi_widget(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content_network_setting_bluetooth_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content_network_setting_bluetooth_widget(void *flowState, int startWidgetIndex);

void create_user_widget_bluetooth_record_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_bluetooth_record_widget(void *flowState, int startWidgetIndex);

void create_user_widget_wi_fi_record_widget(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_wi_fi_record_widget(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/