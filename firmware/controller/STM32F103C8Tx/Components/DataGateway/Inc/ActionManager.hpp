//
// Created by Hu on 2026-08-05.
//

#ifndef STM32F103C8TX_ACTIONMANAGER_HPP
#define STM32F103C8TX_ACTIONMANAGER_HPP
#include "Action.hpp"
#include "DataContext.hpp"

#define MAX_ACTION_SIZE 12

class ActionManager {
public:
    static ActionManager& getInstance()
    {
        static ActionManager instance;
        return instance;
    }

    void init();

    bool onDispatch(DataContext &ctx);

    // 注册action
    void registerAction(uint32_t typeId, IAction* action) {
        if (m_action_count >= MAX_ACTION_SIZE) return;
        // 检查是否已存在
        for (size_t i = 0; i < m_action_count; i++) {
            if (m_actions[i].typeId == typeId) {
                m_actions[i].action = action;  // 更新
                return;
            }
        }
        // 新增
        m_actions[m_action_count].typeId = typeId;
        m_actions[m_action_count].action = action;
        m_action_count++;
    }

    // ========== 查找Action ==========
    IAction* findAction(uint32_t typeId) const {
        printf("findAction: typeId=%u, m_action_count=%u\n", typeId, m_action_count);
        fflush(stdout);
        for (size_t i = 0; i < m_action_count; i++) {
            printf("  [%u] typeId=%u, action=0x%p\n",
                           i, m_actions[i].typeId, m_actions[i].action);
            fflush(stdout);
            if (m_actions[i].typeId == typeId) {
                printf("findAction: FOUND at index %u\n", i);
                return m_actions[i].action;
            }
        }
        __asm__("nop");
        printf("findAction - 4 \r\n");
        return nullptr;
    }
private:
    ActionManager() {
        init();
    };

    ~ActionManager() = default;

    ActionManager(const ActionManager&) = delete;

    ActionManager& operator=(const ActionManager&) = delete;

public:
    struct ActionEntry {
        uint32_t typeId;
        IAction* action;
    };
    ActionEntry m_actions[MAX_ACTION_SIZE] = {};  // 最多12个
    uint8_t m_action_count = 0; // 记录数量
};
#endif //STM32F103C8TX_ACTIONMANAGER_HPP