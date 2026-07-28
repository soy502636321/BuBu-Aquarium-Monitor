/*
 * data_receiver.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_RECEIVER_HPP_
#define COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_RECEIVER_HPP_


#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include "freertos/FreeRTOS.h"
#include "protocol_base.hpp"

class DataReceiver {
public:
    static DataReceiver& getInstance() {
        static DataReceiver instance;
        return instance;
    }
    
    // 禁止拷贝
    DataReceiver(const DataReceiver&) = delete;
    DataReceiver& operator=(const DataReceiver&) = delete;
    
    // 注册协议
    bool registerProtocol(std::unique_ptr<ProtocolBase> protocol);
    
    // 启动所有协议
    bool startAll();
    
    // 停止所有协议
    void stopAll();
    
    // 获取协议数量
    size_t getProtocolCount() const { return m_protocols.size(); }
    
    // 根据类型获取协议
    ProtocolBase* getProtocol(ProtocolType type);
    
    // 设置数据回调（转发给解析器）
    void setDataCallback(RawDataCallback callback);
    
private:
    DataReceiver() = default;
    ~DataReceiver() = default;
    
    // 数据分发：将收到的数据转发给所有注册的解析器
    void onDataReceived(const std::vector<uint8_t>& data);
    
private:
    std::vector<std::unique_ptr<ProtocolBase>> m_protocols;
    RawDataCallback m_callback;
    mutable std::mutex m_mutex;
    bool m_running = false;
};

#endif /* COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_RECEIVER_HPP_ */
