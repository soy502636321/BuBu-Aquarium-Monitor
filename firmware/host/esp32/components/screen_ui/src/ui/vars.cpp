#include "vars.h"

int32_t content_page;


WiFi_Status wifi_status; // WiFi 状态图标显示
bool wifi_enabled; // WiFi 扫描状态

extern "C" int32_t get_var_content_page() {
    return content_page;
}

extern "C" void set_var_content_page(int32_t value) {
    content_page = value;
}


extern "C" WiFi_Status get_var_wifi_status() {
    return wifi_status;
}

extern "C" void set_var_wifi_status(WiFi_Status value) {
    wifi_status = value;
}

extern "C" bool get_var_wifi_enabled() {
    return wifi_enabled;
}

extern "C" void set_var_wifi_enabled(bool value) {
    wifi_enabled = value;
}