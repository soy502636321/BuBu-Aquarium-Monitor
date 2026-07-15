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
    lv_obj_t *obj0__home_menu_btn_1;
    lv_obj_t *obj0__obj62;
    lv_obj_t *obj0__obj63;
    lv_obj_t *obj0__obj63__obj2;
    lv_obj_t *obj0__obj63__obj3;
    lv_obj_t *obj0__obj63__obj4;
    lv_obj_t *obj0__obj63__obj5;
    lv_obj_t *obj0__obj64;
    lv_obj_t *obj0__header_clock_container_1;
    lv_obj_t *obj1;
    lv_obj_t *obj1__obj0;
    lv_obj_t *obj1__icon_image;
    lv_obj_t *obj1__obj1;
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
    lv_obj_t *home_content;
    lv_obj_t *home_content__obj7;
    lv_obj_t *home_content__obj7__obj0;
    lv_obj_t *home_content__obj8;
    lv_obj_t *home_content__obj8__obj0;
    lv_obj_t *home_content__obj9;
    lv_obj_t *home_content__obj9__obj0;
    lv_obj_t *home_content__obj10;
    lv_obj_t *home_content__obj10__obj0;
    lv_obj_t *home_content__obj11;
    lv_obj_t *home_content__obj11__obj0;
    lv_obj_t *home_content__obj12;
    lv_obj_t *home_content__obj12__obj0;
    lv_obj_t *home_content__obj13;
    lv_obj_t *home_content__obj13__obj0;
    lv_obj_t *home_content__obj14;
    lv_obj_t *home_content__obj14__obj0;
    lv_obj_t *home_content__obj15;
    lv_obj_t *home_content__obj15__obj0;
    lv_obj_t *home_content__obj16;
    lv_obj_t *home_content__obj16__obj0;
    lv_obj_t *home_content__obj17;
    lv_obj_t *home_content__obj17__obj0;
    lv_obj_t *home_content__obj18;
    lv_obj_t *home_content__obj18__obj0;
    lv_obj_t *home_content__obj19;
    lv_obj_t *home_content__obj19__obj0;
    lv_obj_t *home_content__home_content;
    lv_obj_t *message_content;
    lv_obj_t *message_content__obj20;
    lv_obj_t *message_content__obj20__obj0;
    lv_obj_t *message_content__obj20__obj1;
    lv_obj_t *message_content__obj21;
    lv_obj_t *message_content__obj21__obj0;
    lv_obj_t *message_content__obj21__obj1;
    lv_obj_t *message_content__obj22;
    lv_obj_t *message_content__obj22__obj0;
    lv_obj_t *message_content__obj22__obj1;
    lv_obj_t *message_content__obj23;
    lv_obj_t *message_content__obj23__obj0;
    lv_obj_t *message_content__obj23__obj1;
    lv_obj_t *message_content__obj24;
    lv_obj_t *message_content__obj24__obj0;
    lv_obj_t *message_content__obj24__obj1;
    lv_obj_t *message_content__obj25;
    lv_obj_t *message_content__obj25__obj0;
    lv_obj_t *message_content__obj25__obj1;
    lv_obj_t *message_content__obj26;
    lv_obj_t *message_content__obj26__obj0;
    lv_obj_t *message_content__obj26__obj1;
    lv_obj_t *message_content__obj27;
    lv_obj_t *message_content__obj27__obj0;
    lv_obj_t *message_content__obj27__obj1;
    lv_obj_t *message_content__obj28;
    lv_obj_t *message_content__obj28__obj0;
    lv_obj_t *message_content__obj28__obj1;
    lv_obj_t *message_content__obj29;
    lv_obj_t *message_content__obj29__obj0;
    lv_obj_t *message_content__obj29__obj1;
    lv_obj_t *message_content__obj30;
    lv_obj_t *obj5;
    lv_obj_t *obj5__obj31;
    lv_obj_t *obj5__obj31__obj0;
    lv_obj_t *obj5__obj31__obj1;
    lv_obj_t *obj5__obj32;
    lv_obj_t *obj5__obj32__obj0;
    lv_obj_t *obj5__obj32__obj1;
    lv_obj_t *obj5__obj33;
    lv_obj_t *obj5__obj33__obj0;
    lv_obj_t *obj5__obj33__obj1;
    lv_obj_t *obj5__obj34;
    lv_obj_t *obj5__obj34__obj0;
    lv_obj_t *obj5__obj34__obj1;
    lv_obj_t *obj5__obj35;
    lv_obj_t *obj5__obj35__obj0;
    lv_obj_t *obj5__obj35__obj1;
    lv_obj_t *obj5__obj36;
    lv_obj_t *obj5__obj36__obj0;
    lv_obj_t *obj5__obj36__obj1;
    lv_obj_t *obj5__obj37;
    lv_obj_t *obj5__obj37__obj0;
    lv_obj_t *obj5__obj37__obj1;
    lv_obj_t *obj5__obj38;
    lv_obj_t *obj5__obj38__obj0;
    lv_obj_t *obj5__obj38__obj1;
    lv_obj_t *obj5__obj39;
    lv_obj_t *obj5__obj39__obj0;
    lv_obj_t *obj5__obj39__obj1;
    lv_obj_t *obj5__obj40;
    lv_obj_t *obj5__obj40__obj0;
    lv_obj_t *obj5__obj40__obj1;
    lv_obj_t *obj5__obj41;
    lv_obj_t *obj5__obj41__obj0;
    lv_obj_t *obj5__obj41__obj1;
    lv_obj_t *obj5__obj42;
    lv_obj_t *obj5__obj42__obj0;
    lv_obj_t *obj5__obj42__obj1;
    lv_obj_t *obj5__obj43;
    lv_obj_t *obj5__obj43__obj0;
    lv_obj_t *obj5__obj43__obj1;
    lv_obj_t *obj5__obj44;
    lv_obj_t *obj5__obj44__obj0;
    lv_obj_t *obj5__obj44__obj1;
    lv_obj_t *obj5__obj45;
    lv_obj_t *obj5__obj45__obj0;
    lv_obj_t *obj5__obj45__obj1;
    lv_obj_t *obj5__device_shortcut_content;
    lv_obj_t *obj6;
    lv_obj_t *obj6__obj46;
    lv_obj_t *obj6__obj46__basic_setting_container_1;
    lv_obj_t *obj6__obj46__obj0;
    lv_obj_t *obj6__obj46__obj1;
    lv_obj_t *obj6__obj46__obj2;
    lv_obj_t *obj6__obj46__obj3;
    lv_obj_t *obj6__obj46__obj4;
    lv_obj_t *obj6__obj46__obj5;
    lv_obj_t *obj6__obj46__obj6;
    lv_obj_t *obj6__obj46__obj7;
    lv_obj_t *obj6__obj46__obj8;
    lv_obj_t *obj6__obj47;
    lv_obj_t *obj6__obj47__obj0;
    lv_obj_t *obj6__obj47__obj0__setting_content_network_setting_wi_fi_obj;
    lv_obj_t *obj6__obj47__obj0__obj0;
    lv_obj_t *obj6__obj47__obj0__obj1;
    lv_obj_t *obj6__obj47__obj0__obj2;
    lv_obj_t *obj6__obj47__obj0__obj3;
    lv_obj_t *obj6__obj47__obj0__obj4;
    lv_obj_t *obj6__obj47__obj0__obj5;
    lv_obj_t *obj6__obj47__obj0__obj6;
    lv_obj_t *obj6__obj47__obj0__obj7;
    lv_obj_t *obj6__obj47__obj0__obj8;
    lv_obj_t *obj6__obj47__obj0__obj9;
    lv_obj_t *obj6__obj47__obj0__obj10;
    lv_obj_t *obj6__obj47__obj0__obj11;
    lv_obj_t *obj6__obj47__obj0__obj12;
    lv_obj_t *obj6__obj47__obj0__obj13;
    lv_obj_t *obj6__obj47__obj0__obj14;
    lv_obj_t *obj6__obj47__obj0__obj15;
    lv_obj_t *obj6__obj47__obj0__obj16;
    lv_obj_t *obj6__obj47__obj0__obj17;
    lv_obj_t *obj6__obj47__obj0__obj18;
    lv_obj_t *obj6__obj47__obj0__obj19;
    lv_obj_t *obj6__obj47__obj0__obj20;
    lv_obj_t *obj6__obj47__obj0__obj21;
    lv_obj_t *obj6__obj47__obj0__obj22;
    lv_obj_t *obj6__obj47__obj0__obj23;
    lv_obj_t *obj6__obj47__obj0__obj24;
    lv_obj_t *obj6__obj47__obj0__obj25;
    lv_obj_t *obj6__obj47__obj0__obj26;
    lv_obj_t *obj6__obj47__obj0__obj27;
    lv_obj_t *obj6__obj47__obj0__obj28;
    lv_obj_t *obj6__obj47__obj0__obj29;
    lv_obj_t *obj6__obj47__obj0__obj30;
    lv_obj_t *obj6__obj47__obj0__obj31;
    lv_obj_t *obj6__obj47__obj0__obj32;
    lv_obj_t *obj6__obj47__obj0__obj33;
    lv_obj_t *obj6__obj47__obj0__obj34;
    lv_obj_t *obj6__obj47__obj0__obj35;
    lv_obj_t *obj6__obj47__obj0__obj36;
    lv_obj_t *obj6__obj47__obj0__obj37;
    lv_obj_t *obj6__obj47__obj1;
    lv_obj_t *obj6__obj47__obj1__obj0;
    lv_obj_t *obj6__obj47__obj1__obj0__obj0;
    lv_obj_t *obj6__obj47__obj1__obj0__obj1;
    lv_obj_t *obj6__obj47__obj1__obj0__obj2;
    lv_obj_t *obj6__obj47__obj1__obj1;
    lv_obj_t *obj6__obj47__obj1__obj1__obj0;
    lv_obj_t *obj6__obj47__obj1__obj1__obj1;
    lv_obj_t *obj6__obj47__obj1__obj1__obj2;
    lv_obj_t *obj6__obj47__obj1__obj2;
    lv_obj_t *obj6__obj47__obj1__obj2__obj0;
    lv_obj_t *obj6__obj47__obj1__obj2__obj1;
    lv_obj_t *obj6__obj47__obj1__obj2__obj2;
    lv_obj_t *obj6__obj47__obj1__obj3;
    lv_obj_t *obj6__obj47__obj1__obj3__obj0;
    lv_obj_t *obj6__obj47__obj1__obj3__obj1;
    lv_obj_t *obj6__obj47__obj1__obj3__obj2;
    lv_obj_t *obj6__obj47__obj1__obj4;
    lv_obj_t *obj6__obj47__obj1__obj4__obj0;
    lv_obj_t *obj6__obj47__obj1__obj4__obj1;
    lv_obj_t *obj6__obj47__obj1__obj4__obj2;
    lv_obj_t *obj6__obj47__obj1__obj5;
    lv_obj_t *obj6__obj47__obj1__obj5__obj0;
    lv_obj_t *obj6__obj47__obj1__obj5__obj1;
    lv_obj_t *obj6__obj47__obj1__obj5__obj2;
    lv_obj_t *obj6__obj47__obj1__obj6;
    lv_obj_t *obj6__obj47__obj1__obj6__obj0;
    lv_obj_t *obj6__obj47__obj1__obj6__obj1;
    lv_obj_t *obj6__obj47__obj1__obj6__obj2;
    lv_obj_t *obj6__obj47__obj1__obj7;
    lv_obj_t *obj6__obj47__obj1__obj7__obj0;
    lv_obj_t *obj6__obj47__obj1__obj7__obj1;
    lv_obj_t *obj6__obj47__obj1__obj7__obj2;
    lv_obj_t *obj6__obj47__obj1__obj8;
    lv_obj_t *obj6__obj47__obj1__obj8__obj0;
    lv_obj_t *obj6__obj47__obj1__obj8__obj1;
    lv_obj_t *obj6__obj47__obj1__obj8__obj2;
    lv_obj_t *obj6__obj47__obj1__obj9;
    lv_obj_t *obj6__obj47__obj1__obj9__obj0;
    lv_obj_t *obj6__obj47__obj1__obj9__obj1;
    lv_obj_t *obj6__obj47__obj1__obj9__obj2;
    lv_obj_t *obj6__obj47__obj1__obj10;
    lv_obj_t *obj6__obj47__obj1__obj11;
    lv_obj_t *obj6__obj47__obj2;
    lv_obj_t *obj6__obj47__obj3;
    lv_obj_t *obj6__obj47__obj4;
    lv_obj_t *obj6__obj47__obj5;
    lv_obj_t *obj6__obj48;
    lv_obj_t *obj6__obj49;
    lv_obj_t *obj6__setting_sidebar_container;
    lv_obj_t *obj6__obj50;
    lv_obj_t *obj6__obj51;
    lv_obj_t *obj6__obj52;
    lv_obj_t *obj6__obj53;
    lv_obj_t *obj6__obj54;
    lv_obj_t *obj6__setting_body_container;
    lv_obj_t *obj6__obj55;
    lv_obj_t *obj7;
    lv_obj_t *obj7__back_container_trigger;
    lv_obj_t *obj7__obj56;
    lv_obj_t *obj7__obj57;
    lv_obj_t *obj7__obj58;
    lv_obj_t *obj7__obj59;
    lv_obj_t *header_container;
    lv_obj_t *body_container;
    lv_obj_t *sidebar_container;
    lv_obj_t *content_container;
} objects_t;

extern objects_t objects;

void create_screen_main_screen();
void tick_screen_main_screen();

void create_user_widget_menu_fcuntion_btn(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_menu_fcuntion_btn(void *flowState, int startWidgetIndex);

void create_user_widget_header_btn_groups(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_btn_groups(void *flowState, int startWidgetIndex);

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

void create_user_widget_message_card(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_message_card(void *flowState, int startWidgetIndex);

void create_user_widget_device_shortcut_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_shortcut_content(void *flowState, int startWidgetIndex);

void create_user_widget_device_shortcut(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_device_shortcut(void *flowState, int startWidgetIndex);

void create_user_widget_setting_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_setting_content(void *flowState, int startWidgetIndex);

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

void create_user_widget_header_content(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_header_content(void *flowState, int startWidgetIndex);

void create_user_widget_pwm_card(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_pwm_card(void *flowState, int startWidgetIndex);

void create_user_widget_switch_card(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_switch_card(void *flowState, int startWidgetIndex);

void create_user_widget_data_card(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_data_card(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/