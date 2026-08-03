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

// ==================== IChannel 基类（精简） ====================
class IChannel {
public:
    virtual ~IChannel() = default;

    // -------- 基本信息 --------
    virtual ChannelType getType() const = 0;
    virtual ChannelDirection getDirection() const = 0;

    // -------- 状态 --------
    virtual bool isConnected() const = 0;
};

// ==================== IOutputChannel（发送） ====================
class IOutputChannel : public virtual IChannel {
public:
    virtual ~IOutputChannel() = default;

    ChannelDirection getDirection() const override {
        return ChannelDirection::OUTPUT;
    }

    // 发送数据
    virtual bool send(const uint8_t* data, uint16_t length) = 0;
    virtual bool send(const char* str) = 0;
    virtual bool send(const DataContext& ctx) = 0;
};

// ==================== IInputChannel（接收） ====================
class IInputChannel : public virtual IChannel {
public:
    virtual ~IInputChannel() = default;

    ChannelDirection getDirection() const override {
        return ChannelDirection::INPUT;
    }

    // 读取数据
    virtual uint16_t read(uint8_t* buffer, uint16_t max_len) = 0;
    virtual bool hasData() const = 0;
    virtual void flush() = 0;
};

// ==================== IBidirChannel（双向） ====================
class IBidirChannel : public IOutputChannel, public IInputChannel {
public:
    virtual ~IBidirChannel() = default;

    ChannelDirection getDirection() const override {
        return ChannelDirection::BIDIR;
    }
};
#endif //STM32F103C8TX_CHANNEL_HPP