//
// Created by Hu on 2026-08-05.
//

#include "ActionExecutor.hpp"

// ============================================================
// ★ 初始化 ★
// ============================================================
void ActionExecutor::init() {
    if (m_initialized) return;

    // ★ 队列存储 DataContext* ★
    m_action_queue = xQueueCreate(10, sizeof(DataContext*));
    if (m_action_queue == nullptr) {
        LOG_ERROR("Action queue creation failed!");
        return;
    }

    BaseType_t ret = xTaskCreate(
        actionTaskEntry,
        "ActionExec",
        256,
        this,
        2,
        &m_task_handle
    );
    if (ret != pdPASS) {
        LOG_ERROR("Action task creation failed! err: %d", ret);
        return;
    }

    // 注册所有 Action 类型
    static SwitchAction switch_action;
    static PwmAction pwm_action;
    static SetupAction setup_action;
    static CollectionAction collection_action;

    registerAction(DeviceSwitch::TYPE_ID, &switch_action);
    registerAction(DevicePwm::TYPE_ID, &pwm_action);
    registerAction(DeviceSetup::TYPE_ID, &setup_action);
    registerAction(DeviceCollection::TYPE_ID, &collection_action);

    m_initialized = true;
    LOG_INFO("ActionExecutor initialized");
}

// ============================================================
// ★ 任务入口 ★
// ============================================================
void ActionExecutor::actionTaskEntry(void* pvParameters) {
    auto* self = static_cast<ActionExecutor*>(pvParameters);
    self->actionProcessLoop();
}

// ============================================================
// ★ 动作执行循环 ★
// ============================================================
void ActionExecutor::actionProcessLoop() {
    DataContext* ctx = nullptr;
    LOG_INFO("ActionExecutor task started");

    while (1) {
        // ★ 阻塞等待 DataContext* ★
        if (xQueueReceive(m_action_queue, &ctx, portMAX_DELAY) == pdTRUE) {
            if (ctx == nullptr) {
                continue;
            }

            // ★ 从 DataContext 中提取数据，选择执行哪个 Action ★
            // 根据 ctx->packet 的类型字段查找对应的 Action
            uint32_t typeId = ctx->getTypeId();  // 从 packet 中解析出 typeId

            IAction* action = findAction(typeId);
            if (action != nullptr) {
                LOG_DEBUG("Executing action type: %d", typeId);
                action->execute(*ctx);  // 传入 DataContext
            } else {
                LOG_WARN("Action type %d not found!", typeId);
            }

            // ★ 释放 DataContext（由谁分配，谁释放） ★
            // 注意：如果 DataContext 是从对象池分配的，这里释放
            // g_data_context_pool.release(ctx);
        }
    }
}

// ============================================================
// ★ 非阻塞分发：入队 DataContext* ★
// ============================================================
bool ActionExecutor::onDispatch(DataContext* ctx) {
    if (m_action_queue == nullptr || ctx == nullptr) {
        return false;
    }

    // ★ 直接入队 DataContext*（零拷贝） ★
    BaseType_t ret = xQueueSend(m_action_queue, &ctx, pdMS_TO_TICKS(100));
    if (ret != pdPASS) {
        LOG_WARN("Action queue full!");
        return false;
    }

    LOG_DEBUG("DataContext enqueued");
    return true;
}