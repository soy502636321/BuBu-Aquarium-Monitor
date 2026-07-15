#pragma once

#include "esp_event.h"


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