/*
 * mqtt_topic.h
 *
 *  Created on: 2026年7月17日
 *      Author: Hu
 */

#ifndef COMPONENTS_MQTT_MANAGER_INCLUDE_MQTT_TOPIC_H_
#define COMPONENTS_MQTT_MANAGER_INCLUDE_MQTT_TOPIC_H_

// mqtt_topic.h
#pragma once

#include <string>
#include <cstdio>
#include "host_info.h"

// ============================================================================
// ★★★ MQTT 主题配置 ★★★
// ============================================================================

struct MqttTopicConfig {
    // -------- 主题模板（使用 %s 占位符）--------
    // 格式: 项目名/用户ID/设备ID/类型
    static constexpr const char* TEMPLATE_DATA      = "BuBu-Aquarium-Monitor/%s/data";
    static constexpr const char* TEMPLATE_HEARTBEAT = "BuBu-Aquarium-Monitor/%s/heartbeat";
    static constexpr const char* TEMPLATE_STATUS    = "BuBu-Aquarium-Monitor/%s/%s/status";
    static constexpr const char* TEMPLATE_COMMAND   = "BuBu-Aquarium-Monitor/%s/command";
    static constexpr const char* TEMPLATE_EVENT     = "BuBu-Aquarium-Monitor/%s/%s/event";
    static constexpr const char* TEMPLATE_RESPONSE  = "BuBu-Aquarium-Monitor/%s/%s/response";
    static constexpr const char* TEMPLATE_CONFIG    = "BuBu-Aquarium-Monitor/%s/%s/config";
    static constexpr const char* TEMPLATE_OTA       = "BuBu-Aquarium-Monitor/%s/ota";
    
    // -------- QoS 配置 --------
    static constexpr int QOS_DATA     = 0;   // 传感器数据，允许丢失
    static constexpr int QOS_HEARTBEAT     = 0;   // 传感器数据，允许丢失
    static constexpr int QOS_STATUS   = 1;   // 状态上报，确保送达
    static constexpr int QOS_COMMAND  = 1;   // 控制指令，确保送达
    static constexpr int QOS_EVENT    = 1;   // 事件通知，确保送达
    static constexpr int QOS_CONFIG   = 2;   // 配置更新，恰好一次
    static constexpr int QOS_OTA      = 2;   // OTA 升级，恰好一次
    
    // -------- 保留消息 --------
    static constexpr bool RETAIN_STATUS  = true;   // 状态保留
    static constexpr bool RETAIN_DATA    = false;  // 数据不保留
    static constexpr bool RETAIN_COMMAND = false;  // 指令不保留
    
    // ========================================================================
    // ★★★ 工具函数：生成主题 ★★★
    // ========================================================================
    
    /**
     * @brief 生成心跳上报主题
     */
    static std::string makeHeartbeatTopic() {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_HEARTBEAT, HostInfo::instance().getHostId().c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成数据上报主题
     */
    static std::string makeDataTopic() {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_DATA, HostInfo::instance().getHostId().c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成状态上报主题
     */
    static std::string makeStatusTopic(const std::string& userId, const std::string& boardId) {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_STATUS, userId.c_str(), boardId.c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成控制指令主题（需要订阅）
     */
    static std::string makeCommandTopic() {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_COMMAND, HostInfo::instance().getHostId().c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成事件主题
     */
    static std::string makeEventTopic(const std::string& userId, const std::string& boardId) {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_EVENT, userId.c_str(), boardId.c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成响应主题
     */
    static std::string makeResponseTopic(const std::string& userId, const std::string& boardId) {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_RESPONSE, userId.c_str(), boardId.c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成配置主题
     */
    static std::string makeConfigTopic(const std::string& userId, const std::string& boardId) {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_CONFIG, userId.c_str(), boardId.c_str());
        return std::string(topic);
    }
    
    /**
     * @brief 生成 OTA 主题
     */
    static std::string makeOtaTopic() {
        char topic[128];
        snprintf(topic, sizeof(topic), TEMPLATE_OTA, HostInfo::instance().getHostId().c_str());
        return std::string(topic);
    }
};

#endif /* COMPONENTS_MQTT_MANAGER_INCLUDE_MQTT_TOPIC_H_ */
