//
// Created by Hu on 2026-08-05.
//

#include "ActionManager.hpp"

void ActionManager::init() {
    // 初始化
    static SwitchAction switch_action;
    static PwmAction pwm_action;
    static SetupAction setup_action;
    static CollectionAction collection_action;
    registerAction(DeviceSwitch::TYPE_ID, &switch_action); // 开关动作
    registerAction(DevicePwm::TYPE_ID, &pwm_action); // PWM动作
    registerAction(DeviceSetup::TYPE_ID, &setup_action); // 设置、配置动作
    registerAction(DeviceCollection::TYPE_ID, &collection_action); // 立即采集动作
}

bool ActionManager::onDispatch(DataContext &ctx) {
    // ========== 分发 ==========
    printf("ActionManager::onDispatch\r\n");
    printf("A O- %d \r\n", ctx.packet.getLength());
    IAction* action = findAction(DeviceSwitch::TYPE_ID);
    printf("get action : %d\r\n", DeviceSwitch::TYPE_ID);
    if (action == nullptr) {
        printf("Error: No action found for typeId: %d\r\n", DeviceSwitch::TYPE_ID);
    }
    return action->execute(ctx);
}
