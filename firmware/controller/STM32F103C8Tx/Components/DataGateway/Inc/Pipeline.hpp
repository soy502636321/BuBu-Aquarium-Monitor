#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <vector>      // ✅ 添加 vector 头文件
#include <string>      // ✅ 添加 string 头文件（用于 getName()）

#include "Valve.hpp"

namespace DataGateway {

// 管理 Valve 链，执行所有 Valve
class Pipeline {
private:
    std::vector<IValve*> valves_;  // ✅ 使用 vector 管理所有 Valve

public:
    Pipeline() : valves_() {}

    ~Pipeline() {
        clear();
    }

    // 禁止拷贝（防止双重删除）
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    // -------- 添加 Valve --------
    void addValve(IValve* valve) {
        if (valve == nullptr) return;

        valves_.push_back(valve);
        printf("[Pipeline] Added: %s (total: %zu)\n",
               valve->getName().c_str(), valves_.size());
    }

    // -------- 清空 --------
    void clear() {
        // 删除所有 Valve
        for (IValve* valve : valves_) {
            delete valve;
        }
        valves_.clear();
    }

    // -------- 执行 --------
    bool execute(ValveContext& ctx) {
        if (valves_.empty()) {
            printf("[Pipeline] No valves!\n");
            return false;
        }

        printf("\n[Pipeline] Executing %zu valves...\n", valves_.size());

        for (IValve* valve : valves_) {
            if (!valve->invoke(ctx)) {
                printf("[Pipeline] Stopped at: %s\n",
                       valve->getName().c_str());
                return false;
            }
        }

        printf("[Pipeline] Done!\n");
        return true;
    }

    // -------- 获取阀门数量 --------
    size_t size() const {
        return valves_.size();
    }

    // -------- 获取指定阀门（可选） --------
    IValve* getValve(size_t index) const {
        if (index < valves_.size()) {
            return valves_[index];
        }
        return nullptr;
    }

    // -------- 是否为空 --------
    bool empty() const {
        return valves_.empty();
    }

    // -------- 重置所有阀门（保留结构） --------
    void reset() {
        for (IValve* valve : valves_) {
            valve->reset();  // 假设 IValve 有 reset() 方法
        }
        printf("[Pipeline] Reset all valves\n");
    }
};

} // namespace DataGateway

#endif // PIPELINE_HPP
