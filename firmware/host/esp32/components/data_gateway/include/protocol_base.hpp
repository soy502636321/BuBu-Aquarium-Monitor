/*
 * protocol_base.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_BASE_HPP_
#define COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_BASE_HPP_


#include <string>
#include <vector>
#include <functional>
#include "device.hpp"

// 协议类型枚举
enum class ProtocolType : uint8_t {
    TTL_SERIAL = 0,
    BLE = 1,
    WIFI = 2,
    LORA = 3,
    RS485 = 4,
    CAN = 5
};

// 原始数据回调类型
using RawDataCallback = std::function<void(const std::vector<uint8_t>&)>;

// 协议基类
class ProtocolBase {
public:
    virtual ~ProtocolBase() = default;
    
    // 协议初始化
    virtual bool init() = 0;
    
    // 协议启动
    virtual bool start() = 0;
    
    // 协议停止
    virtual void stop() = 0;
    
    // 发送数据（向采集板下发命令）
    virtual bool sendCommand(const std::vector<uint8_t>& data) = 0;
    
    // 设置数据接收回调
    void setDataCallback(RawDataCallback callback) {
        m_callback = callback;
    }
    
    // 获取协议类型
    virtual ProtocolType getType() const = 0;
    
    // 获取协议名称
    virtual std::string getName() const = 0;
    
    // 检查协议是否运行中
    virtual bool isRunning() const = 0;
    
protected:
    RawDataCallback m_callback = nullptr;
};

#endif /* COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_BASE_HPP_ */
