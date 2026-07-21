#include "wifi_manager.h"
#include "actions.h"
#include "lwip/sockets.h"
#include "psa_crypto_se.h"
#include "wifi_event.h"
#include "event_bus.h"     
#include "ui_event.h"
#include "mqtt_manager.h" 

// 确保包含所有必要的头文件
#include <cstring>
#include <algorithm>
#include <map>

// ESP-IDF 头文件（虽然已经在 .h 中包含了，但 .cpp 中也需要）
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "eez-flow.h"
#include "structs.h"
#include "vars.h"

static constexpr const char* TAG = "WiFiManager";

// ============================================================================
// 单例实现（Meyers Singleton）
// ============================================================================

WiFiManager& WiFiManager::instance() {
    static WiFiManager instance;
    return instance;
}

// ============================================================================
// 辅助函数
// ============================================================================

static WiFiAuthMode to_wifi_auth_mode(wifi_auth_mode_t mode) {
    switch (mode) {
        case WIFI_AUTH_OPEN:               return WiFiAuthMode::OPEN;
        case WIFI_AUTH_WPA2_PSK:           return WiFiAuthMode::WPA2_PSK;
        case WIFI_AUTH_WPA3_PSK:           return WiFiAuthMode::WPA3_PSK;
        case WIFI_AUTH_WPA2_ENTERPRISE:    return WiFiAuthMode::WPA2_ENTERPRISE;
        // case WIFI_AUTH_WPA3_ENTERPRISE:    return WiFiAuthMode::WPA3_ENTERPRISE;
        default:                           return WiFiAuthMode::OPEN;
    }
}

static std::string bssid_to_string(const uint8_t bssid[6]) {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return std::string(buf);
}

// ============================================================================
// 初始化 / 反初始化
// ============================================================================

esp_err_t WiFiManager::init(bool enable_auto_reconnect) {
    if (m_is_initialized) {
        ESP_LOGW(TAG, "WiFiManager already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing WiFiManager (auto_reconnect=%s)",  enable_auto_reconnect ? "ON" : "OFF");
    
    m_auto_reconnect = enable_auto_reconnect;
    m_user_disconnect = false;

    // 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化网络接口
    ret = esp_netif_init();
    ESP_ERROR_CHECK(ret);

    // 初始化事件循环
	ret = esp_event_loop_create_default();
	if (ret == ESP_ERR_INVALID_STATE) {
	    ESP_LOGW(TAG, "Event loop already created, continuing...");
	    ret = ESP_OK;
	}
	ESP_ERROR_CHECK(ret);

    // 创建默认 WiFi 站接口
    esp_netif_create_default_wifi_sta();

    // 初始化 WiFi 驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    ESP_ERROR_CHECK(ret);

    // 注册事件处理器
    ret = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
                                      &WiFiManager::event_handler, nullptr);
    ESP_ERROR_CHECK(ret);
    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                                      &WiFiManager::event_handler, nullptr);
    ESP_ERROR_CHECK(ret);

    // 设置 WiFi 模式
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_ERROR_CHECK(ret);

    // 启动 WiFi
    ret = esp_wifi_start();
    ESP_ERROR_CHECK(ret);

    m_is_initialized = true;
    m_info.status = WiFiStatus::DISCONNECTED;
    
    //注册自定义事件
    for (int i = 0; i < WIFI_EVENT_COUNT; i++) {
        WiFiUserEvent event = static_cast<WiFiUserEvent>(i);
        
        // 普通 enum 可以隐式转换为 int32_t
        EventBus::instance().subscribe(
            WIFI_USER_EVENT,
            event,  // ✅ 直接使用，不需要 static_cast
            WiFiManager::user_event_handler,
            this
        );
    } 
    
    ESP_LOGI(TAG, "WiFiManager initialized successfully");
    return ESP_OK;
}

void WiFiManager::deinit() {
    if (!m_is_initialized) return;
    
    ESP_LOGI(TAG, "Deinitializing WiFiManager");
    
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_event_loop_delete_default();
    
    m_is_initialized = false;
    m_info.clear();
    m_scan_results.clear();
    m_last_ssid.clear();
    m_last_password.clear();
    
    ESP_LOGI(TAG, "WiFiManager deinitialized");
}

// ============================================================================
// 扫描
// ============================================================================

esp_err_t WiFiManager::scan_start(uint16_t scan_time_ms, bool show_hidden) {
    if (!m_is_initialized) {
        ESP_LOGE(TAG, "WiFiManager not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (m_is_scanning) {
        ESP_LOGW(TAG, "Scan already in progress");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Starting WiFi scan (time=%dms, hidden=%s)", 
             scan_time_ms, show_hidden ? "ON" : "OFF");
    
    m_is_scanning = true;
    m_scan_results.clear();
    update_status(WiFiStatus::SCANNING);
    
    wifi_scan_config_t scan_config = {
        .ssid = nullptr,
        .bssid = nullptr,
        .channel = 0,
        .show_hidden = show_hidden,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        // 和蓝牙共存情况下无法设置扫描事件
        //.scan_time = {
        //    .active = {
        //        .min = static_cast<uint32_t>(scan_time_ms / 2),
        //        .max = static_cast<uint32_t>(scan_time_ms)
        //    }
        //}
    };
    
    esp_err_t ret = esp_wifi_scan_start(&scan_config, false);
    if (ret != ESP_OK) {
        m_is_scanning = false;
        update_status(WiFiStatus::DISCONNECTED);
        ESP_LOGE(TAG, "Scan start failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

std::vector<WiFiAPInfo> WiFiManager::get_scan_results() const {
    return m_scan_results;
}

// ============================================================================
// 连接管理
// ============================================================================

esp_err_t WiFiManager::connect(const std::string& ssid, const std::string& password, WiFiAuthMode auth_mode) {
    if (!m_is_initialized) {
        ESP_LOGE(TAG, "WiFiManager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (ssid.empty()) {
        ESP_LOGE(TAG, "SSID cannot be empty");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid.c_str());
    
    // 保存凭据（用于重连）
    m_last_ssid = ssid;
    m_last_password = password;
    m_user_disconnect = false;
    
    m_info.ssid = ssid;
    update_status(WiFiStatus::CONNECTING);

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password) - 1);
    
    // 设置认证模式
    switch (auth_mode) {
        case WiFiAuthMode::OPEN:
            wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
            break;
        case WiFiAuthMode::WPA3_PSK:
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA3_PSK;
            break;
        case WiFiAuthMode::WPA2_PSK:
        default:
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
            break;
    }

    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        update_status(WiFiStatus::FAILED);
        return ret;
    }

    ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        update_status(WiFiStatus::FAILED);
        ESP_LOGE(TAG, "WiFi connect failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

void WiFiManager::disconnect(bool user_disconnect) {
    if (!m_is_initialized) return;
    
    ESP_LOGI(TAG, "Disconnecting (user_disconnect=%s)", user_disconnect ? "YES" : "NO");
    
    m_user_disconnect = user_disconnect;
    esp_wifi_disconnect();
}

esp_err_t WiFiManager::reconnect() {
    if (!m_is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (m_last_ssid.empty()) {
        ESP_LOGE(TAG, "No previous SSID to reconnect");
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "Reconnecting to: %s", m_last_ssid.c_str());
    m_user_disconnect = false;
    
    return connect(m_last_ssid, m_last_password);
}

// ============================================================================
// 状态查询
// ============================================================================

std::optional<int8_t> WiFiManager::get_rssi() const {
    if (!is_connected()) {
        return std::nullopt;
    }
    return m_info.rssi;
}

std::optional<std::string> WiFiManager::get_ip() const {
    if (!is_connected() || m_info.ip.empty()) {
        return std::nullopt;
    }
    return m_info.ip;
}

// ============================================================================
// 内部方法
// ============================================================================

void WiFiManager::update_status(WiFiStatus new_status) {
    WiFiStatus old_status = m_info.status;
    if (old_status == new_status) return;
    
    m_info.status = new_status;
    ESP_LOGD(TAG, "Status changed: %d -> %d", 
             static_cast<int>(old_status), static_cast<int>(new_status));
    
    if (m_status_callback) {
        m_status_callback(old_status, new_status);
    }
}

void WiFiManager::update_info_from_ap() {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
        return;
    }
    
    m_info.rssi = ap_info.rssi;
    m_info.channel = ap_info.primary;
    m_info.mac = bssid_to_string(ap_info.bssid);
}

void WiFiManager::on_scan_done() {
    if (!m_is_scanning) {
        return;
    }
    m_is_scanning = false;

    // ★★★ 减少数组大小 ★★★
    static wifi_ap_record_t ap_records[15];
    uint16_t count = 15;
    esp_err_t ret = esp_wifi_scan_get_ap_records(&count, ap_records);

    if (ret != ESP_OK || count == 0) {
        update_status(WiFiStatus::SCAN_DONE);
        return;
    }

    // ★★★ 用 map 去重 + 保留最强信号 ★★★
    std::map<std::string, wifi_ap_record_t> best_aps;
    const int8_t RSSI_THRESHOLD = -85;  // 过滤弱信号

    for (uint16_t i = 0; i < count; i++) {
        // 过滤弱信号
        if (ap_records[i].rssi < RSSI_THRESHOLD) {
            continue;
        }

        std::string bssid = bssid_to_string(ap_records[i].bssid);
        auto it = best_aps.find(bssid);
        
        if (it == best_aps.end() || ap_records[i].rssi > it->second.rssi) {
            best_aps[bssid] = ap_records[i];
        }
    }

    // ★★★ 最多保留 10 个 ★★★
    const size_t MAX_RESULT = 10;
    size_t result_count = std::min(best_aps.size(), MAX_RESULT);
    
    m_scan_results.clear();
    m_scan_results.reserve(result_count);

    size_t idx = 0;
    for (const auto& pair : best_aps) {
        if (idx >= MAX_RESULT) break;
        
        const auto& record = pair.second;
        WiFiAPInfo ap;
        ap.ssid = std::string((char*)record.ssid);
        ap.bssid = pair.first;
        ap.rssi = record.rssi;
        ap.channel = record.primary;
        ap.auth_mode = to_wifi_auth_mode(record.authmode);
        m_scan_results.emplace_back(std::move(ap));
        
        // 更新UI
	    eez::Value value = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_WIFI_RECORD_LIST);
	    eez::ArrayValue *array = value.getArray();
	    eez::Value *values = array->values;
	    
	    WiFiRecordValue recordValue = values[idx];
      	recordValue.active(true);
      	recordValue.ssid((const char*) record.ssid);
		recordValue.rssi(ap.rssi);   
        idx++;
    }

    ESP_LOGI(TAG, "Scan done: %d raw, %d unique, %d kept (RSSI > %d)", 
             count, best_aps.size(), m_scan_results.size(), RSSI_THRESHOLD);
    update_status(WiFiStatus::SCAN_DONE);
}

void WiFiManager::on_connected() {
    // 获取 IP 地址
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif) {
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(netif, &ip_info);
        char ip_str[16];
        esp_ip4addr_ntoa(&ip_info.ip, ip_str, sizeof(ip_str));
        m_info.ip = ip_str;
    }
    
    update_info_from_ap();
    update_status(WiFiStatus::CONNECTED);
    
    ESP_LOGI(TAG, "Connected to %s, IP: %s, RSSI: %d dBm",
             m_info.ssid.c_str(), m_info.ip.c_str(), m_info.rssi);
    
    if (m_connect_callback) {
        m_connect_callback(true, m_info);
    }
}

void WiFiManager::on_disconnected(wifi_event_sta_disconnected_t* info) {
    int reason = info ? info->reason : -1;
    ESP_LOGW(TAG, "Disconnected, reason: %d", reason);
    
    // 清除连接信息（保留 ssid）
    m_info.ip.clear();
    m_info.rssi = 0;
    m_info.mac.clear();
    
    // 判断是否需要自动重连
    bool should_auto_reconnect = m_auto_reconnect && 
                                  !m_user_disconnect &&
                                  m_info.status == WiFiStatus::CONNECTED;
    
    update_status(WiFiStatus::DISCONNECTED);
    
    if (should_auto_reconnect && !m_last_ssid.empty()) {
        ESP_LOGI(TAG, "Auto-reconnecting in 3 seconds...");
        // 使用定时器延迟重连（简化版直接调用）
        vTaskDelay(pdMS_TO_TICKS(3000));
        reconnect();
    }
    
    if (m_disconnect_callback) {
        m_disconnect_callback();
    }
}

// ============================================================================
// ESP-IDF 事件处理器（静态）
// ============================================================================

void WiFiManager::event_handler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data) {
    auto& mgr = instance();

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WiFi station started");
                break;
                
            case WIFI_EVENT_STA_CONNECTED: {
                ESP_LOGI(TAG, "WiFi connected to AP");
                mgr.update_status(WiFiStatus::CONNECTING);
                break;
                }
            case WIFI_EVENT_STA_DISCONNECTED: {
                auto* info = static_cast<wifi_event_sta_disconnected_t*>(event_data);
                mgr.on_disconnected(info);
                break;
            }
            case WIFI_EVENT_SCAN_DONE:
                mgr.on_scan_done();
                break;
                
            default:
                break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        mgr.on_connected(); // 连接网络成功
        
		UILoadingData loading_data;
		loading_data.on_complete = [](bool success, void* user_data) {
			eez_flow_pop_screen(LV_SCR_LOAD_ANIM_NONE, 200, 0);
		};
	    EventBus::instance()
		    .publish(
		        UI_EVENT,
		        static_cast<int32_t>(UIEvent::UI_HIDE_LOADING),
		        &loading_data,
		        sizeof(loading_data)
		    );
        
        // 清理WiFi连接相关的内存
        MqttManager::instance().init();
        
    }
}

void WiFiManager::user_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    auto& mgr = instance();
    if (event_base == WIFI_USER_EVENT) {
		switch(event_id) {
			case WIFI_CONNECT: {
				esp_err_t ret = esp_wifi_disconnect();
			   
			    if (ret != ESP_ERR_WIFI_NOT_CONNECT && ret != ESP_OK) {
			        ESP_LOGE(TAG, "❌ 断开连接失败: %s", esp_err_to_name(ret));
			        break;
			    }
			    vTaskDelay(pdMS_TO_TICKS(100)); // 等待断开完成
			    
    			const char* password = static_cast<const char*>(event_data);
                ESP_LOGI("EVENT", "Received WiFi Password: %s", password);
                ESP_LOGI("EVENT", "Password length: %d", strlen(password));

				eez::Value recordIndexValue = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_WIFI_RECORD_INDEX);
                WiFiAPInfo ap = WiFiManager::instance().get_scan_results()[recordIndexValue.getInt32()];
                ESP_LOGI("EVENT", "Received record index: %s", ap.ssid.c_str());
                
                WiFiManager::instance().connect(ap.ssid, password, ap.auth_mode);
				break;
			}
			default:
				break;
		}        
    }
}
