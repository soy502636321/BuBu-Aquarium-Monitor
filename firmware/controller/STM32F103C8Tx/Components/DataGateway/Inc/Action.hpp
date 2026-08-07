//
// Created by Hu on 2026-08-05.
//

#ifndef STM32F103C8TX_ACTION_HPP
#define STM32F103C8TX_ACTION_HPP
#include "DataContext.hpp"

// ==================== IDataChannel 基类（精简） ====================
class IAction {
public:
    virtual bool execute(DataContext& ctx);
};

class SwitchAction : public IAction {
public:
    bool execute(DataContext &ctx) {
        DeviceSwitch* device_switch = ctx.getData<DeviceSwitch>();
        return true;
    }
};

class PwmAction : public IAction {
    public:
    bool execute(DataContext &ctx) {
        return true;
    }
};

class SetupAction : public IAction {
    public:
    bool execute(DataContext &ctx) {
        return true;
    }
};

// 立即采集动作
class CollectionAction : public IAction {
    public:
    bool execute(DataContext &ctx) {
        return true;
    }
};
#endif //STM32F103C8TX_ACTION_HPP