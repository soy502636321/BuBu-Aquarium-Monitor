#include "bluetooth_manager.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"


static const char *TAG="ble_manager";


static ble_info_t ble_info;



static int ble_gap_event(
        struct ble_gap_event *event,
        void *arg)
{

    switch(event->type)
    {

    case BLE_GAP_EVENT_CONNECT:

        if(event->connect.status == 0)
        {
            ble_info.status =
                BLE_STATUS_CONNECTED;

            ble_info.conn_id =
                event->connect.conn_handle;


            ESP_LOGI(
                TAG,
                "BLE connected"
            );
        }
        else
        {
            ble_info.status =
                BLE_STATUS_ADVERTISING;
        }

        break;



    case BLE_GAP_EVENT_DISCONNECT:

        ble_info.status =
            BLE_STATUS_DISCONNECTED;


        ble_manager_start();

        break;



    default:
        break;

    }


    return 0;
}




void ble_advertise(void)
{

    struct ble_gap_adv_params adv_params={};


    ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        ble_gap_event,
        NULL
    );


    ble_info.status =
        BLE_STATUS_ADVERTISING;

}




static void ble_on_sync(void)
{
    ble_advertise();
}





esp_err_t ble_manager_init(
        const char *device_name)
{

    memset(
        &ble_info,
        0,
        sizeof(ble_info)
    );


    strcpy(
        ble_info.device_name,
        device_name
    );


    ble_info.status =
        BLE_STATUS_INIT;



    nimble_port_init();


    ble_svc_gap_init();


    ble_svc_gap_device_name_set(
        device_name
    );


    ble_hs_cfg.sync_cb =
        ble_on_sync;



    nimble_port_freertos_init(
        [](void *param)
        {
            nimble_port_run();
        }
    );


    return ESP_OK;
}



void ble_manager_start(void)
{
    ble_advertise();
}



void ble_manager_stop(void)
{
    ble_gap_adv_stop();

    ble_info.status =
        BLE_STATUS_IDLE;
}



ble_status_t ble_manager_get_status(void)
{
    return ble_info.status;
}



ble_info_t ble_manager_get_info(void)
{
    return ble_info;
}