#include "event_bus.h"
#include "esp_log.h"

static const char *TAG = "BuBu-Aquarium-Monitor[event_bus]";

EventBus &EventBus::instance() {
  static EventBus bus;
  return bus;
}

esp_err_t EventBus::init() {
	ESP_LOGI(TAG, "事件管理初始化...");
  if (initialized)
    return ESP_OK;

  esp_err_t ret = esp_event_loop_create_default();

  if (ret == ESP_OK) {
    initialized = true;
  }

  return ret;
}

esp_err_t EventBus::publish(esp_event_base_t base, int32_t id, 
                           const void *data, size_t size) {
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_event_post(base, id, data, size, portMAX_DELAY);
}

esp_err_t EventBus::subscribe(esp_event_base_t base, int32_t id, esp_event_handler_t handler, void *arg) {
  return esp_event_handler_register(base, id, handler, arg);
}

esp_err_t EventBus::unsubscribe(esp_event_base_t base, int32_t id, esp_event_handler_t handler) {
  return esp_event_handler_unregister(base, id, handler);
}