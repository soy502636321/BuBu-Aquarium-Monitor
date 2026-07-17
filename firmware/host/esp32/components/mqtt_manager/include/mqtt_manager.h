// mqtt_manager.h
#pragma once

#include <string>
#include "mqtt_client.h"
#include "esp_event.h"

// ============================================================================
// ★★★ MQTT 管理器（单例）★★★
// ============================================================================

class MqttManager {
public:
    // -------- 单例 --------
    static MqttManager& instance();

    // 禁止拷贝
    MqttManager(const MqttManager&) = delete;
    MqttManager& operator=(const MqttManager&) = delete;

    // ========================================================================
    // ★★★ 生命周期 ★★★
    // ========================================================================

    /**
     * @brief 初始化 MQTT 客户端
     * @param broker Broker 地址，如 "mqtt://broker.emqx.io"
     * @return true=成功, false=失败
     */
    bool init(const std::string& broker = "");

    /**
     * @brief 设置设备信息（用于自动订阅）
     * @param boardId 设备 ID
     * @param userId 用户 ID
     */
    void setDeviceInfo(const std::string& boardId, const std::string& userId);

    // ========================================================================
    // ★★★ 发布/订阅 ★★★
    // ========================================================================

    /**
     * @brief 发布消息
     * @param topic 主题
     * @param payload 消息内容
     * @param qos QoS 等级 (0, 1, 2)
     * @return true=成功, false=失败
     */
    bool publish(const std::string& topic, const std::string& payload, int qos = 1);

    /**
     * @brief 订阅主题
     * @param topic 主题
     * @param qos QoS 等级 (0, 1, 2)
     * @return true=成功, false=失败
     */
    bool subscribe(const std::string& topic, int qos = 1);

    // ========================================================================
    // ★★★ 状态查询 ★★★
    // ========================================================================

    /**
     * @brief 获取 MQTT 连接状态
     * @return true=已连接, false=未连接
     */
    bool connected() const;

    /**
     * @brief 获取设备 ID
     */
    const std::string& getBoardId() const { return boardId_; }

    /**
     * @brief 获取用户 ID
     */
    const std::string& getUserId() const { return userId_; }

private:
    // -------- 私有构造/析构 --------
    MqttManager();
    ~MqttManager();

    // -------- 事件处理器（静态） --------
    static void eventHandler(void* handler_args,
                             esp_event_base_t base,
                             int32_t event_id,
                             void* event_data);

    // -------- 成员变量 --------
    esp_mqtt_client_handle_t client_ = nullptr;
    bool connected_ = false;
    std::string boardId_;
    std::string userId_;
};
