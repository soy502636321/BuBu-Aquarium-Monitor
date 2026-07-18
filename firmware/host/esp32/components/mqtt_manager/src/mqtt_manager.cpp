#include "mqtt_manager.h"
#include "event_bus.h"
#include "data_types.h"
#include "device_event.h" 
#include "mqtt_config.h"
#include "mqtt_topic.h"

#include "esp_log.h"
#include <string>

static const char *TAG = "MqttManager";

MqttManager &MqttManager::instance() {
  static MqttManager obj;
  return obj;
}

MqttManager::MqttManager() {}

MqttManager::~MqttManager() {

  if (client_) {
    esp_mqtt_client_stop(client_);

    esp_mqtt_client_destroy(client_);
  }
}

bool MqttManager::init(const std::string &broker) {
  // 如果传入的 broker 为空，使用配置中的默认值
  std::string brokerUri = broker.empty() ? std::string(MqttBrokerConfig::broker()) : broker;
  ESP_LOGI(TAG, "正在连接至MQTT服务器: %s", brokerUri.c_str());
  esp_mqtt_client_config_t cfg = {};

  // -------- Broker 地址 --------
  cfg.broker.address.uri = brokerUri.c_str();

  // -------- 认证信息 --------
  auto username = MqttBrokerConfig::username();
  auto password = MqttBrokerConfig::password();
  if (!username.empty()) {
    cfg.credentials.username = username.data();
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // ESP-IDF v5.0+
    if (!password.empty()) {
      cfg.credentials.authentication.password = password.data();
    }
#else
    // ESP-IDF v4.x
    if (!password.empty()) {
      cfg.credentials.password = password.data();
    }
#endif
  }

  // -------- Client ID --------
  std::string clientId = MqttBrokerConfig::clientId();
  cfg.credentials.client_id = clientId.c_str();
  ESP_LOGI(TAG, "客户端 ID: %s", clientId.c_str());

  // -------- 连接参数 --------
  cfg.session.keepalive = MqttBrokerConfig::keepalive();
  cfg.session.disable_clean_session = false;
  cfg.network.disable_auto_reconnect = false;
  cfg.network.reconnect_timeout_ms = MqttBrokerConfig::reconnectTimeoutMs();

  // -------- SSL/TLS 配置 --------
  if (MqttBrokerConfig::sslEnabled()) {
    cfg.broker.verification.certificate = MqttBrokerConfig::ca_cert();
    ESP_LOGI(TAG, "SSL 启用");
  }

  client_ = esp_mqtt_client_init(&cfg);
  if (!client_) {
    ESP_LOGE(TAG, "创建 MQTT 客户端失败");
    return false;
  }

  esp_mqtt_client_register_event(client_, MQTT_EVENT_ANY, eventHandler, this);

  esp_err_t ret = esp_mqtt_client_start(client_);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(ret));
    return false;
  }

  ESP_LOGI(TAG, "MQTT initialized successfully");
  return true;
}

void MqttManager::setDeviceInfo(const std::string &boardId,
                                const std::string &userId) {

  boardId_ = boardId;

  userId_ = userId;
}

bool MqttManager::connected() const { return connected_; }

bool MqttManager::publish(const std::string &topic, const std::string &payload,int qos) {
  if (!connected_)
    return false;
  int id = esp_mqtt_client_publish(client_, topic.c_str(), payload.c_str(), 0,qos, 0);
  return id >= 0;
}

bool MqttManager::subscribe(const std::string &topic, int qos) {
  if (!connected_)
    return false;

  int id = esp_mqtt_client_subscribe(client_, topic.c_str(), qos);

  return id >= 0;
}

void onTestHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
	ESP_LOGI(TAG, "测试mqtt事件调用发布消息");
		
}

void MqttManager::eventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {

  auto self = static_cast<MqttManager *>(handler_args);

  auto event = static_cast<esp_mqtt_event_handle_t>(event_data);

  switch (event_id) {

  case MQTT_EVENT_CONNECTED: {
    self->connected_ = true;
    ESP_LOGI(TAG, "MQTT 成功连接");
    //订阅通用主题
    std::string commandTopic = MqttTopicConfig::makeCommandTopic();
    ESP_LOGI(TAG, "测试主题%s", commandTopic.c_str()); 
    MqttManager::instance().subscribe(commandTopic, MqttTopicConfig::QOS_COMMAND);
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    self->connected_ = false;

    ESP_LOGW(TAG, "MQTT 断开连接");

    break;
  }
  case MQTT_EVENT_DATA: {
    ESP_LOGI(TAG, "RX topic:%.*s", event->topic_len, event->topic);
    ESP_LOGI(TAG, "RX data:%.*s", event->data_len, event->data);
	DeviceData data;
	data.device_id = "AA:BB:CC:DD:EE:FF";
    data.timestamp = 1690000000;
    data.add("angle_x", 0.058f)
        .add("angle_y", 0.032f)
        .add("temperature", 25.5f)
        .add("rssi", -65)
        //.add("status", std::string("normal"))
        ;
	
    break;
  }
  default:

    break;
  }
}