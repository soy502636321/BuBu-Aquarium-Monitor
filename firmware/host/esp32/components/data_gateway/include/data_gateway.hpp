/*
 * 数据网关主接口
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_
#define COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_

#include <string>
#include <functional>
#include "device.hpp"
#include "protocol_base.hpp"
#include "protocol_parser.hpp"
#include "data_receiver.hpp"

// 网关配置
struct DataGatewayConfig {
    bool auto_start = true;              // 是否自动启动
    uint32_t task_stack_size = 4096;     // 任务栈大小
    uint8_t task_priority = 5;           // 任务优先级
    std::string default_protocol = "ttl"; // 默认协议
};

// 数据网关 - 主接口类
class DataGateway {
public:
    static DataGateway& getInstance() {
        static DataGateway instance;
        return instance;
    }
    
    // 禁止拷贝
    DataGateway(const DataGateway&) = delete;
    DataGateway& operator=(const DataGateway&) = delete;
    
    // ===== 初始化 =====
    bool init(const DataGatewayConfig& config = DataGatewayConfig());
    void deinit();
    
    // ===== 启动/停止 =====
    bool start();
    void stop();
    bool isRunning() const { return m_running; }
    
    // ===== 协议管理 =====
    bool registerProtocol(std::unique_ptr<ProtocolBase> protocol);
    bool unregisterProtocol(ProtocolType type);
    ProtocolBase* getProtocol(ProtocolType type);
    
    // ===== 数据回调设置 =====
    void setRecordCallback(DeviceRecordCallback callback) {
        m_parser.setRecordCallback(callback);
    }
    
    // ===== 发送命令 =====
    bool sendCommand(const std::string& device_id, const std::vector<uint8_t>& data);
    bool sendCommand(ProtocolType type, const std::vector<uint8_t>& data);
    
    // ===== 状态查询 =====
    std::string getStatus() const;
    size_t getProtocolCount() const;
    
    // ===== 配置 =====
    void setConfig(const DataGatewayConfig& config) { m_config = config; }
    const DataGatewayConfig& getConfig() const { return m_config; }
    
private:
    DataGateway() = default;
    ~DataGateway() = default;
    
    // 内部数据转发
    void onDataReceived(const std::vector<uint8_t>& data);
    
private:
    DataGatewayConfig m_config;
    ProtocolParser m_parser;
    DataReceiver& m_receiver = DataReceiver::getInstance();
    bool m_running = false;
    std::string m_last_error;
};

#endif /* COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_ */
