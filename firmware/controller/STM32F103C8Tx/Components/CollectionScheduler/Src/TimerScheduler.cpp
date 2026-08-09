//
// Created by Hu on 2026/8/2.
//

#include "TimerScheduler.hpp"

    // ============================================================
    // ★ 单例获取 ★
    // ============================================================
    TimerScheduler& TimerScheduler::getInstance() {
        static TimerScheduler instance;
        return instance;
    }

    // ============================================================
    // ★ 构造函数 ★
    // ============================================================
    TimerScheduler::TimerScheduler()
        : m_task_count(0)
        , tick_seconds(0)
        , m_timer(nullptr)
        , m_initialized(false) {
        // 清空任务数组
        memset(m_tasks, 0, sizeof(m_tasks));
    }

    // ============================================================
    // ★ 初始化 ★
    // ============================================================
    void TimerScheduler::init(uint32_t initial_tick) {
        if (m_initialized) {
            LOG_WARN("TimerScheduler already initialized");
            return;
        }

        tick_seconds = initial_tick;
        m_task_count = 0;
        memset(m_tasks, 0, sizeof(m_tasks));

        // ★ 创建 FreeRTOS 软件定时器 ★
        m_timer = xTimerCreate(
            "SchedTimer",           // 定时器名称
            pdMS_TO_TICKS(1000),    // 1秒周期
            pdTRUE,                 // 自动重载
            this,                   // 回调参数（传递 this）
            timerCallback           // 回调函数
        );

        if (m_timer == nullptr) {
            LOG_ERROR("Timer creation failed!");
            return;
        }

        m_initialized = true;
        LOG_INFO("TimerScheduler initialized");
    }

    // ============================================================
    // ★ 启动定时器 ★
    // ============================================================
    void TimerScheduler::start() {
        if (m_timer == nullptr) {
            LOG_ERROR("Timer not created!");
            return;
        }

        if (xTimerStart(m_timer, pdMS_TO_TICKS(100)) != pdPASS) {
            LOG_ERROR("Timer start failed!");
        } else {
            LOG_INFO("TimerScheduler started");
        }
    }

    // ============================================================
    // ★ 停止定时器 ★
    // ============================================================
    void TimerScheduler::stop() {
        if (m_timer != nullptr) {
            xTimerStop(m_timer, pdMS_TO_TICKS(100));
            LOG_INFO("TimerScheduler stopped");
        }
    }

    // ============================================================
    // ★ 添加任务 ★
    // ============================================================
    bool TimerScheduler::addTask(const char* name, uint32_t interval_seconds, void (*callback)(void*), void* arg) {
        if (m_task_count >= MAX_TASK_SIZE) {
            LOG_ERROR("Too many tasks! Max: %d", MAX_TASK_SIZE);
            return false;
        }

        TimerTask* task = &m_tasks[m_task_count];
        strncpy(task->name, name, sizeof(task->name) - 1);
        task->name[sizeof(task->name) - 1] = '\0';
        task->interval_seconds = interval_seconds;
        task->callback = callback;
        task->arg = arg;
        task->enabled = true;
        task->last_run_tick = 0;

        m_task_count++;
        LOG_INFO("Task added: %s, interval: %u seconds", name, interval_seconds);
        return true;
    }

    // ============================================================
    // ★ 启用/禁用任务 ★
    // ============================================================
    void TimerScheduler::enableTask(const char* name, bool enabled) {
        for (int i = 0; i < m_task_count; i++) {
            if (strcmp(m_tasks[i].name, name) == 0) {
                m_tasks[i].enabled = enabled;
                LOG_INFO("Task %s %s", name, enabled ? "enabled" : "disabled");
                return;
            }
        }
        LOG_WARN("Task %s not found", name);
    }

    // ============================================================
    // ★ 获取任务数量 ★
    // ============================================================
    size_t TimerScheduler::getTaskCount() const {
        return m_task_count;
    }

    // ============================================================
    // ★ 获取当前 tick（秒） ★
    // ============================================================
    uint32_t TimerScheduler::getTickSeconds() const {
        return tick_seconds;
    }

    // ============================================================
    // ★ 定时器回调（静态函数） ★
    // ============================================================
    void TimerScheduler::timerCallback(TimerHandle_t xTimer) {
        // 从定时器句柄获取 TimerScheduler 实例
        TimerScheduler* self = static_cast<TimerScheduler*>(pvTimerGetTimerID(xTimer));
        if (self != nullptr) {
            self->tick_seconds++;
            self->checkTasks();
        }
    }

    // ============================================================
    // ★ 检查并执行任务 ★
    // ============================================================
    void TimerScheduler::checkTasks() {
        for (int i = 0; i < m_task_count; i++) {
            TimerTask* task = &m_tasks[i];
            if (!task->enabled) continue;

            // 检查是否到达执行时间
            if (tick_seconds % task->interval_seconds == 0) {
                // 防止同一 tick 重复执行
                if (task->last_run_tick != tick_seconds) {
                    task->last_run_tick = tick_seconds;
                    if (task->callback != nullptr) {
                        task->callback(task->arg);
                    }
                }
            }
        }
    }