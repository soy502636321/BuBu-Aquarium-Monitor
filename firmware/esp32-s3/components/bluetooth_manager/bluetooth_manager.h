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

} bluetooth_status_t;


typedef struct
{
    bluetooth_status_t status;
    uint16_t conn_id;
    char device_name[32];
} bluetooth_info_t;



esp_err_t bluetooth_manager_init(
        const char *device_name
);


void bluetooth_manager_start(void);


void bluetooth_manager_stop(void);


bluetooth_status_t bluetooth_manager_get_status(void);


bluetooth_info_t bluetooth_manager_get_info(void);



#ifdef __cplusplus
}
#endif