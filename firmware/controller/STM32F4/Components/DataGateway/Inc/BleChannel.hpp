//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_BLECHANNEL_HPP
#define STM32F103C8TX_BLECHANNEL_HPP

#include "Channel.hpp"
#include <cstdio>
#include <functional>

class BleChannel : public IChannel {
private:
    std::function<void(const uint8_t*, uint16_t)> m_callback;

public:
    BleChannel() = default;

    // =============================================
    // 1️⃣ 来自 IOutputChannel
    // =============================================
    // ✅ 添加返回值和 override
    // bool send(const uint8_t* data, uint16_t len) override {
    //     printf("BLE send: %d bytes\n", len);
    //     // TODO: 实际 BLE 发送代码
    //     return true;  // ✅ 必须返回 bool
    // }

    // =============================================
    // 2️⃣ 来自 IDataChannel
    // =============================================
    // ✅ 添加 const 和 override
    ChannelDirection getDirection() const override {
        return ChannelDirection::BIDIR;
    }

    // =============================================
    // 3️⃣ 来自 IInputChannel（必须实现！）
    // =============================================
    // ✅ 实现 setDataCallback（不是 receive！）
    // void setDataCallback(std::function<void(const uint8_t*, uint16_t)> callback) override {
    //     m_callback = callback;
    //     printf("BLE: data m_callback registered\n");
    // }

    // =============================================
    // 4️⃣ 当 BLE 收到数据时调用（内部使用）
    // =============================================
    void onDataReceived(const uint8_t* data, uint16_t len) {
        if (m_callback) {
            m_callback(data, len);
        }
    }
};

#endif // STM32F103C8TX_BLECHANNEL_HPP