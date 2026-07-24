#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_event.h"

#define BLUETOOTH_DEVICE_MAX_SIZE 10

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
  
	static int event_handler(struct ble_gap_event *event, void *arg);
	
	static void user_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);

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
  
  void advertise();

  void onSync();

private:
  char device_name[32];

  bluetooth_status_t status;

  uint16_t conn_id;

  bluetooth_device_t device_list[BLUETOOTH_DEVICE_MAX_SIZE];

  int device_count;
  
};