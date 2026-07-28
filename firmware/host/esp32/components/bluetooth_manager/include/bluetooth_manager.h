#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_event.h"

#define BLUETOOTH_DEVICE_MAX_SIZE 10
#define BLUETOOTH_SERVICE_DATA_UUID "4eb240fb-aea0-4235-9b45-af0f9c76852b"
#define BLUETOOTH_SERVICE_COMMAND_UUID "94d263bf-1df5-4ddb-901c-e2902511b385"

struct bluetooth_device_t {
  char name[32];
  uint8_t addr[6];
	uint8_t addr_type;          // ✅ 地址类型
  int rssi;
};

enum bluetooth_status_t {
  BLE_STATUS_INIT,
  BLE_STATUS_IDLE,
  BLE_STATUS_CONNECTED,
  BLE_STATUS_DISCONNECTED,
  BLE_STATUS_ADVERTISING
};

class BluetoothManager {

public:
  // 获取单例
  static BluetoothManager &instance();

  // 初始化
  esp_err_t init(const char *device_name);

  // 开始扫描
  void start_scan();

  // 停止
  void stop_scan();
  
	esp_err_t connect(const uint8_t* addr, uint8_t addr_type);

  bluetooth_status_t getStatus();

  bluetooth_device_t *getDevices();

  int getDeviceCount();
  
  void discover_descriptors(uint16_t conn_handle, uint16_t char_handle);
  
  // ==================== 描述符发现回调 ====================
static int descriptor_discovery_cb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
        uint16_t characteristic_handle,     // ✅ 添加
    const struct ble_gatt_dsc *dsc,
    void *arg);
  
    // ✅ 启用通知
    void enable_heart_rate_notifications(uint16_t conn_handle);
    
   // ✅ 回调函数
    static int notify_callback(uint16_t conn_handle,
                               const struct ble_gatt_error *error,
                               struct ble_gatt_attr *attr,
                               void *arg);
  
	static int event_handler(struct ble_gap_event *event, void *arg);
	
    //static int gattc_event_handler(struct ble_gattc_event *event, void *arg);
	
	static void user_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);
	
	static int service_discovery_cb(
	    uint16_t conn_handle,
	    const struct ble_gatt_error *error,
	    const struct ble_gatt_svc *service,
	    void *arg);
	    
    static int characteristic_discovery_cb(uint16_t conn_handle,
                                           const struct ble_gatt_error *error,
                                           const struct ble_gatt_chr *chr,
                                           void *arg);

private:
  // 构造私有化
  BluetoothManager();

  // 禁止复制
  BluetoothManager(const BluetoothManager &) = delete;

  BluetoothManager &operator=(const BluetoothManager &) = delete;

private:
  //static int gapEventHandler(struct ble_gap_event *event, void *arg);
  
  void handleConnectEvent(struct ble_gap_event *event);
  
  void handleDiscEvent(struct ble_gap_event *event);
  
  void handleDiscCompleteEvent(struct ble_gap_event *event);

  int handleEvent(struct ble_gap_event *event);
  
    // GATT 事件处理
    void handleServiceDiscoveryCompleteEvent(struct ble_gattc_event *event);
    void handleCharacteristicDiscoveredEvent(struct ble_gattc_event *event);
    void handleNotifyEvent(struct ble_gattc_event *event);
    void handleWriteEvent(struct ble_gattc_event *event);
  
  void advertise();

  void onSync();

private:
  char device_name[32];

  bluetooth_status_t status;

  uint16_t conn_id;

  bluetooth_device_t device_list[BLUETOOTH_DEVICE_MAX_SIZE];

  int device_count;
  
  // 测试数据
    // Heart Rate Service相关
    uint16_t heart_rate_start_handle = 0;
    uint16_t heart_rate_end_handle = 0;
    uint16_t heart_rate_chr_handle = 0;        // 心率特征句柄
    uint16_t heart_rate_ccc_handle = 0;        // CCCD句柄
    bool heart_rate_notification_enabled = false;
    uint16_t current_heart_rate = 0;
  
};