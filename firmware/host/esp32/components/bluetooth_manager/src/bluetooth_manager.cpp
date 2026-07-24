#include <stdint.h>
#include <string>
#include <vector>

#include "event_bus.h"     
#include "bluetooth_manager.h"
#include "bluetooth_event.hpp"

#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "eez-flow.h"
#include "structs.h"
#include "vars.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "services/gap/ble_svc_gap.h"

static const char *TAG = "BuBu-Aquarium-Monitor[bluetooth_manager]";

/**
 * 单例
 */
BluetoothManager &BluetoothManager::instance() {
  static BluetoothManager manager;
  return manager;
}

BluetoothManager::BluetoothManager() {
  memset(device_name, 0, sizeof(device_name));

  memset(device_list, 0, sizeof(device_list));

  status = BLE_STATUS_INIT;

  device_count = 0;
}

esp_err_t BluetoothManager::init(const char *name) {

  ESP_LOGI(TAG, "Bluetooth init");

  strncpy(device_name, name, sizeof(device_name) - 1);

  nimble_port_init();

  ble_svc_gap_init();

  ble_svc_gap_device_name_set(device_name);

  ble_hs_cfg.sync_cb = [](void) { BluetoothManager::instance().onSync(); };

  nimble_port_freertos_init([](void *param) { nimble_port_run(); });
  
    //注册自定义事件
    for (int i = 0; i < BLUETOOTH_USER_EVENT_COUNT; i++) {
        BluetoothUserEvent event = static_cast<BluetoothUserEvent>(i);
        
        // 普通 enum 可以隐式转换为 int32_t
        EventBus::instance().subscribe(
            BLUETOOTH_USER_EVENT,
            event,  // ✅ 直接使用，不需要 static_cast
            BluetoothManager::user_event_handler,
            this
        );
    } 
  return ESP_OK;
}

void BluetoothManager::onSync() {
	ESP_LOGI(TAG, "测试回调的位置"); 
	// advertise(); 
}

void BluetoothManager::advertise() {

  struct ble_gap_adv_params params = {};

  ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &params, &BluetoothManager::event_handler, this);

  status = BLE_STATUS_ADVERTISING;
}

void BluetoothManager::start_scan() {
    ESP_LOGI(TAG, "开始扫描附近蓝牙设备...");
  device_count = 0;
  	struct ble_gap_disc_params params = {
	  	.itvl = 0x0010,  			//160ms
		.window = 0x0010,			//50ms
		.filter_policy = 0,			//过滤
		.limited = 0,				//不限时
		.passive = 0,				//主动扫描
		.filter_duplicates = 1		//过滤重复
	};

  int rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, 5000, &params, &BluetoothManager::event_handler, this);
  ESP_LOGI(TAG, "蓝牙扫描完成，扫描结果【%d】", rc);
}

void BluetoothManager::stop_scan() {
  ble_gap_disc_cancel();
  status = BLE_STATUS_IDLE;
}

esp_err_t BluetoothManager::connect(const uint8_t* addr, uint8_t addr_type) {
 	// 设置目标设备地址
    //memcpy(target_device_addr_.val, addr, 6);
    //target_device_addr_.type = addr_type;
    ble_addr_t target_device_addr_;
	memcpy(target_device_addr_.val, addr, 6);
	target_device_addr_.type = addr_type;
    
    // 配置连接参数
    struct ble_gap_conn_params conn_params = {};
    conn_params.scan_itvl = 0x0010;      // 扫描间隔 (10ms)
    conn_params.scan_window = 0x0010;    // 扫描窗口 (10ms)
    conn_params.itvl_min = 0x0018;       // 最小连接间隔 (24 * 1.25ms = 30ms)
    conn_params.itvl_max = 0x0028;       // 最大连接间隔 (40 * 1.25ms = 50ms)
    conn_params.latency = 0;             // 从机延迟
    conn_params.supervision_timeout = 0x0400;  // 超时时间 (4s)
    conn_params.min_ce_len = 0;          // 最小 CE 长度
    conn_params.max_ce_len = 0;          // 最大 CE 长度
    
    // ✅ 发起连接
    int rc = ble_gap_connect(
        BLE_OWN_ADDR_PUBLIC,       // 本地地址类型
        &target_device_addr_,      // 目标设备地址
        30000,                     // 超时 30 秒
        &conn_params,              // 连接参数
        &BluetoothManager::event_handler,     // 回调函数
        nullptr                       // 传递 this
    );
    
	if (rc == 0) {
        //is_connecting_ = true;
        std::string addr_str = "蓝牙地址";
        ESP_LOGI(TAG, "Connecting to %s...", addr_str.c_str());
        return true;
    } else {
        ESP_LOGE(TAG, "Failed to connect, rc=%d", rc);
        return false;
    }
}

void BluetoothManager::handleConnectEvent(struct ble_gap_event *event) {
  if (event->connect.status == 0) {
    this->status = BLE_STATUS_CONNECTED;
    this->conn_id = event->connect.conn_handle;
    ESP_LOGI(TAG, "connected");
  }
}

void BluetoothManager::handleDiscEvent(struct ble_gap_event *event) {
  struct ble_hs_adv_fields fields;
  int rc = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
  if (rc == 0 && fields.name != NULL) {
    if (device_count < BLUETOOTH_DEVICE_MAX_SIZE) {
      auto &dev = device_list[device_count++];
      memset(&dev, 0, sizeof(dev));
      memcpy(dev.addr, event->disc.addr.val, 6);
    	dev.addr_type = event->disc.addr.type;
      memcpy(dev.name, fields.name, fields.name_len);
      dev.rssi = event->disc.rssi;
      ESP_LOGI(TAG,
               "设备名称:[%s] 设备地址:[%02X:%02X:%02X:%02X:%02X:%02X] "
               "设备信号:[%d]",
               dev.name, dev.addr[0], dev.addr[1], dev.addr[2], dev.addr[3],
               dev.addr[4], dev.addr[5], dev.rssi);
    }
  }
}

void BluetoothManager::handleDiscCompleteEvent(struct ble_gap_event *event) {
    ESP_LOGI(TAG, "蓝牙扫描完成，发现[%d]个设备", device_count);
    eez::Value value = eez::flow::getGlobalVariable(
        FLOW_GLOBAL_VARIABLE_BLUETOOTH_RECORD_LIST);
    eez::ArrayValue *array = value.getArray();
    eez::Value *records = array->values;
    // 更新到ui数组
    for (int i = 0; i < device_count; i++) {
      bluetooth_device_t &bluetooth_device = device_list[i];
      BluetoothRecordValue record = records[i];
      record.active(true);
      record.rssi(bluetooth_device.rssi);
      record.name(bluetooth_device.name);
    }
}

bluetooth_status_t BluetoothManager::getStatus() { return status; }

bluetooth_device_t *BluetoothManager::getDevices() { return device_list; }

int BluetoothManager::getDeviceCount() { return device_count; }

int BluetoothManager::event_handler(struct ble_gap_event *event, void *arg) {
    auto& mgr = instance();
    
    switch (event->type) {
	  case BLE_GAP_EVENT_CONNECT: {
		mgr.handleConnectEvent(event);
	    break;
	  }
	  case BLE_GAP_EVENT_DISC: {
		  mgr.handleDiscEvent(event);
		  break;
	  }
	  case BLE_GAP_EVENT_DISC_COMPLETE: {
		  mgr.handleDiscCompleteEvent(event);
		  break;
	  }
		default:
		break;
	}
	return 0;
}

void BluetoothManager::user_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *data) {
    auto& mgr = instance();
    if (event_base == BLUETOOTH_USER_EVENT) {
		switch(event_id) {
			case BLUETOOTH_USER_EVENT_CONNECT_REQUEST: {
    			int32_t record_index = *(int32_t*)data;
    			bluetooth_device_t &bluetooth_device = mgr.device_list[record_index];
				ESP_LOGI(TAG, "收到蓝牙连接的请求，设备名称:[%s] 设备地址:[%02X:%02X:%02X:%02X:%02X:%02X] "
               "设备信号:[%d]",
               	bluetooth_device.name, bluetooth_device.addr[0], bluetooth_device.addr[1], bluetooth_device.addr[2], bluetooth_device.addr[3],
               	bluetooth_device.addr[4], bluetooth_device.addr[5], bluetooth_device.rssi);
               	
               	mgr.connect(bluetooth_device.addr, bluetooth_device.addr_type);
				break;
			}
			default:
				break;
		}        
    }		
}