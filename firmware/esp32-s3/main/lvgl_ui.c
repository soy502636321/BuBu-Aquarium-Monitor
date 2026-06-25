#include "lvgl.h"
#include "lvgl_ui.h"

void ui_init(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "BuBu Aquarium Monitor");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}