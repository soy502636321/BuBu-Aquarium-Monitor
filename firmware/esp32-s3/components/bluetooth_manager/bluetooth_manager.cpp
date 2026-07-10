#include <string>
#include <vector>

#include "bluetooth_manager.h"

#include "esp_log.h"
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

  return ESP_OK;
}

void BluetoothManager::onSync() { advertise(); }

void BluetoothManager::advertise() {

  struct ble_gap_adv_params params = {};

  ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &params,
                    gapEventHandler, this);

  status = BLE_STATUS_ADVERTISING;
}

void BluetoothManager::start_scan() {

  device_count = 0;

  struct ble_gap_disc_params params = {.itvl = 0x0010,
                                       .window = 0x0010,
                                       .filter_policy = 0,
                                       .limited = 0,
                                       .passive = 0,
                                       .filter_duplicates = 1};

  int rc =
      ble_gap_disc(BLE_OWN_ADDR_PUBLIC, 5000, &params, gapEventHandler, this);

  ESP_LOGI(TAG, "scan result=%d", rc);
}

void BluetoothManager::stop_scan() {
  ble_gap_disc_cancel();

  status = BLE_STATUS_IDLE;
}

int BluetoothManager::gapEventHandler(struct ble_gap_event *event, void *arg) {

  BluetoothManager *manager = static_cast<BluetoothManager *>(arg);

  return manager->handleEvent(event);
}

int BluetoothManager::handleEvent(struct ble_gap_event *event) {

  switch (event->type) {

  case BLE_GAP_EVENT_CONNECT: {

    if (event->connect.status == 0) {
      status = BLE_STATUS_CONNECTED;

      conn_id = event->connect.conn_handle;

      ESP_LOGI(TAG, "connected");
    }

    break;
  }
  case BLE_GAP_EVENT_DISC: {
    struct ble_hs_adv_fields fields;
    int rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
    if (rc == 0 && fields.name != NULL) {
      if (device_count < BLUETOOTH_DEVICE_MAX_SIZE) {
        auto &dev = device_list[device_count++];
        memset(&dev, 0, sizeof(dev));
        memcpy(dev.addr, event->disc.addr.val, 6);
        memcpy(dev.name, fields.name, fields.name_len);
        dev.rssi = event->disc.rssi;
        ESP_LOGI(TAG,
                 "设备名称:[%s] 设备地址:[%02X:%02X:%02X:%02X:%02X:%02X] "
                 "设备信号:[%d]",
                 dev.name, dev.addr[0], dev.addr[1], dev.addr[2], dev.addr[3],
                 dev.addr[4], dev.addr[5], dev.rssi);
      }
      break;
    }
  }
  case BLE_GAP_EVENT_DISC_COMPLETE: {
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
    break;
  }
  default:
    break;
  }

  return 0;
}

bluetooth_status_t BluetoothManager::getStatus() { return status; }

bluetooth_device_t *BluetoothManager::getDevices() { return device_list; }

int BluetoothManager::getDeviceCount() { return device_count; }