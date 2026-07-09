#include "wifi_manager.h"

#include <string.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "eez-flow.h"
#include "structs.h"
#include "vars.h"    

#define WIFI_CONNECTED_BIT BIT0

static const char *TAG="BuBu-Aquarium-Monitor_wifi_manager.h";

static EventGroupHandle_t wifi_event_group;

static wifi_info_t wifi_info =
{
    .status = WiFi_Status_IDLE,
    .rssi = 0
};

static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

			    static uint16_t number = 10;
			    static wifi_ap_record_t ap_info[10];
static void wifi_event_handler(
        void *arg,
        esp_event_base_t event_base,
        int32_t event_id,
        void *event_data)
{
    if(event_base == WIFI_EVENT) {
        switch(event_id) {
        case WIFI_EVENT_STA_START:
            wifi_info.status = WiFi_Status_CONNECTING;
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            wifi_info.status = WiFi_Status_DISCONNECTED;
            esp_wifi_connect();
            break;
		case WIFI_EVENT_SCAN_DONE: {
				ESP_LOGI(TAG, "WiFi扫描完成");
			    uint16_t ap_count = 0;
			    memset(ap_info, 0, sizeof(ap_info));
			    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
			    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
			    
			    eez::Value value = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_WIFI_RECORD_LIST);
			    eez::ArrayValue *array = value.getArray();
			    for (int i = 0; i < number; i++) {
			        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
			        WiFi_Record_tValue record = array->values[i];
			        record.active(true);
			    	record.ssid((const char *) ap_info[i].ssid);
			    }
				break;
			}
        default:
            break;
        }

    }

    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        sprintf(
            wifi_info.ip,
            IPSTR,
            IP2STR(
              &event->ip_info.ip
            )
        );

        wifi_info.status = WiFi_Status_CONNECTING;

        xEventGroupSetBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGI(TAG, "IP:%s", wifi_info.ip);
    }

}

esp_err_t wifi_manager_init(void)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_LOGI(TAG,
	    "internal free=%d",
	    heap_caps_get_free_size(MALLOC_CAP_INTERNAL)
	);
	
	ESP_LOGI(TAG,
	    "8bit free=%d",
	    heap_caps_get_free_size(MALLOC_CAP_8BIT)
	);
	ESP_LOGI(TAG,
		"internal=%u psram=%u",
		heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
		heap_caps_get_free_size(MALLOC_CAP_SPIRAM)
	);
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );

    wifi_config_t wifi_config={};

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA
        )
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );

//    ESP_ERROR_CHECK(
//        esp_wifi_start()
//    );
    return ESP_OK;
}

esp_err_t wifi_manager_scan(void)
{
    wifi_info.status = WiFi_Status_SCANNING;

    ESP_ERROR_CHECK(
        esp_wifi_start()
    );

    return esp_wifi_scan_start(
        NULL,
        false
    );
}

esp_err_t wifi_manager_connect(
        const char *ssid,
        const char *password)
{
    wifi_config_t config={};
    strcpy(
        (char*)config.sta.ssid,
        ssid
    );

    strcpy(
        (char*)config.sta.password,
        password
    );

    strcpy(
        wifi_info.ssid,
        ssid
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &config
        )
    );

    wifi_info.status = WiFi_Status_CONNECTING;

    return esp_wifi_connect();

}

void wifi_manager_disconnect(void)
{
    esp_wifi_disconnect();

    wifi_info.status = WiFi_Status_DISCONNECTED;
}



wifi_info_t wifi_manager_get_info(void)
{
    wifi_ap_record_t ap_info;
    if(
      esp_wifi_sta_get_ap_info(
        &ap_info
      )
      == ESP_OK
    )
    {
        wifi_info.rssi =
            ap_info.rssi;
    }
    return wifi_info;
}


WiFi_Status wifi_manager_get_status(void)
{
    return wifi_info.status;
}

