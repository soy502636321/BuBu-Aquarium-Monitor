/*
 * data_gateway.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */
// components/data_gateway/src/data_gateway.cpp
#include "data_gateway.hpp"
#include "esp_log.h"

static const char* TAG = "DataGateway";

bool DataGateway::init(const DataGatewayConfig& config) {
    if (m_running) {
        ESP_LOGW(TAG, "Gateway already initialized");
        return true;
    }
    
    m_config = config;
    
    // 设置解析器的回调（默认空，由外部设置）
    // m_parser.setRecordCallback(...);
    
    // 注册数据接收器的回调
    m_receiver.setDataCallback([this](const std::vector<uint8_t>& data) {
        this->onDataReceived(data);
    });
    
    ESP_LOGI(TAG, "DataGateway initialized");
    return true;
}

void DataGateway::deinit() {
    stop();
    ESP_LOGI(TAG, "DataGateway deinitialized");
}

bool DataGateway::start() {
    if (m_running) {
        ESP_LOGW(TAG, "Gateway already running");
        return true;
    }
    
    if (!m_receiver.startAll()) {
        m_last_error = "Failed to start receiver";
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    m_running = true;
    ESP_LOGI(TAG, "DataGateway started");
    return true;
}

void DataGateway::stop() {
    if (!m_running) {
        return;
    }
    
    m_receiver.stopAll();
    m_running = false;
    ESP_LOGI(TAG, "DataGateway stopped");
}

bool DataGateway::registerProtocol(std::unique_ptr<ProtocolBase> protocol) {
    return m_receiver.registerProtocol(std::move(protocol));
}

ProtocolBase* DataGateway::getProtocol(ProtocolType type) {
    return m_receiver.getProtocol(type);
}

bool DataGateway::sendCommand(const std::string& device_id, const std::vector<uint8_t>& data) {
    // 根据设备ID找到对应的协议
    // 简化实现：发送到第一个可用的协议
    if (m_receiver.getProtocolCount() == 0) {
        m_last_error = "No protocol available";
        return false;
    }
    
    auto* protocol = m_receiver.getProtocol(ProtocolType::TTL_SERIAL);
    if (protocol == nullptr) {
        m_last_error = "No TTL protocol available";
        return false;
    }
    
    return protocol->sendCommand(data);
}

bool DataGateway::sendCommand(ProtocolType type, const std::vector<uint8_t>& data) {
    auto* protocol = m_receiver.getProtocol(type);
    if (protocol == nullptr) {
        m_last_error = "Protocol not found";
        return false;
    }
    
    return protocol->sendCommand(data);
}

void DataGateway::onDataReceived(const std::vector<uint8_t>& data) {
    // 转发给解析器
    m_parser.parseRawData(data);
}

std::string DataGateway::getStatus() const {
    std::string status = "DataGateway: ";
    status += m_running ? "RUNNING" : "STOPPED";
    status += ", protocols: " + std::to_string(m_receiver.getProtocolCount());
    return status;
}

size_t DataGateway::getProtocolCount() const {
    return m_receiver.getProtocolCount();
}



