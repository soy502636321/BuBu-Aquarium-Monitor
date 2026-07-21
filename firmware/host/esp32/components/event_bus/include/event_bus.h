#pragma once

#include "esp_event.h"
#include <functional>
#include <atomic>
#include <memory>

using EventCallback = std::function<void(bool success, void* callback_data)>;

/**
 * 所有事件数据的基类
 * 
 * 用法：所有事件数据结构都继承这个类，这样就可以在 publish 中统一处理回调
 * 
 * 示例：
 * struct MyEventData : public EventData {
 *     int value;
 *     std::string message;
 * };
 */
 
class EventData {
public:
	EventCallback on_complete;  // 完成回调
	void* user_data;             // 用户自定义数据
	std::atomic<bool> executed; // 防止重复执行
	
	EventData() : on_complete(nullptr) {
        executed = false;
    }
    
    /**
     * @brief 【关键】带回调的构造函数
     * @param cb 回调函数
     * @param ud 用户数据
     */
    EventData(EventCallback cb, void* ud = nullptr) 
        : on_complete(cb), user_data(ud) {
        executed = false;
    }
    
    virtual ~EventData() = default;
    
    // 禁止拷贝（避免意外复制）
    EventData(const EventData&) = delete;
    EventData& operator=(const EventData&) = delete;
    
    /**
     * @brief 检查是否有回调
     */
    bool has_callback() const {
        return on_complete != nullptr;
    }
    
    /**
     * @brief 执行回调（线程安全，只会执行一次）
     * @param success 操作结果
     */
    void execute_callback(bool success) {
        bool expected = false;
        // 原子操作：只有第一次调用才会执行
        if (executed.compare_exchange_strong(expected, true)) {
            if (on_complete) {
                on_complete(success, nullptr);
            }
        }
    }
    
    /**
     * @brief 重置执行状态（用于重新发布同一个事件）
     */
    void reset() {
        executed = false;
    }
};

class EventBus
{

public:

    static EventBus& instance();


    /**
     * 初始化事件系统
     */
    esp_err_t init();


    /**
     * 发布事件
     */
    esp_err_t publish(
        esp_event_base_t base,
        int32_t id,
        const void *data,
        size_t size
    );


    /**
     * 注册事件监听
     */
    esp_err_t subscribe(
        esp_event_base_t base,
        int32_t id,
        esp_event_handler_t handler,
        void *arg = nullptr
    );


    /**
     * 注销事件监听
     */
    esp_err_t unsubscribe(
        esp_event_base_t base,
        int32_t id,
        esp_event_handler_t handler
    );


private:

    EventBus() = default;

    bool initialized = false;

};