#include <string>
#include "vars.h"

int32_t content_page;
bool loading;


WiFi_Status wifi_status; // WiFi 状态图标显示
bool wifi_enabled; // WiFi 扫描状态

std::string launch_message;

int32_t launch_progress;

extern "C" int32_t get_var_launch_progress() {
    return launch_progress;
}

extern "C" void set_var_launch_progress(int32_t value) {
    launch_progress = value;
}

extern "C" const char *get_var_launch_message() {
    return launch_message.c_str();
}

extern "C" void set_var_launch_message(const char *value) {
    launch_message = value;
}

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

extern "C" bool get_var_loading() {
    return loading;
}

extern "C" void set_var_loading(bool value) {
    loading = value;
}