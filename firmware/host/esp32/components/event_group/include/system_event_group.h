/*
 * system_event_group.h
 *
 *  Created on: 2026年7月21日
 *      Author: Hu
 */

#ifndef COMPONENTS_EVENT_GROUP_INCLUDE_SYSTEM_EVENT_GROUP_H_
#define COMPONENTS_EVENT_GROUP_INCLUDE_SYSTEM_EVENT_GROUP_H_

#include <stdint.h>                    // 提供 uint32_t 等类型
#include "sdkconfig.h"                 // ESP-IDF 配置
#include "freertos/FreeRTOS.h"         // FreeRTOS 基础
#include "freertos/event_groups.h"     // 事件组
#include <functional>
#include <vector>
#include <mutex>

class SystemEventGroup {
public:
    // ============================================================
    // 事件位定义
    // ============================================================
    static constexpr EventBits_t EVT_WIFI_CONNECTED = (1 << 0);
    static constexpr EventBits_t EVT_WIFI_GOT_IP    = (1 << 1);
    static constexpr EventBits_t EVT_WIFI_FAILED    = (1 << 2);
    static constexpr EventBits_t EVT_NTP_SYNCED     = (1 << 3);
    static constexpr EventBits_t EVT_SENSOR_READY   = (1 << 4);
    static constexpr EventBits_t EVT_UI_READY       = (1 << 5);
    static constexpr EventBits_t EVT_SYSTEM_READY   = (1 << 6);
    
    // 组合
    static constexpr EventBits_t EVT_ALL_READY = 
        EVT_WIFI_GOT_IP | EVT_NTP_SYNCED | EVT_SENSOR_READY | EVT_UI_READY;
    
    // ============================================================
    // 单例
    // ============================================================
    static SystemEventGroup& instance();
    
    // ============================================================
    // 初始化
    // ============================================================
    void init();
    
    // ============================================================
    // 设置事件
    // ============================================================
    void set(EventBits_t bits);
    
    // ============================================================
    // 清除事件
    // ============================================================
    void clear(EventBits_t bits);
    
    // ============================================================
    // 注册监听
    // ============================================================
    void on(EventBits_t bits, std::function<void()> callback);
    void on(EventBits_t bits, std::function<void(EventBits_t)> callback);
    
    // ============================================================
    // 解除监听
    // ============================================================
    void off(EventBits_t bits);
    void offAll();
    
    // ============================================================
    // 检查状态
    // ============================================================
    bool isSet(EventBits_t bits);
    EventBits_t get();
    
    // ============================================================
    // 等待（阻塞）
    // ============================================================
    bool wait(EventBits_t bits, TickType_t timeout = portMAX_DELAY);
    
private:
    SystemEventGroup();
    ~SystemEventGroup();
    
    SystemEventGroup(const SystemEventGroup&) = delete;
    SystemEventGroup& operator=(const SystemEventGroup&) = delete;
    
    void notify(EventBits_t bits);
    
    struct CB {
        EventBits_t bits;
        std::function<void()> cb;
    };
    
    struct CBWithBits {
        EventBits_t bits;
        std::function<void(EventBits_t)> cb;
    };
    
    EventGroupHandle_t m_group;
    std::mutex m_mutex;
    std::vector<CB> m_callbacks;
    std::vector<CBWithBits> m_callbacks_with_bits;
};

#endif /* COMPONENTS_EVENT_GROUP_INCLUDE_SYSTEM_EVENT_GROUP_H_ */
