/*
 * ble_protocol.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

// components/data_gateway/src/ble_protocol.cpp
#include "protocol_base.hpp"
#include "esp_log.h"

static const char* TAG = "BLEProtocol";

class BLEProtocol : public ProtocolBase {
public:
    BLEProtocol() = default;
    ~BLEProtocol() override { stop(); }
    
    bool init() override {
        // 这里实现 BLE 初始化
        // 包括: bt_controller_init, bluedroid_init, gatts_init 等
        ESP_LOGI(TAG, "BLE protocol initialized");
        return true;
    }
    
    bool start() override {
        if (m_running) return true;
        
        // 开始 BLE 广播
        m_running = true;
        ESP_LOGI(TAG, "BLE protocol started");
        return true;
    }
    
    void stop() override {
        m_running = false;
        ESP_LOGI(TAG, "BLE protocol stopped");
    }
    
    bool sendCommand(const std::vector<uint8_t>& data) override {
        // 通过 BLE GATT 发送数据
        ESP_LOGI(TAG, "Sending BLE command, %zu bytes", data.size());
        return true;
    }
    
    ProtocolType getType() const override { return ProtocolType::BLE; }
    std::string getName() const override { return "BLE"; }
    bool isRunning() const override { return m_running; }
    
    // 外部调用：当 BLE 收到数据时
    void onDataReceived(const std::vector<uint8_t>& data) {
        if (m_callback) {
            m_callback(data);
        }
    }
    
private:
    bool m_running = false;
};


