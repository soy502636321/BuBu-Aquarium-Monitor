//
// Created by Hu on 2026/8/2.
//

#ifndef STM32F103C8TX_TIMERSCHEDULER_HPP
#define STM32F103C8TX_TIMERSCHEDULER_HPP


#include <vector>
#include <cstdint>
#include "CollectionTask.hpp"

namespace Components {

    class TimerScheduler {

    private:
        // -------- 单例模式 --------
        TimerScheduler() : tick_time(0) {
            init();
        }
        ~TimerScheduler() = default;

        TimerScheduler(const TimerScheduler&) = delete;
        TimerScheduler& operator=(const TimerScheduler&) = delete;


    private:
        std::vector<CollectionTask> tasks;
        uint32_t tick_time;
        bool initialized = false;

    public:
        // -------- 获取单例 --------
        static TimerScheduler& getInstance() {
            static TimerScheduler instance;
            return instance;
        }

        void onTick() {
            tick_time++;
            printf("[TimerScheduler] %lu S -> onTimerTick\r\n", tick_time);

            // ✅ 每 30 秒执行一次（从 30 开始，不在 0 时刻触发）
            if (tick_time > 0 && tick_time % 10 == 0) {
                printf("[TimerScheduler] 10S -> onTimerTick\r\n");
                // 在这里执行你的 30 秒任务
            }
        }

        void init(uint32_t initial_tick = 0) {
            if (initialized) {
                printf("[TimerScheduler] 已初始化\r\n");
                return;
            }
            tick_time = initial_tick;
            initialized = true;
            printf("[TimerScheduler] 初始完成，一共 [%zu] 个人任务\r\n", tasks.size());
        }

        void start() {
            printf("[TimerScheduler] 启动定时采集任务\r\n");
        }

        // 添加任务
        void addTask(const char* name, uint32_t interval_ms,
                     std::function<void()> callback) {
            tasks.emplace_back(name, interval_ms, callback);
        }

        // 更新 tick (在 SysTick 或定时器中断中调用)
        void tick(uint32_t ms) {
            tick_time = ms;
            checkTasks();
        }

        // 检查并执行任务
        void checkTasks() {
            for (auto& task : tasks) {
                if (!task.enabled) continue;

                if (tick_time - task.last_run_ms >= task.interval_ms) {
                    task.last_run_ms = tick_time;
                    if (task.callback) {
                        task.callback();
                    }
                }
            }
        }

        // 获取任务数量
        size_t getTaskCount() const { return tasks.size(); }
    };
}
#endif //STM32F103C8TX_TIMERSCHEDULER_HPP