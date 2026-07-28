#include <stdint.h>
#include <string>
#include <vector>

#include "event_bus.h"     
#include "bluetooth_manager.h"
#include "bluetooth_event.hpp"

#include "utils.hpp"
#include "data_packet.hpp"
#include "data_gateway.hpp"

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
#include "host/ble_uuid.h"
#include "host/ble_hs_adv.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

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

int BluetoothManager::service_discovery_cb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_svc *service,
    void *arg)
{
    BluetoothManager& manager = BluetoothManager::instance();
    
    // ✅ 正确：检查 error->status
    if (error->status != 0) {
        if (error->status == BLE_HS_EDONE) {
            // ✅ 所有服务发现完成！
            ESP_LOGI(TAG, "✅ All services discovered!");
            
            // 如果找到了Heart Rate服务，开始发现其特征
            if (manager.heart_rate_start_handle != 0) {
                ESP_LOGI(TAG, "🔍 Starting characteristic discovery...");
                // manager.discover_characteristics(conn_handle);
			    // ✅ 在服务范围内发现特征
			    int rc = ble_gattc_disc_all_chrs(
			        conn_handle,
			        manager.heart_rate_start_handle,          // 起始句柄
			        manager.heart_rate_end_handle,            // 结束句柄
			        manager.characteristic_discovery_cb,      // 回调函数
			        NULL                              // 参数（使用单例，传NULL）
			    );
			    
			    if (rc == 0) {
			        ESP_LOGI(TAG, "✅ Characteristic discovery started");
			    } else {
			        ESP_LOGE(TAG, "❌ Failed to start characteristic discovery, rc=%d", rc);
			    }
            } else {
                ESP_LOGW(TAG, "⚠️ Heart Rate Service not found!");
            }
            return 0;
        }
        
        ESP_LOGE(TAG, "Service discovery error: %d", error->status);
        return error->status;
    }

    // 处理发现的每个服务
    if (service != NULL && service->uuid.u.type == BLE_UUID_TYPE_128) {
        	char uuid_str[40];
    		ble_uuid_to_str((ble_uuid_t*)&service->uuid, uuid_str);

        	ESP_LOGI(
				TAG,
				"Found 128-bit service UUID=%s start=%d end=%d",
				uuid_str,
				service->start_handle,
				service->end_handle
			);

        	if(strcmp(uuid_str, BLUETOOTH_SERVICE_DATA_UUID) == 0)
        	{
        		ESP_LOGI(TAG, "发现采集板数据上报服务，服务UUID【%s】", uuid_str);
        		manager.heart_rate_start_handle =
					service->start_handle;

        		manager.heart_rate_end_handle =
					service->end_handle;
        	}
    }
    
    return 0;
}


// ==================== 写入CCCD完成回调 ====================
int BluetoothManager::notify_callback(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg)
{
    if (error->status == 0) {
        ESP_LOGI(TAG, "✅ Notifications enabled successfully!");
        ESP_LOGI(TAG, "  attr_handle: 0x%04X", attr->handle);
        ESP_LOGI(TAG, "  📡 Waiting for heart rate data...");
    } else {
        ESP_LOGE(TAG, "❌ Failed to enable notification, status=%d", error->status);
    }
    return 0;
}

// ==================== 启用通知（推荐） ====================
void BluetoothManager::enable_heart_rate_notifications(uint16_t conn_handle)
{
    BluetoothManager& manager = BluetoothManager::instance();
    
    if (manager.heart_rate_chr_handle == 0) {
        ESP_LOGE(TAG, "❌ Heart Rate characteristic not found!");
        return;
    }
    
    // ✅ 使用描述符发现中找到的 CCCD 句柄
    if (manager.heart_rate_ccc_handle == 0) {
        ESP_LOGE(TAG, "❌ CCCD handle not found!");
        return;
    }
    
    ESP_LOGI(TAG, "📨 Enabling Heart Rate notifications...");
    ESP_LOGI(TAG, "  char_handle: 0x%04X", manager.heart_rate_chr_handle);
    ESP_LOGI(TAG, "  cccd_handle: 0x%04X", manager.heart_rate_ccc_handle);
    
    // ✅ 写入 0x0001 启用通知
    uint8_t value[2] = {0x01, 0x00};
    
    int rc = ble_gattc_write_flat(
        conn_handle,
        manager.heart_rate_ccc_handle,  // ✅ 使用 0x0019
        value,
        sizeof(value),
        notify_callback,
        NULL
    );
    
    if (rc == 0) {
        ESP_LOGI(TAG, "✅ Notification enable request sent to handle 0x%04X", 
                 manager.heart_rate_ccc_handle);
    } else {
        ESP_LOGE(TAG, "❌ Failed to enable notifications, rc=%d", rc);
    }
}

// ==================== 描述符发现回调 ====================
int BluetoothManager::descriptor_discovery_cb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t characteristic_handle,
    const struct ble_gatt_dsc *dsc,
    void *arg)
{
    BluetoothManager& manager = BluetoothManager::instance();
    
    // ✅ 先检查 dsc == NULL（发现完成）
    if (dsc == NULL) {
        // 发现完成，不管 error->status（可能是 0 或 14）
        ESP_LOGI(TAG, "✅ All descriptors discovered!");
        
        if (manager.heart_rate_ccc_handle != 0) {
            ESP_LOGI(TAG, "📝 CCCD found: 0x%04X", manager.heart_rate_ccc_handle);
            manager.enable_heart_rate_notifications(conn_handle);
        } else {
            ESP_LOGE(TAG, "❌ CCCD not found!");
        }
        return 0;
    }

    // ✅ 检查错误（只在 dsc != NULL 时检查）
    if (error->status != 0) {
        ESP_LOGE(TAG, "Descriptor discovery error: %d", error->status);
        return error->status;
    }

    // ✅ 只处理 16-bit UUID
    if (dsc->uuid.u.type == BLE_UUID_TYPE_16) {
        uint16_t uuid16 = dsc->uuid.u16.value;
        
        // 转换为字符串（用于调试）
        char uuid_str[64];
        ble_uuid_to_str((ble_uuid_t*)&dsc->uuid, uuid_str);
        ESP_LOGI(TAG, "📋 Descriptor: %s, handle=0x%04X", uuid_str, dsc->handle);
        
        // ✅ 只匹配 CCCD (0x2902)
        if (uuid16 == 0x2902) {
            ESP_LOGI(TAG, "📝 Found CCCD descriptor!");
            ESP_LOGI(TAG, "  handle: 0x%04X", dsc->handle);
            manager.heart_rate_ccc_handle = dsc->handle;
        }
    }
    
    return 0;
}
// ==================== 发现描述符 ====================
void BluetoothManager::discover_descriptors(uint16_t conn_handle, uint16_t char_handle)
{
    BluetoothManager& manager = BluetoothManager::instance();
    
    ESP_LOGI(TAG, "🔍 Discovering descriptors for char_handle: 0x%04X", char_handle);
    
    // 获取特征所在的服务的范围
    if (manager.heart_rate_start_handle == 0 || manager.heart_rate_end_handle == 0) {
        ESP_LOGE(TAG, "❌ Service handle range not set!");
        return;
    }
    
    // ✅ 发现所有描述符
    int rc = ble_gattc_disc_all_dscs(
        conn_handle,
        manager.heart_rate_start_handle,
        manager.heart_rate_end_handle,
        descriptor_discovery_cb,
        NULL
    );
    
    if (rc == 0) {
        ESP_LOGI(TAG, "✅ Descriptor discovery started");
    } else {
        ESP_LOGE(TAG, "❌ Failed to discover descriptors, rc=%d", rc);
    }
}

// 特征发现回调
int BluetoothManager::characteristic_discovery_cb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_chr *chr,
    void *arg)
{
    BluetoothManager& manager = BluetoothManager::instance();

    // ✅ 先检查 chr == NULL（发现完成）
    if (chr == NULL) {
        // 发现完成，不管 error->status 是什么（通常是 0 或 14）
        ESP_LOGI(TAG, "✅ All characteristics discovered!");
        
        if (manager.heart_rate_chr_handle != 0) {
            ESP_LOGI(TAG, "❤️ Heart Rate char handle: 0x%04X", manager.heart_rate_chr_handle);
            // ✅ 启用通知
            // manager.enable_heart_rate_notifications(conn_handle); //会错误
            // ✅ 先发现描述符，找到 CCCD 后再启用通知
			manager.discover_descriptors(conn_handle, manager.heart_rate_chr_handle);
        } else {
            ESP_LOGW(TAG, "⚠️ Heart Rate Measurement not found!");
        }
        return 0;
    }

    // ✅ 处理每个特征（chr != NULL）
    // 如果是真正的错误，error->status != 0
    if (error->status != 0) {
        ESP_LOGE(TAG, "Characteristic discovery error: %d", error->status);
        return error->status;
    }

    if (chr->uuid.u.type == BLE_UUID_TYPE_128) {
        uint16_t uuid16 = chr->uuid.u16.value;
        ESP_LOGI(TAG, "📋 Found characteristic: 0x%04X, def_handle=%d", 
                 uuid16, chr->def_handle);
        
        if (uuid16 == 0x7685) {
            ESP_LOGI(TAG, "❤️ Found Heart Rate Measurement!");
            manager.heart_rate_chr_handle = chr->def_handle;
        }
    }
    
    return 0;
}


void BluetoothManager::handleConnectEvent(struct ble_gap_event *event) {
  if (event->connect.status == 0) {
    this->status = BLE_STATUS_CONNECTED;
    this->conn_id = event->connect.conn_handle;
    ESP_LOGI(TAG, "蓝牙设备连接成功");

        // ✅ 主动发起服务发现
        int rc = ble_gattc_disc_all_svcs(
            event->connect.conn_handle,
            service_discovery_cb,    // 回调函数
            NULL                     // 回调参数
        );
        
        if (rc == 0) {
            ESP_LOGI(TAG, "Service discovery started");
        }
            
  } else {
	         ESP_LOGE(TAG, "❌ Connection failed, status=%d", event->connect.status);
        this->status = BLE_STATUS_DISCONNECTED;
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


void BluetoothManager::handleServiceDiscoveryCompleteEvent(struct ble_gattc_event *event) {
    ESP_LOGI(TAG, "✅ Service discovery complete");
    
    // ✅ 现在可以开始查找特征了
    // 方式1：直接搜索特定 UUID
    ble_uuid_t target_uuid;
    // 设置你需要的服务 UUID...
    
    // 方式2：或者在 BLE_GATTC_EVENT_DISC_CHR 中逐个匹配
}

void BluetoothManager::handleCharacteristicDiscoveredEvent(struct ble_gattc_event *event) {
   
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
		case BLE_GAP_EVENT_DISCONNECT: {
    	ESP_LOGI(TAG, "❌ Disconnected, reason=%d", event->disconnect.reason);
		  break;
	  }
	  case BLE_GAP_EVENT_ENC_CHANGE: {
		  break;
	  }
	  case BLE_GAP_EVENT_NOTIFY_RX: {
		  ESP_LOGI(TAG, "收到心跳数据");

	  	uint8_t buffer[10] = {0};  // 全部初始化为 0
	  	DataGateway::instance().receive(buffer, sizeof(buffer), DataSource::BLE);
    uint16_t len = OS_MBUF_PKTLEN(event->notify_rx.om);
    uint8_t *data = event->notify_rx.om->om_data;
    
    		ESP_LOGI(TAG, "📩 Heart Rate Notification:");
    		ESP_LOG_BUFFER_HEX(TAG, data, len);
 
    uint8_t flags = data[0];
    uint16_t heart_rate = 0;
    uint16_t offset = 1;
 
    // ✅ 解析心率值
    if (flags & 0x01) {
        // 16-bit 心率值
        if (len >= 3) {
            heart_rate = (data[1] << 8) | data[2];
            offset += 2;
        }
    } else {
        // 8-bit 心率值
        heart_rate = data[1];
        offset += 1;
    }
    
    ESP_LOGI(TAG, "❤️ Heart Rate: %d BPM", heart_rate);
    
    // ✅ 传感器接触状态
    if (flags & 0x02) {
        bool contact = (flags & 0x04) != 0;
        ESP_LOGI(TAG, "  📍 Sensor Contact: %s", contact ? "YES" : "NO");
    }
    
    // ✅ 能量消耗（如果有）
    if ((flags & 0x08) && len >= offset + 2) {
        uint16_t energy = (data[offset] << 8) | data[offset + 1];
        ESP_LOGI(TAG, "  ⚡ Energy Expended: %d", energy);
        offset += 2;
    }
    
    // ✅ RR-Interval（如果有）
    if ((flags & 0x10) && len >= offset + 2) {
        uint16_t rr = (data[offset] << 8) | data[offset + 1];
        ESP_LOGI(TAG, "  📊 RR-Interval: %.2f ms", rr / 1024.0);
    }
		  break;
	  }
	  case BLE_GAP_EVENT_MTU: {
		  
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