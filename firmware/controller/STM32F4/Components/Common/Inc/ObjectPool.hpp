//
// Created by Hu on 2026-08-06.
//

#ifndef STM32F103C8TX_OBJECTPOOL_H
#define STM32F103C8TX_OBJECTPOOL_H
#include "stm32f4xx_hal.h"
#include "Logger.hpp"

template<typename T, int N>
class ObjectPool {
private:
    T m_pool[N];
    bool m_inUse[N] = {false};
    bool m_resetPending = false;  // 等待重置标志
    int m_waitingCount = 0;       // 等待的任务数

public:
    // ===== 分配对象（阻塞等待） =====
    T* allocate() {
        while (true) {
            // 尝试分配空闲对象
            for (int i = 0; i < N; i++) {
                if (!m_inUse[i]) {
                    m_inUse[i] = true;
                    // ✅ 重置对象到初始状态
                    resetObject(i);
                    LOG_WARN("[Pool] Allocated slot %d, address 0x%p\n",i, &m_pool[i]);
                    return &m_pool[i];
                }
            }

            // ===== 没有空闲对象，等待 =====
            LOG_WARN("[Pool] No free slot! Waiting...\n");
            m_waitingCount++;
            // 等待 10ms 后重试
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    // ===== 分配对象（带超时） =====
    T* allocate(uint32_t timeout_ms) {
        TickType_t start = xTaskGetTickCount();
        TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);

        while (true) {
            for (int i = 0; i < N; i++) {
                if (!m_inUse[i]) {
                    resetObject(i);
                    m_inUse[i] = true;
                    printf("[Pool] Allocated slot %d (waited %u ms)\n", i, HAL_GetTick() - start);
                    return &m_pool[i];
                }
            }

            // 检查超时
            if (timeout_ms != portMAX_DELAY) {
                if ((xTaskGetTickCount() - start) >= timeout_ticks) {
                    return nullptr;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    // ===== 释放对象 =====
    void release(T* ptr) {
        if (ptr == nullptr) return;

        for (int i = 0; i < N; i++) {
            if (&m_pool[i] == ptr) {
                m_inUse[i] = false;
                printf("[Pool] Released slot %d, address 0x%p\n", i, ptr);
                return;
            }
        }

        printf("[Pool] ERROR: ptr 0x%p not found!\n", ptr);
    }

    // ===== 重置对象 =====
    void resetObject(int index) {
        // 方法1：赋值默认值
        m_pool[index] = T();

        // 方法2：如果 T 有 clear() 方法
        // m_pool[index].reset();

        // 方法3：placement new（重新构造）
        // m_pool[index].~T();
        // new (&m_pool[index]) T();

        m_inUse[index] = false;
    }

    // ===== 获取状态 =====
    int available() const {
        int count = 0;
        for (int i = 0; i < N; i++) {
            if (!m_inUse[i]) count++;
        }
        return count;
    }

    bool isFull() const {
        return available() == 0;
    }
};

inline ObjectPool<DataContext, 8> g_rx_data_context_pool;
inline ObjectPool<DataContext, 8> g_tx_data_context_pool;

#endif //STM32F103C8TX_OBJECTPOOL_H