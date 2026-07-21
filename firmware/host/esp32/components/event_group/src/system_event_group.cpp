/*
 * system_event.cpp
 *
 *  Created on: 2026年7月21日
 *      Author: Hu
 */
// system_event_group.cpp
#include "system_event_group.h"
#include "esp_log.h"
#include "freertos/event_groups.h"   // ✅ 必须包含
#include <algorithm>                  // ✅ 用于 std::remove_if


static const char* TAG = "BuBu-Aquarium-Monitor[system_event_group]";

// ============================================================
// 单例
// ============================================================
SystemEventGroup& SystemEventGroup::instance() {
    static SystemEventGroup instance;
    return instance;
}

// ============================================================
// 构造/析构
// ============================================================
SystemEventGroup::SystemEventGroup() : m_group(NULL) {}

SystemEventGroup::~SystemEventGroup() {
    if (m_group) {
        vEventGroupDelete(m_group);
    }
}

// ============================================================
// 初始化
// ============================================================
void SystemEventGroup::init() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_group) {
        m_group = xEventGroupCreate();
        ESP_LOGI(TAG, "✅ 初始化成功");
    }
}

// ============================================================
// 设置事件
// ============================================================
void SystemEventGroup::set(EventBits_t bits) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_group) {
        xEventGroupSetBits(m_group, bits);
        notify(bits);
    }
}

// ============================================================
// 清除事件
// ============================================================
void SystemEventGroup::clear(EventBits_t bits) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_group) {
        xEventGroupClearBits(m_group, bits);
    }
}

// ============================================================
// 注册监听（无参数）
// ============================================================
void SystemEventGroup::on(EventBits_t bits, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callbacks.push_back({bits, callback});
    
    // 如果已经触发，立即执行
    if (m_group && (xEventGroupGetBits(m_group) & bits)) {
        callback();
    }
}

// ============================================================
// 注册监听（带参数）
// ============================================================
void SystemEventGroup::on(EventBits_t bits, std::function<void(EventBits_t)> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callbacks_with_bits.push_back({bits, callback});
    
    if (m_group) {
        EventBits_t current = xEventGroupGetBits(m_group);
        if (current & bits) {
            callback(current & bits);
        }
    }
}

// ============================================================
// 解除监听
// ============================================================
void SystemEventGroup::off(EventBits_t bits) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_callbacks.erase(
        std::remove_if(m_callbacks.begin(), m_callbacks.end(),
            [bits](const CB& cb) { return cb.bits == bits; }),
        m_callbacks.end()
    );
    
    m_callbacks_with_bits.erase(
        std::remove_if(m_callbacks_with_bits.begin(), m_callbacks_with_bits.end(),
            [bits](const CBWithBits& cb) { return cb.bits == bits; }),
        m_callbacks_with_bits.end()
    );
}

// ============================================================
// 解除所有
// ============================================================
void SystemEventGroup::offAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callbacks.clear();
    m_callbacks_with_bits.clear();
}

// ============================================================
// 检查状态
// ============================================================
bool SystemEventGroup::isSet(EventBits_t bits) {
    if (!m_group) return false;
    return (xEventGroupGetBits(m_group) & bits) == bits;
}

EventBits_t SystemEventGroup::get() {
    if (!m_group) return 0;
    return xEventGroupGetBits(m_group);
}

// ============================================================
// 等待
// ============================================================
bool SystemEventGroup::wait(EventBits_t bits, TickType_t timeout) {
    if (!m_group) return false;
    EventBits_t result = xEventGroupWaitBits(m_group, bits, pdFALSE, pdTRUE, timeout);
    return (result & bits) == bits;
}

// ============================================================
// 内部：触发回调
// ============================================================
void SystemEventGroup::notify(EventBits_t bits) {
    for (const auto& cb : m_callbacks) {
        if (bits & cb.bits) {
            cb.cb();
        }
    }
    for (const auto& cb : m_callbacks_with_bits) {
        if (bits & cb.bits) {
            cb.cb(bits & cb.bits);
        }
    }
}

