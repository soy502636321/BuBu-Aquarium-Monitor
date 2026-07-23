/*
 * device_manager.cpp
 *
 *  Created on: 2026年7月13日
 *      Author: Hu
 */

#include "device_manager.hpp"
#include "device_event.hpp"
#include "device.hpp"
#include "esp_log.h"

#include <algorithm>

static const char *TAG = "BuBu-Aquarium-Monitor[device_manager]";

ESP_EVENT_DEFINE_BASE(DEVICE_EVENT);

DeviceManager &DeviceManager::instance() {
  static DeviceManager instance;
  return instance;
}

void DeviceManager::onDeviceRegisterEvent(DeviceBase *device) {
	ESP_LOGI(TAG, "触发设备在线事件");
	//测试mqtt的发送
	
}

void DeviceManager::onDeviceUnregisterEvent(DeviceBase *device) {
	ESP_LOGI(TAG, "触发设备离线事件事件");
}


void DeviceManager::event_handler(
    void *arg,
    esp_event_base_t base,
    int32_t id,
    void *data
)
{
	ESP_LOGI(TAG, "触发事件");
    auto self =
    static_cast<DeviceManager*>(arg);

    switch(id)
    {

    case DEVICE_REGISTER:
        self->onDeviceRegisterEvent(static_cast<DeviceBase*>(data));
        break;

    case DEVICE_UNREGISTER:
		self->onDeviceUnregisterEvent(static_cast<DeviceBase*>(data));
        break;
    }
}

bool DeviceManager::registerDevice(DeviceBase *device) {
  if (device == nullptr) {
    return false;
  }

  // 防止重复注册
  if (find(device->getDeviceId()) != nullptr) {
    return false;
  }

  devices.push_back(device);

  return true;
}

bool DeviceManager::unregisterDevice(const std::string& device_id) {
    auto it = std::find_if(devices.begin(), devices.end(),
                           [&device_id](DeviceBase* device) {  // 参数类型 DeviceBase*
                               return device->getDeviceId() == device_id;
                           });
    
    if (it == devices.end()) {
        ESP_LOGW(TAG, "Device %s not found", device_id.c_str());
        return false;
    }
    
    devices.erase(it);
    ESP_LOGI(TAG, "Device %s unregistered", device_id.c_str());
    return true;
}

DeviceBase *DeviceManager::find(const std::string& id) {
  for (auto *device : devices) {
    if (device->getDeviceId() == id) {
      return device;
    }
  }

  return nullptr;
}

void DeviceManager::init() {
  ESP_LOGI(TAG, "设备管理初始化...");
    for (int i = 0; i < DEVICE_EVENT_COUNT; i++) {
        DeviceEvent event = static_cast<DeviceEvent>(i);
        
        // 普通 enum 可以隐式转换为 int32_t
        EventBus::instance().subscribe(
            DEVICE_EVENT,
            event,  // ✅ 直接使用，不需要 static_cast
            DeviceManager::event_handler,
            this
        );
    }  
}

void DeviceManager::update() {
  for (auto *device : devices) {
    if (device != nullptr) {
      // device->update();
    }
  }
}

void DeviceManager::loop() {
  for (auto *device : devices) {
    if (device != nullptr) {
      // device->loop();
    }
  }
}

size_t DeviceManager::count() const { return devices.size(); }