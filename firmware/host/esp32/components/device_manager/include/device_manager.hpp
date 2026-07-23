/*
 * 设备管理入口
 *
 *  Created on: 2026年7月13日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_HPP_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_HPP_

#include "event_bus.h"
#include "device.hpp"
#include <cstdint>
#include <vector>

class Device;

class DeviceManager {
public:
  // 获取单例
  static DeviceManager &instance();

  // 禁止拷贝
  DeviceManager(const DeviceManager &) = delete;
  DeviceManager &operator=(const DeviceManager &) = delete;

  bool registerDevice(DeviceBase *device);
  bool unregisterDevice(const std::string& id);
  void onDeviceRegisterEvent(DeviceBase *device);
  void onDeviceUnregisterEvent(DeviceBase *device);

  DeviceBase *find(const std::string& id);

  void init();
  void update();
  void loop();

  size_t count() const;

private:
  DeviceManager() = default;
  ~DeviceManager() = default;

  static void event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);

private:
  std::vector<DeviceBase *> devices;
};

#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_HPP_ */
