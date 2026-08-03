//
// Created by Hu on 2026/8/2.
//

#ifndef STM32F103C8TX_COLLECTIONTASK_HPP
#define STM32F103C8TX_COLLECTIONTASK_HPP

#include <cstdint>
#include <functional>

namespace Components {

// 传感器采集任务
struct CollectionTask {
    const char* name;                    // 传感器名称
    uint32_t interval_seconds;                // 采集间隔 (毫秒)
    uint32_t last_run_seconds;                // 上次运行时间
    std::function<void()> callback;      // 采集回调函数
    bool enabled;                        // 是否启用

    // 构造函数
    CollectionTask(const char* n, uint32_t interval, std::function<void()> cb)
        : name(n), interval_seconds(interval), last_run_seconds(0), callback(cb), enabled(true) {}

    CollectionTask(const char* n, uint32_t interval, std::function<void()> cb, bool en)
        : name(n), interval_seconds(interval), last_run_seconds(0), callback(cb), enabled(en) {}
};
}
#endif //STM32F103C8TX_COLLECTIONTASK_HPP