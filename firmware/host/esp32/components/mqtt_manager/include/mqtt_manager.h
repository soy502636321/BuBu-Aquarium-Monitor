#pragma once

#include <string>
#include "mqtt_client.h"


class MqttManager
{

public:

    static MqttManager& instance();


    /**
     * 初始化MQTT
     */
    bool init(
        const std::string& broker
    );


    /**
     * 设置设备信息
     */
    void setDeviceInfo(
        const std::string& boardId,
        const std::string& userId
    );


    /**
     * 发布消息
     */
    bool publish(
        const std::string& topic,
        const std::string& payload,
        int qos = 1
    );


    /**
     * 订阅
     */
    bool subscribe(
        const std::string& topic,
        int qos = 1
    );


    /**
     * MQTT状态
     */
    bool connected() const;



private:


    MqttManager();
    ~MqttManager();


    //禁止复制
    MqttManager(
        const MqttManager&
    ) = delete;


    MqttManager& operator=(
        const MqttManager&
    ) = delete;



private:


    static void eventHandler(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data
    );



private:


    esp_mqtt_client_handle_t client_ = nullptr;


    bool connected_ = false;


    std::string boardId_;

    std::string userId_;

};