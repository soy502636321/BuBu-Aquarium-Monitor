//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP
#define STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP

#include "crc16.hpp"

class PacketV1DecoderRxValve: public IRxValve {
public:
    bool process(DataContext& ctx) override {
        //
        printf("test Decoder Rx Value\r\n");
        std::vector<uint8_t> payload = ctx.packet.getPayload();
        uint8_t data_type = payload[3];

        switch (data_type) {
            case static_cast<uint8_t>(CommandType::DATA): {
                return decodeData(ctx);
            }
            case 0x02: {
                break;
            }
            default:
                printf("NOT IMPLEMENTED YET\r\n");
                return false;
        }
        // PWM控制
        // SWITCH控制
        // 设置配置

        return true;
    }

private:
    bool decodeData(DataContext& ctx) {
        std::vector<uint8_t> payload = ctx.packet.getPayload();
        size_t payload_length = ctx.packet.getLength();
        uint8_t data_version = payload[2];
        uint8_t data_type = payload[3];
        uint8_t data_len = payload[4];

        DeviceRecord record;

        return true;
    }
};

#endif //STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP