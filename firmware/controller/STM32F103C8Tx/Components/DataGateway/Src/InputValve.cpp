#include "Valve.hpp"
#include <functional>
#include "Channel.hpp"
// #include "UartChannel.hpp"

InputValve::InputValve() {
    // 初始化代码
    // addChannel(static_cast<IInputChannel*>(&UartChannel::getInstance()));
}

bool InputValve::process(DataContext &ctx) {
    if (!m_enabled || !ctx.packet->getData()) {
        return false;
    }

    // m_stats.total++;

    DataPacket packet;
    // packet.setRawData(ctx.packet.data, ctx.packet.length);
    // packet.source = ctx.packet.source;
    // packet.timestamp = ctx.packet.timestamp;

    if (m_auto_parse) {
        // packet.parse();
    }

    dispatch(packet);
    return true;
}

void InputValve::dispatch(const DataPacket &packet) {
    // if (m_callbacks.onData) {
        // m_callbacks.onData(packet);
    // }

    // switch (packet.getType()) {
    //     case PacketType::COMMAND:
    //         m_stats.commands++;
    //         if (m_callbacks.onCommand) m_callbacks.onCommand(packet);
    //         break;
    //     case PacketType::RECORD:
    //         m_stats.records++;
    //         if (m_callbacks.onRecord) m_callbacks.onRecord(packet);
    //         break;
    //     default:
    //         m_stats.raw++;
    //         if (m_callbacks.onRawData) m_callbacks.onRawData(packet);
    //         break;
    // }
}

// void InputValve::onCommand(std::function<void(const DataPacket &)> cb) {
//     m_callbacks.onCommand = cb;
// }
//
// void InputValve::onRecord(std::function<void(const DataPacket &)> cb) {
//     m_callbacks.onRecord = cb;
// }
//
// void InputValve::onRawData(std::function<void(const DataPacket &)> cb) {
//     m_callbacks.onRawData = cb;
// }
//
// void InputValve::onData(std::function<void(const DataPacket &)> cb) {
//     m_callbacks.onData = cb;
// }

// void InputValve::resetStats() {
//     m_stats = Stats{};
// }
