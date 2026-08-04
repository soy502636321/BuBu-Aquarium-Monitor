//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP
#define STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP

class PacketValidatorRxValve: public IRxValve {
public:
    bool process(DataContext& packet) override {
        return true;
    }
};

#endif //STM32F103C8TX_PACKETVALIDATORRXVALVE_HPP