//
// Created by Hu on 2026-08-05.
//

#ifndef STM32F103C8TX_ACTIONEXECUTOR_HPP
#define STM32F103C8TX_ACTIONEXECUTOR_HPP

#include "Action.hpp"
#include "DataContext.hpp"
#include "Logger.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define MAX_ACTION_SIZE 12

class ActionExecutor {
public:
    static ActionExecutor& getInstance() {
        static ActionExecutor instance;
        return instance;
    }

    void init();

    // ★ 非阻塞分发：传入 DataContext* ★
    bool onDispatch(DataContext* ctx);

    // 注册 action
    void registerAction(uint32_t typeId, IAction* action) {
        if (m_action_count >= MAX_ACTION_SIZE) return;
        for (size_t i = 0; i < m_action_count; i++) {
            if (m_actions[i].typeId == typeId) {
                m_actions[i].action = action;
                return;
            }
        }
        m_actions[m_action_count].typeId = typeId;
        m_actions[m_action_count].action = action;
        m_action_count++;
    }

    // 查找 Action
    IAction* findAction(uint32_t typeId) const {
        for (size_t i = 0; i < m_action_count; i++) {
            if (m_actions[i].typeId == typeId) {
                return m_actions[i].action;
            }
        }
        return nullptr;
    }

private:
    ActionExecutor() = default;
    ~ActionExecutor() = default;
    ActionExecutor(const ActionExecutor&) = delete;
    ActionExecutor& operator=(const ActionExecutor&) = delete;

    // ★ 任务相关 ★
    static void actionTaskEntry(void* pvParameters);
    void actionProcessLoop();

private:
    struct ActionEntry {
        uint32_t typeId;
        IAction* action;
    };
    ActionEntry m_actions[MAX_ACTION_SIZE] = {};
    uint8_t m_action_count = 0;

    // ★ 队列：存储 DataContext* ★
    QueueHandle_t m_action_queue = nullptr;
    TaskHandle_t m_task_handle = nullptr;
    bool m_initialized = false;
};

#endif // STM32F103C8TX_ACTIONEXECUTOR_HPP