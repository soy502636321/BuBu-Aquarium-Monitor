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
        TimerScheduler() : tick_seconds(0) {
            init();
        }
        ~TimerScheduler() = default;

        TimerScheduler(const TimerScheduler&) = delete;
        TimerScheduler& operator=(const TimerScheduler&) = delete;


    private:
        std::vector<CollectionTask> tasks;
        uint32_t tick_seconds;
        bool initialized = false;

    public:
        // -------- 获取单例 --------
        static TimerScheduler& getInstance() {
            static TimerScheduler instance;
            return instance;
        }

        void onTick() {
            tick_seconds++;
            checkTasks();
        }

        void init(uint32_t initial_tick = 0) {
            if (initialized) {
                printf("[TimerScheduler] 已初始化\r\n");
                return;
            }
            tick_seconds = initial_tick;
            initialized = true;
            printf("[TimerScheduler] 初始完成，一共 [%zu] 个人任务\r\n", tasks.size());
        }

        void start() {
            printf("[TimerScheduler] 启动定时采集任务\r\n");
        }

        // 添加任务
        void addTask(const char* name, uint32_t interval_ms, std::function<void()> callback) {
            tasks.emplace_back(name, interval_ms, callback);
        }

        // 检查并执行任务
        void checkTasks() {
            for (auto& task : tasks) {
                if (!task.enabled) continue;
                // 取模判断：当前 tick 能被 interval 整除时执行
                if (tick_seconds > 0 && tick_seconds % task.interval_seconds == 0) {
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