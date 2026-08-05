// DataGateway/inc/Valve.hpp
#ifndef VALVE_HPP
#define VALVE_HPP

#include <cstdint>
#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

#include "Channel.hpp"
#include "DataContext.hpp"

// ==================== Valve 基类 ====================
class IValve {
public:
    virtual ~IValve() = default;

protected:
    bool m_enabled = true;
};

// ==================== Tx Valve ====================
class ITxValve : public IValve {
public:
    // 处理发送数据包
    // 返回 true: 继续处理，false: 中止
    virtual bool process(DataContext& ctx) = 0;
};

// ==================== Rx Valve ====================
class IRxValve : public IValve {
public:
    // 处理接收数据包
    // 返回 true: 继续处理，false: 中止
    virtual bool process(DataContext& packet) = 0;
};

// 输入阀门
// class InputValve : public IRxValve {
// public:
//     InputValve();  // 声明默认构造函数
//
//     bool process(DataContext& ctx) override;
//
//     void onDataReady(const uint8_t* data, uint16_t len) {
//         // 准备数据上下文
//         printf("input -> onDataReady\r\n");
//     }
//
//     // -------- 通道管理 --------
//     bool addChannel(IInputChannel* channel) {
//         if (channel == nullptr) {
//             return false;
//         }
//         for (auto& ch : m_channels) {
//             if (ch == channel) {
//                 return false;  // 已存在
//             }
//         }
//         channel->setDataCallback([this](const uint8_t* data, uint16_t len) {
//                 // 通道有数据到达！
//                 this->onDataReady(data, len);
//         });
//         m_channels.push_back(channel);
//         return true;
//     }
//
//     bool removeChannel(ChannelType type) {
//         // auto it = std::remove_if(m_channels.begin(), m_channels.end(),
//         //     [type](IInputChannel* channel) {
//         //         return channel && channel->getType() == type;
//         //     });
//         //
//         // if (it != m_channels.end()) {
//         //     m_channels.erase(it, m_channels.end());
//         //     return true;
//         // }
//         return false;
//     }
//
//     void clearChannels() {
//         m_channels.clear();
//     }
//     // 配置
//     void setAutoParse(bool enable) { m_auto_parse = enable; }
//     void resetStats();
//
// private:
//     void dispatch(const DataPacket& packet);
//
//     // InputCallbacks m_callbacks;
//     bool m_auto_parse = true;
//     std::vector<IInputChannel*> m_channels;
// };

#endif // VALVE_HPP
