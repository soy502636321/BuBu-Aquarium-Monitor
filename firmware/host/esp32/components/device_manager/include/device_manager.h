/*
 * 设备管理入口
 *
 *  Created on: 2026年7月13日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_H_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_H_

#include "event_bus.h"
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

  bool registerDevice(Device *device);
  bool unregisterDevice(uint16_t id);
  void onDeviceRegisterEvent(Device *device);
  void onDeviceUnregisterEvent(Device *device);

  Device *find(uint16_t id);

  void init();
  void update();
  void loop();

  size_t count() const;

private:
  DeviceManager() = default;
  ~DeviceManager() = default;

  static void event_handler(void *arg, esp_event_base_t base, int32_t id,
                            void *data);

private:
  std::vector<Device *> devices;
};

#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_MANAGER_H_ */
