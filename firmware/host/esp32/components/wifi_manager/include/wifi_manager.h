#pragma once

#include <string>
#include <functional>
#include <optional>
#include <vector>
#include <cstdint>

// ESP-IDF 头文件（必须按顺序包含）
#include "esp_err.h"
#include "esp_wifi.h"        // 提供 wifi_event_sta_disconnected_t, wifi_auth_mode_t
#include "esp_event.h"       // 提供 esp_event_base_t
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"       // 提供 esp_netif_t

// ============================================================================
// 枚举定义（enum class 类型安全）
// ============================================================================

enum class WiFiStatus : uint8_t {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    SCANNING,
    SCAN_DONE,
    FAILED
};

enum class WiFiAuthMode : uint8_t {
    OPEN = 0,
    WPA2_PSK,
    WPA3_PSK,
    WPA2_ENTERPRISE,
    WPA3_ENTERPRISE
};

// ============================================================================
// 数据结构
// ============================================================================

struct WiFiInfo {
    WiFiStatus status = WiFiStatus::DISCONNECTED;
    std::string ssid;
    std::string ip;
    std::string mac;
    int8_t rssi = 0;
    uint8_t channel = 0;
    
    bool is_connected() const { return status == WiFiStatus::CONNECTED; }
    bool has_ip() const { return !ip.empty(); }
    
    void clear() {
        status = WiFiStatus::DISCONNECTED;
        ssid.clear();
        ip.clear();
        mac.clear();
        rssi = 0;
        channel = 0;
    }
};

struct WiFiAPInfo {
    std::string ssid;
    std::string bssid;
    int8_t rssi = 0;
    uint8_t channel = 0;
    WiFiAuthMode auth_mode = WiFiAuthMode::OPEN;
    
    bool is_encrypted() const { return auth_mode != WiFiAuthMode::OPEN; }
    bool is_wpa2() const {
        return auth_mode == WiFiAuthMode::WPA2_PSK ||
               auth_mode == WiFiAuthMode::WPA2_ENTERPRISE;
    }
    bool is_wpa3() const {
        return auth_mode == WiFiAuthMode::WPA3_PSK ||
               auth_mode == WiFiAuthMode::WPA3_ENTERPRISE;
    }
};

// ============================================================================
// WiFi 管理器类（单例模式）
// ============================================================================

class WiFiManager {
public:
    // 回调函数类型
    using StatusCallback = std::function<void(WiFiStatus old_status, WiFiStatus new_status)>;
    using ConnectCallback = std::function<void(bool success, const WiFiInfo& info)>;
    using ScanCallback = std::function<void(const std::vector<WiFiAPInfo>& aps)>;
    using DisconnectCallback = std::function<void()>;

    // ----------------------------------------------------------------
    // 单例访问（唯一公开的获取方式）
    // ----------------------------------------------------------------
    static WiFiManager& instance();

    // 禁止拷贝和赋值
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;

    // ----------------------------------------------------------------
    // 生命周期
    // ----------------------------------------------------------------
    
    /**
     * @brief 初始化 WiFi 管理器
     * @param enable_auto_reconnect 是否启用自动重连
     * @return ESP_OK 成功
     */
    esp_err_t init(bool enable_auto_reconnect = true);
    
    /**
     * @brief 反初始化
     */
    void deinit();
    
    /**
     * @brief 是否已初始化
     */
    bool is_initialized() const { return m_is_initialized; }

    // ----------------------------------------------------------------
    // 扫描
    // ----------------------------------------------------------------
    
    /**
     * @brief 异步扫描 WiFi
     * @param scan_time_ms 扫描时间（毫秒），默认300ms
     * @param show_hidden 是否扫描隐藏网络
     * @return ESP_OK 成功
     */
    esp_err_t scan_start(uint16_t scan_time_ms = 300, bool show_hidden = true);
    
    /**
     * @brief 获取扫描结果（同步，需等待扫描完成）
     */
    std::vector<WiFiAPInfo> get_scan_results() const;

    // ----------------------------------------------------------------
    // 连接管理
    // ----------------------------------------------------------------
    
    /**
     * @brief 连接到 WiFi
     * @param ssid WiFi 名称
     * @param password WiFi 密码（可为空）
     * @param auth_mode 认证模式（默认WPA2）
     * @return ESP_OK 成功
     */
    esp_err_t connect(const std::string& ssid, 
                      const std::string& password = "",
                      WiFiAuthMode auth_mode = WiFiAuthMode::WPA2_PSK);
    
    /**
     * @brief 断开连接
     * @param user_disconnect 是否为用户主动断开（影响自动重连行为）
     */
    void disconnect(bool user_disconnect = true);
    
    /**
     * @brief 重新连接上次的 WiFi
     */
    esp_err_t reconnect();

    // ----------------------------------------------------------------
    // 状态查询
    // ----------------------------------------------------------------
    
    WiFiInfo get_info() const { return m_info; }
    WiFiStatus get_status() const { return m_info.status; }
    bool is_connected() const { return m_info.status == WiFiStatus::CONNECTED; }
    std::optional<int8_t> get_rssi() const;
    std::optional<std::string> get_ip() const;

    // ----------------------------------------------------------------
    // 回调注册
    // ----------------------------------------------------------------
    
    void set_status_callback(StatusCallback callback) { m_status_callback = std::move(callback); }
    void set_connect_callback(ConnectCallback callback) { m_connect_callback = std::move(callback); }
    void set_scan_callback(ScanCallback callback) { m_scan_callback = std::move(callback); }
    void set_disconnect_callback(DisconnectCallback callback) { m_disconnect_callback = std::move(callback); }

private:
    // ----------------------------------------------------------------
    // 私有构造/析构（单例）
    // ----------------------------------------------------------------
    WiFiManager() = default;
    ~WiFiManager() = default;

    // ----------------------------------------------------------------
    // 内部状态
    // ----------------------------------------------------------------
    
    WiFiInfo m_info;
    std::string m_last_ssid;
    std::string m_last_password;
    std::vector<WiFiAPInfo> m_scan_results;
    
    bool m_is_initialized = false;
    bool m_is_scanning = false;
    bool m_auto_reconnect = true;
    bool m_user_disconnect = false;

    // 回调
    StatusCallback m_status_callback = nullptr;
    ConnectCallback m_connect_callback = nullptr;
    ScanCallback m_scan_callback = nullptr;
    DisconnectCallback m_disconnect_callback = nullptr;

    // ----------------------------------------------------------------
    // 内部方法
    // ----------------------------------------------------------------
    
    void update_status(WiFiStatus new_status);
    void update_info_from_ap();
    void on_scan_done();
    void on_connected();
    void on_disconnected(wifi_event_sta_disconnected_t* info);
    
    // ESP-IDF 事件处理器（静态）
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    // 自定义事件处理
    static void user_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data);
};

