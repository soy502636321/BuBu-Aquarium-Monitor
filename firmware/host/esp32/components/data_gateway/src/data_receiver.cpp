/*
 * data_receiver.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

// components/data_gateway/src/data_receiver.cpp
#include "data_receiver.hpp"
#include "esp_log.h"

static const char* TAG = "DataReceiver";

bool DataReceiver::registerProtocol(std::unique_ptr<ProtocolBase> protocol) {
    if (protocol == nullptr) {
        ESP_LOGE(TAG, "Cannot register null protocol");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // 检查是否已存在同类型协议
    for (const auto& p : m_protocols) {
        if (p->getType() == protocol->getType()) {
            ESP_LOGW(TAG, "Protocol %s already registered", p->getName().c_str());
            return false;
        }
    }
    
    // 设置数据回调
    protocol->setDataCallback([this](const std::vector<uint8_t>& data) {
        this->onDataReceived(data);
    });
    
    // 初始化协议
    if (!protocol->init()) {
        ESP_LOGE(TAG, "Failed to init protocol: %s", protocol->getName().c_str());
        return false;
    }
    
    m_protocols.push_back(std::move(protocol));
    ESP_LOGI(TAG, "Protocol registered, total: %zu", m_protocols.size());
    return true;
}

bool DataReceiver::startAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_running) {
        ESP_LOGW(TAG, "Receiver already running");
        return true;
    }
    
    bool all_started = true;
    for (auto& protocol : m_protocols) {
        if (!protocol->start()) {
            ESP_LOGE(TAG, "Failed to start protocol: %s", protocol->getName().c_str());
            all_started = false;
        } else {
            ESP_LOGI(TAG, "Protocol %s started", protocol->getName().c_str());
        }
    }
    
    if (all_started) {
        m_running = true;
        ESP_LOGI(TAG, "All protocols started");
    }
    
    return all_started;
}

void DataReceiver::stopAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& protocol : m_protocols) {
        protocol->stop();
        ESP_LOGI(TAG, "Protocol %s stopped", protocol->getName().c_str());
    }
    
    m_running = false;
    ESP_LOGI(TAG, "All protocols stopped");
}

ProtocolBase* DataReceiver::getProtocol(ProtocolType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& protocol : m_protocols) {
        if (protocol->getType() == type) {
            return protocol.get();
        }
    }
    return nullptr;
}

void DataReceiver::setDataCallback(RawDataCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = callback;
}

void DataReceiver::onDataReceived(const std::vector<uint8_t>& data) {
    if (m_callback) {
        m_callback(data);
    }
}


