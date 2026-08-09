//
// Created by Hu on 2026/8/2.
//

#ifndef STM32F103C8TX_COLLECTIONTASK_HPP
#define STM32F103C8TX_COLLECTIONTASK_HPP

#include <cstdint>
// 传感器采集任务
struct TimerTask {
            char name[32];
            uint32_t interval_seconds;
            void (*callback)(void* arg);
            void* arg;
            bool enabled;
            uint32_t last_run_tick;
};

#endif //STM32F103C8TX_COLLECTIONTASK_HPP