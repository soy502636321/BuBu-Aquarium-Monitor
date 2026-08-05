//
// Created by Hu on 2026-08-05.
//

#include "ActionManager.hpp"

void ActionManager::init() {
    // 初始化
    SwitchAction switch_action;
    registerAction(0, &switch_action); // 开关工作
}

void ActionManager::onDispatch(DataContext &ctx) {
    // ========== 分发 ==========
    IAction* action = findAction(ctx.getTypeId());
    return action ? action->execute(ctx) : false;
}
