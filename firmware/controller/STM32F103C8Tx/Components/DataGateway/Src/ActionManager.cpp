//
// Created by Hu on 2026-08-05.
//

#include "ActionManager.hpp"

void ActionManager::init() {
    // 初始化
    SwitchAction switch_action;
    PwmAction pwm_action;
    SetupAction setup_action;
    CollectionAction collection_action;
    registerAction(DeviceSwitch::TYPE_ID, &switch_action); // 开关动作
    registerAction(DevicePwm::TYPE_ID, &pwm_action); // PWM动作
    registerAction(DeviceSetup::TYPE_ID, &setup_action); // 设置、配置动作
    registerAction(DeviceCollection::TYPE_ID, &collection_action); // 立即采集动作
}

bool ActionManager::onDispatch(DataContext &ctx) {
    // ========== 分发 ==========
    IAction* action = findAction(ctx.getTypeId());
    return action ? action->execute(ctx) : false;
}
