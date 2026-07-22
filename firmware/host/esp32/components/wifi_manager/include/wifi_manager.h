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

// ============ 标准 802.11 原因码 (1-24) ============
#define WIFI_REASON_UNSPECIFIED              1   // 未知原因
#define WIFI_REASON_AUTH_EXPIRE              2   // 认证过期
#define WIFI_REASON_AUTH_LEAVE               3   // 发起方取消认证
#define WIFI_REASON_ASSOC_EXPIRE             4   // 关联过期
#define WIFI_REASON_ASSOC_TOOMANY            5   // 关联数过多
#define WIFI_REASON_NOT_AUTHED               6   // 未认证
#define WIFI_REASON_NOT_ASSOCED              7   // 未关联
#define WIFI_REASON_ASSOC_LEAVE              8   // 发起方取消关联
#define WIFI_REASON_ASSOC_NOT_AUTHED         9   // 关联但未认证
#define WIFI_REASON_DISASSOC_PWRCAP          10  // 功率不足断开
#define WIFI_REASON_DISASSOC_SUPCHAN         11  // 信道不支持
#define WIFI_REASON_IE_INVALID               13  // IE无效
#define WIFI_REASON_MIC_FAILURE              14  // MIC校验失败（密码错误）
#define WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   15  // 4次握手超时（密码错误）
#define WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT 16  // 组密钥更新超时
#define WIFI_REASON_IE_IN_4WAY_DIFFERS       17  // IE在4次握手中不匹配
#define WIFI_REASON_GROUP_CIPHER_INVALID     18  // 组加密方式无效
#define WIFI_REASON_PAIRWISE_CIPHER_INVALID  19  // 点对点加密无效
#define WIFI_REASON_AKMP_INVALID             20  // AKMP无效
#define WIFI_REASON_UNSUPP_RSN_IE_VERSION    21  // RSN版本不支持
#define WIFI_REASON_INVALID_RSN_IE_CAP       22  // RSN能力无效
#define WIFI_REASON_802_1X_AUTH_FAILED       23  // 802.1X认证失败
#define WIFI_REASON_CIPHER_SUITE_REJECTED    24  // 加密套件被拒绝

// ============ ESP32 自定义原因码 (200+) ============
#define WIFI_REASON_BEACON_TIMEOUT           200 // 信标超时（信号弱）
#define WIFI_REASON_NO_AP_FOUND              201 // 未找到AP
#define WIFI_REASON_AUTH_FAIL                202 // 认证失败（密码错误）
#define WIFI_REASON_ASSOC_FAIL               203 // 关联失败
#define WIFI_REASON_HANDSHAKE_TIMEOUT        204 // 握手超时
#define WIFI_REASON_CRYPTO_INIT_FAIL         214 // 加密初始化失败

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
    
    esp_err_t scan_stop();
    
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
    
    // 错误信息（最简单）
    std::string getErrorMessage(int reason);

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

