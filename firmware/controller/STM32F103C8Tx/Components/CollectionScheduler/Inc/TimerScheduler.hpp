//
// Created by Hu on 2026/8/2.
//

#ifndef STM32F103C8TX_TIMERSCHEDULER_HPP
#define STM32F103C8TX_TIMERSCHEDULER_HPP

#include <cstdint>
#include <cstring>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "Logger.hpp"
#include "TimerTask.hpp"

// ★ 最大任务数量 ★
#define MAX_TASK_SIZE  4

    class TimerScheduler {

    public:
        // -------- 获取单例 --------
        static TimerScheduler& getInstance();

        // -------- 初始化 --------
        void init(uint32_t initial_tick = 0);

        // -------- 启动/停止定时器 --------
        void start();
        void stop();

        // -------- 添加任务 --------
        bool addTask(const char* name, uint32_t interval_seconds, void (*callback)(void*), void* arg);

        // -------- 启用/禁用任务 --------
        void enableTask(const char* name, bool enabled);

        // -------- 获取信息 --------
        size_t getTaskCount() const;
        uint32_t getTickSeconds() const;

    private:
        // -------- 单例模式 --------
        TimerScheduler();
        ~TimerScheduler() = default;
        TimerScheduler(const TimerScheduler&) = delete;
        TimerScheduler& operator=(const TimerScheduler&) = delete;

        // -------- 内部函数 --------
        static void timerCallback(TimerHandle_t xTimer);
        void checkTasks();

    private:
        // -------- 成员变量 --------
        TimerTask m_tasks[MAX_TASK_SIZE];
        int m_task_count;
        uint32_t tick_seconds;

        TimerHandle_t m_timer;
        bool m_initialized;
    };

#endif //STM32F103C8TX_TIMERSCHEDULER_HPP