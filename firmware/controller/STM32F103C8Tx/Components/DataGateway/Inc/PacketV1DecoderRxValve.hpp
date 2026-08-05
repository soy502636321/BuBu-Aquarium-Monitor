//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP
#define STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP

#include "crc16.hpp"

class PacketV1DecoderRxValve: public IRxValve {
public:
    bool process(DataContext& ctx) override {
        uint8_t* payload = ctx.packet.getRaw();
        size_t payload_length = ctx.packet.getLength();

        uint8_t ver = payload[3];
        uint8_t type = payload[4]; // 数据类型

        switch (type) {
            case 0x01: {
                break;
            }
            case 0x02: {
                break;
            }
        }
        // PWM控制
        // SWITCH控制
        // 设置配置

        return true;
    }
};

#endif //STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP