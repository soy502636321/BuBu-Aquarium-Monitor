#include <vector>
#include <string>

#include "bluetooth_manager.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "services/gap/ble_svc_gap.h"

static const char *TAG="BuBu-Aquarium-Monitor[bluetooth_manager]";

struct BLE_Device
{
    char name[32];
    ble_addr_t addr;
    int rssi;
};

static bluetooth_info_t bluetooth_info;

std::vector<BLE_Device> ble_devices;

static uint8_t own_addr_type;

static int ble_event_callback(struct ble_gap_event *event, void *arg) {
    switch(event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        if(event->connect.status == 0)
        {
            bluetooth_info.status =
                BLE_STATUS_CONNECTED;
            bluetooth_info.conn_id =
                event->connect.conn_handle;


            ESP_LOGI(
                TAG,
                "BLE connected"
            );
        }
        else
        {
            bluetooth_info.status =
                BLE_STATUS_ADVERTISING;
        }

        break;



    case BLE_GAP_EVENT_DISCONNECT:

        bluetooth_info.status =
            BLE_STATUS_DISCONNECTED;


        bluetooth_manager_start();

        break;

	case BLE_GAP_EVENT_DISC:
	{
		struct ble_hs_adv_fields fields;

        int rc = ble_hs_adv_parse_fields(
            &fields,
            event->disc.data,
            event->disc.length_data
        );

        char name[32] = {0};

        if(rc == 0)
        {
            // 完整名称
            if(fields.name != NULL && fields.name_len > 0)
            {
                int len = fields.name_len;

                if(len >= sizeof(name))
                    len = sizeof(name)-1;

                memcpy(
                    name,
                    fields.name,
                    len
                );

                name[len] = '\0';
            }
            // 简短名称
            else if(fields.name != NULL)
            {
                memcpy(
                    name,
                    fields.name,
                    fields.name_len
                );
            }
        }


        ESP_LOGI(TAG,
            "name=%s RSSI=%d",
            name,
            event->disc.rssi
        );
        }
		break;
	case BLE_GAP_EVENT_DISC_COMPLETE:
		ESP_LOGI(TAG, "BLE_GAP_EVENT_DISC_COMPLETE_蓝牙扫描完成，发现%d个设备", sizeof(ble_devices));
		for (auto &dev : ble_devices)
		{
		    ESP_LOGI(TAG, "name=%s rssi=%d", dev.name, dev.rssi);
		}
		break;
    default:
        break;

    }


    return 0;
}

void bluetooth_advertise(void)
{

    struct ble_gap_adv_params adv_params={};

    ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        ble_event_callback,
        NULL
    );


    bluetooth_info.status =
        BLE_STATUS_ADVERTISING;

}




static void bluetooth_on_sync(void)
{
    bluetooth_advertise();
}

esp_err_t bluetooth_manager_init(const char *device_name) {
	ESP_LOGI(TAG, "蓝牙功能初始化");
    memset(&bluetooth_info, 0, sizeof(bluetooth_info));
    strcpy(bluetooth_info.device_name, device_name);

    bluetooth_info.status = BLE_STATUS_INIT;

    nimble_port_init();

    ble_svc_gap_init();

    ble_svc_gap_device_name_set(
        device_name
    );

    ble_hs_cfg.sync_cb =
        bluetooth_on_sync;

    nimble_port_freertos_init(
        [](void *param)
        {
            nimble_port_run();
        }
    );

    return ESP_OK;
}



void bluetooth_manager_start(void)
{
    // bluetooth_advertise();
    
   struct ble_gap_disc_params params={
    .itvl = 0x0010,
    .window = 0x0010,
    .filter_policy = 0,
    .limited = 0,
    .passive = 0,   // 关键
    .filter_duplicates = 1,
   };

    int rc =
        ble_gap_disc(
            own_addr_type,
            5000,
            &params,
            ble_event_callback,
            NULL
        );


    if(rc != 0)
    {
        ESP_LOGE(TAG,
            "scan failed:%d",
            rc);
    }
}



void bluetooth_manager_stop(void)
{
    ble_gap_adv_stop();

    bluetooth_info.status =
        BLE_STATUS_IDLE;
}



bluetooth_status_t bluetooth_manager_get_status(void)
{
    return bluetooth_info.status;
}



bluetooth_info_t bluetooth_manager_get_info(void)
{
    return bluetooth_info;
}