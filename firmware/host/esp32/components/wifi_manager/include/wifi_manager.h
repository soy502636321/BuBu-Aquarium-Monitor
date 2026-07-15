#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "vars.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    WiFi_Status status;

    char ssid[32];
    char ip[16];

    int8_t rssi;

} wifi_info_t;



esp_err_t wifi_manager_init(void);


esp_err_t wifi_manager_scan(void);

esp_err_t wifi_manager_connect(
        const char *ssid,
        const char *password
);


void wifi_manager_disconnect(void);


wifi_info_t wifi_manager_get_info(void);


WiFi_Status wifi_manager_get_status(void);


#ifdef __cplusplus
}
#endif