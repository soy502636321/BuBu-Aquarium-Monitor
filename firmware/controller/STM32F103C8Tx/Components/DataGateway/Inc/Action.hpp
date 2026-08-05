//
// Created by Hu on 2026-08-05.
//

#ifndef STM32F103C8TX_ACTION_HPP
#define STM32F103C8TX_ACTION_HPP
#include "DataContext.hpp"

// ==================== IDataChannel 基类（精简） ====================
class IAction {
public:
    virtual ~IAction() = default;
    virtual void execute(DataContext& ctx);
};

class SwitchAction : public IAction {
public:
    void execute(DataContext &ctx) override {

    }
};

class PwmAction : public IAction {
    public:
    void execute(DataContext &ctx) override {

    }
};
#endif //STM32F103C8TX_ACTION_HPP