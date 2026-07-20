#include "actions.h"

#include "bluetooth_manager.h"
#include "wifi_manager.h"
#include "bluetooth_manager.h"
#include "wifi_manager.h"
#include "event_bus.h"
#include "wifi_event.h"
#include "device_event.h"
#include "event_bus.h"

#include "eez-flow.h"
#include "structs.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lvgl.h"
#include "router.h"
#include "screens.h"
#include "vars.h"
#include <cstdio>
#include <stdint.h>
#include <sys/_intsup.h>

static const char *TAG = "BuBu-Aquarium-Monitor[ui-actions]";

lv_event_code_t MY_EVENT_UPDATE_DEVICE =
    (lv_event_code_t)(LV_EVENT_VALUE_CHANGED + 1);

void action_home_click(lv_event_t *e) {
  printf("Hello World!");
  lv_event_send(objects.main_screen, MY_EVENT_UPDATE_DEVICE, NULL);
}

void action_router_push(lv_event_t *e) {
  using namespace eez;
  using namespace eez::flow;
  printf("Router Push!");
  Value page_value =
      eez::flow::getUserProperty(ACTION_ROUTER_PUSH_PROPERTY_PAGE);
  // eez::flow::AsyncAction *asyncAction = eez::flow::beginAsyncExecution();
  int32_t page_id = page_value.getInt32();
  RouterPage_t page = {.page_id = static_cast<content_page_t>(page_id),
                       .argument = nullptr};
  Router::instance().push(page);
  printf("value=%ld\n", page_id);
}

void action_router_back(lv_event_t *e) {
  printf("Router Back。。。");
  Router::instance().pop();
}

void action_device_firmware_ota(lv_event_t *e) {
  printf("action_device_firmware_ota Click");
}

void action_wifi_scan_start(lv_event_t *e) {
  printf("Start Scan WiFi......");
  // esp_wifi_scan_start(NULL, false);
}

void action_printf_hw(lv_event_t *e) { printf("Hello World!"); }

void action_printf(lv_event_t *e) {
    const char * text = (const char *)lv_event_get_user_data(e);
    printf("User text: %s\n", text != NULL ? text : "没有内容");
}

extern "C" void action_switch_wifi_enabled(lv_event_t *e) {
  lv_obj_t *switch_obj = lv_event_get_target(e);
  bool checked = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);
  if (checked) {
    ESP_LOGI(TAG, "启用WiFi，开始扫描...");
    WiFiManager::instance().scan_start();
  }
}

extern "C" void action_bluetooth_start_scan(lv_event_t *e) {
  lv_obj_t *switch_obj = lv_event_get_target(e);
  bool checked = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);
  if (checked) {
    ESP_LOGI(TAG, "启用蓝牙扫描...");
    BluetoothManager::instance().start_scan();
  }
}

extern "C" void action_bluetooth_stop_scan(lv_event_t *e) {
	printf("action_bluetooth_stop_scan......");
	BluetoothManager::instance().stop_scan();
    eez::Value value = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_BLUETOOTH_RECORD_LIST);
    eez::ArrayValue *array = value.getArray();
	for (int i = 0; i < array->arraySize; i++) {
    	BluetoothRecordValue record = array->values[i];  // 也可以直接操作
		printf("action_bluetooth_stop_scan......%s", record.name());
		record.active(false);
	}
}

extern "C" void action_connect_bluetooth(lv_event_t *e) {
  printf("action_connect_bluetooth...");
}

extern "C" void action_bluetooth_record_update_event(lv_event_t *e) {}

extern "C" void action_on_pwm_value_changed(lv_event_t *e) {
	  eez::Value pwm_value = eez::flow::getUserProperty(ACTION_ON_PWM_VALUE_CHANGED_PROPERTY_PWM_VALUE);
	  ESP_LOGI(TAG, "获取值%d", pwm_value.getInt());
}

extern "C" void action_on_switch_value_changed(lv_event_t *e) {
	eez::Value checked_value = eez::flow::getUserProperty(ACTION_ON_SWITCH_VALUE_CHANGED_PROPERTY_SWITCH_CHECKED);
	ESP_LOGI(TAG, "获取开关开启状态=%s", checked_value.getBoolean() ? "开启" : "关闭");
	
    EventBus::instance()
    .publish(
        DEVICE_EVENT,
        static_cast<int32_t>(DeviceEvent::DEVICE_REGISTER),
        nullptr,
        0
    );
}

extern "C" void action_on_wifi_enabled(lv_event_t * e) {
	ESP_LOGI(TAG, "启用WiFi，开始扫描WiFi...");
    WiFiManager::instance().scan_start();
};
extern "C" void action_on_wifi_disabled(lv_event_t * e) {};
extern "C" void action_on_wifi_status_switch(lv_event_t * e) {};

extern "C" void action_on_wifi_connect(lv_event_t *e) {
	eez::Value recordIndexValue = eez::flow::getUserProperty(ACTION_ON_WIFI_CONNECT_PROPERTY_RECORD_INDEX);
	int32_t recordIndex = recordIndexValue.getInt32();
	
	eez::Value passwordValue = eez::flow::getUserProperty(ACTION_ON_WIFI_CONNECT_PROPERTY_PASSWORD);
	const char* password = passwordValue.getString(); 
    EventBus::instance()
    .publish(
        WIFI_USER_EVENT,
        static_cast<int32_t>(WiFiUserEvent::WIFI_CONNECT),
        password,
        strlen(password) + 1
    );	
}


void actions_init() {
  lv_obj_add_event_cb(objects.main_screen, action_bluetooth_record_update_event, MY_EVENT_UPDATE_DEVICE, NULL);
}