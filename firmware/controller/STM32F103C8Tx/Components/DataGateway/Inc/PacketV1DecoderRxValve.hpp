//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP
#define STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP

#include "crc16.hpp"
#include "Device.hpp"

class PacketV1DecoderRxValve: public IRxValve {
public:
    bool process(DataContext& ctx) override {
        //
        printf("test Decoder Rx Value\r\n");
        const uint8_t *payload = ctx.packet.getPayload();
        uint8_t data_type = payload[3];

        switch (data_type) {
            case static_cast<uint8_t>(DeviceDataType::DATA): {
                return decodeData(ctx);
            }
            case static_cast<uint8_t>(DeviceDataType::SWITCH): {
                return decodeSwitch(ctx);
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
        const uint8_t *payload = ctx.packet.getPayload();
        size_t payload_length = ctx.packet.getLength();
        uint8_t data_version = payload[2];
        uint8_t data_type = payload[3];
        uint8_t data_len = payload[4];

        return true;
    }

    bool decodeSwitch(DataContext& ctx) {
        printf("test Decoder Switch Rx Value\r\n");
        const uint8_t* payload = ctx.packet.getPayload();
        size_t payload_length = ctx.packet.getLength();
        uint8_t data_version = payload[2];
        DeviceDataType data_type = static_cast<DeviceDataType>(payload[3]);
        uint8_t data_len = payload[4];
        DataPacket packet;
        printf("test Decoder Switch Rx Value 1\r\n");

        static DeviceSwitch device_switch;
        packet.setVersion(data_version);
        packet.setDataType(data_type);
        packet.setLength(payload_length);
        printf("test Decoder Switch Rx Value 2\r\n");

        for (size_t i = 5; i < (5 + data_len); i += 2) {
            uint8_t channel = payload[i]; // 频道
            uint8_t state = payload[i + 1]; // 开关
            device_switch.addRelay(channel, static_cast<bool>(state)); // 存储需要操作的继电器频道
        }
        printf("test Decoder Switch Rx Value 5\r\n");
        packet.setData(device_switch);
        printf("test Decoder Switch Rx Value 6\r\n");
        ctx.packet = packet;
        printf("test Decoder Switch Rx Value 7\r\n");
        return true;
    }
};

#endif //STM32F103C8TX_PACKET_DECODER_RXVALVE_HPP