#ifndef EEZ_LVGL_UI_EXTENSION_GUI_H
#define EEZ_LVGL_UI_EXTENSION_GUI_H

#include "lvgl.h"

#include "eez-flow.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_extension_init();
void ui_extension_tick();

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_EXTENSION_GUI_H