#include "mqtt_manager.h"

#include "esp_log.h"

static const char* TAG="MqttManager";

MqttManager& MqttManager::instance()
{
    static MqttManager obj;
    return obj;
}



MqttManager::MqttManager()
{

}



MqttManager::~MqttManager()
{

    if(client_)
    {
        esp_mqtt_client_stop(client_);

        esp_mqtt_client_destroy(client_);
    }

}



bool MqttManager::init(
        const std::string& broker)
{

    esp_mqtt_client_config_t cfg{};


    cfg.broker.address.uri =
        broker.c_str();



    client_ =
        esp_mqtt_client_init(
            &cfg
        );


    if(!client_)
    {
        return false;
    }



    esp_mqtt_client_register_event(
        client_,
        MQTT_EVENT_ANY,
        eventHandler,
        this
    );



    return
        esp_mqtt_client_start(
            client_
        )
        == ESP_OK;

}



void MqttManager::setDeviceInfo(
        const std::string& boardId,
        const std::string& userId)
{

    boardId_=boardId;

    userId_=userId;

}



bool MqttManager::connected() const
{

    return connected_;

}



bool MqttManager::publish(
        const std::string& topic,
        const std::string& payload,
        int qos)
{


    if(!connected_)
        return false;



    int id =
        esp_mqtt_client_publish(
            client_,
            topic.c_str(),
            payload.c_str(),
            0,
            qos,
            0
        );


    return id>=0;

}




bool MqttManager::subscribe(
        const std::string& topic,
        int qos)
{


    if(!connected_)
        return false;


    int id =
        esp_mqtt_client_subscribe(
            client_,
            topic.c_str(),
            qos
        );


    return id>=0;

}



void MqttManager::eventHandler(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data)
{


    auto self =
        static_cast<MqttManager*>(
            handler_args
        );


    auto event =
        static_cast<esp_mqtt_event_handle_t>(
            event_data
        );



    switch(event_id)
    {


    case MQTT_EVENT_CONNECTED:

        self->connected_=true;


        ESP_LOGI(
            TAG,
            "MQTT Connected"
        );


        break;



    case MQTT_EVENT_DISCONNECTED:

        self->connected_=false;


        ESP_LOGW(
            TAG,
            "MQTT Disconnected"
        );


        break;



    case MQTT_EVENT_DATA:

        ESP_LOGI(
            TAG,
            "RX topic:%.*s",
            event->topic_len,
            event->topic
        );


        ESP_LOGI(
            TAG,
            "RX data:%.*s",
            event->data_len,
            event->data
        );


        break;


    default:

        break;

    }

}