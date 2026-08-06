//
// Created by Hu on 2026-08-06.
//

#ifndef STM32F103C8TX_OBJECTPOOL_H
#define STM32F103C8TX_OBJECTPOOL_H
#include "stm32f1xx_hal.h"

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
                    printf("[Pool] Allocated slot %d, address 0x%p\n",
                           i, &m_pool[i]);
                    return &m_pool[i];
                }
            }

            // ===== 没有空闲对象，等待 =====
            printf("[Pool] No free slot! Waiting...\n");
            m_waitingCount++;

            // 等待 10ms 后重试
            HAL_Delay(200);
        }
    }

    // ===== 分配对象（带超时） =====
    T* allocate(uint32_t timeout_ms) {
        uint32_t start = HAL_GetTick();

        while (true) {
            for (int i = 0; i < N; i++) {
                if (!m_inUse[i]) {
                    m_inUse[i] = true;
                    resetObject(i);
                    printf("[Pool] Allocated slot %d (waited %u ms)\n", i, HAL_GetTick() - start);
                    return &m_pool[i];
                }
            }

            if (HAL_GetTick() - start >= timeout_ms) {
                printf("[Pool] Timeout! No free slot\n");
                return nullptr;
            }

            HAL_Delay(10);
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

        m_inUse[index] = true;
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
#endif //STM32F103C8TX_OBJECTPOOL_H