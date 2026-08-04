//
// Created by Hu on 2026-08-03.
//

#ifndef STM32F103C8TX_CHANNEL_HPP
#define STM32F103C8TX_CHANNEL_HPP

#include <cstdint>
#include <functional>

#include "DataContext.hpp"

// ==================== 通道类型 ====================
enum class ChannelType : uint8_t {
    UART = 0,
    BLE
};

// ==================== 通道方向 ====================
enum class ChannelDirection : uint8_t {
    INPUT = 0,      // 只接收
    OUTPUT = 1,     // 只发送
    BIDIR = 2       // 双向
};

// ==================== IDataChannel 基类（精简） ====================
class IDataChannel {
public:
    virtual ~IDataChannel() = default;

    virtual ChannelDirection getDirection() const = 0;

    void setCallback(std::function<void(uint8_t*, uint16_t)> cb){
        m_callback = cb;
    }

protected:
    std::function<void(uint8_t*, uint16_t)> m_callback;
protected:

    void onReceive(uint8_t* data, uint16_t len) {
        if(m_callback) {
            m_callback(data,len);
        }
    }
};

#endif //STM32F103C8TX_CHANNEL_HPP