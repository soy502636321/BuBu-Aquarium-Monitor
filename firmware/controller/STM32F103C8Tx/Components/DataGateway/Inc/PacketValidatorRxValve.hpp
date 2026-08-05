//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP
#define STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP

#include "crc16.hpp"

class PacketValidatorRxValve: public IRxValve {
public:
    bool process(DataContext& ctx) override {
        std::vector<uint8_t> payload = ctx.packet.getPayload();
        size_t payload_length = ctx.packet.getLength();
        bool verify = CRC16::verify(payload, payload_length); // 验证数据
        if (!verify) {
            printf("CRC16::verify() failed\r\n");
        }
        return verify;
    }
};

#endif //STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP