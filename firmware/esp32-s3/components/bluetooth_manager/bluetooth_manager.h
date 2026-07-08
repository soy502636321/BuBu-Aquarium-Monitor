#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    BLE_STATUS_IDLE = 0,
    BLE_STATUS_INIT,
    BLE_STATUS_ADVERTISING,
    BLE_STATUS_CONNECTED,
    BLE_STATUS_DISCONNECTED,
    BLE_STATUS_ERROR

} ble_status_t;


typedef struct
{
    ble_status_t status;

    uint16_t conn_id;

    char device_name[32];

} ble_info_t;



esp_err_t ble_manager_init(
        const char *device_name
);


void ble_manager_start(void);


void ble_manager_stop(void);


ble_status_t ble_manager_get_status(void);


ble_info_t ble_manager_get_info(void);



#ifdef __cplusplus
}
#endif