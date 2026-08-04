#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>      // ✅ 添加 vector 头文件
#include <string>      // ✅ 添加 string 头文件（用于 getName()）

#include "DataContext.hpp"
#include "Valve.hpp"
#include <vector>
#include <memory>
#include <string>

// ==================== Tx Pipeline ====================
class ITxPipeline {
public:
    virtual ~ITxPipeline() = default;

    // 处理发送数据包
    virtual bool execute(DataContext& ctx) = 0;

    // 阀门管理
    virtual void addValve(ITxValve* valve) = 0;
    virtual void clearValves() = 0;
    virtual size_t getValveCount() const = 0;
};

// ==================== Rx Pipeline ====================
class IRxPipeline {
public:
    virtual ~IRxPipeline() = default;
    // 处理接收数据包
    virtual bool execute(DataContext& ctx) = 0;
    // 阀门管理
    virtual void addValve(IRxValve* valve) = 0;
    virtual void clearValves() = 0;
    virtual size_t getValveCount() const = 0;
};

// ==================== Tx Pipeline 默认实现 ====================
class TxPipeline : public ITxPipeline {
public:
    TxPipeline() {}
    bool execute(DataContext& ctx) override {
        for (auto& valve : m_valves) {
            if (!valve) continue;
            if (!valve->process(ctx)) {
                return false;  // 阀门处理失败
            }
            if (ctx.aborted) {
                return false;
            }
        }
        return true;
    }

    void addValve(ITxValve* valve) override {
        if (valve) {
            m_valves[m_valve_index++] = valve;
        }
    }

    void clearValves() override {
    }

    size_t getValveCount() const override {
        return m_valve_index;
    }

private:
    ITxValve* m_valves[4];
    uint32_t m_valve_index = 0;
};

// ==================== Rx Pipeline 默认实现 ====================
class RxPipeline : public IRxPipeline {
public:
    RxPipeline() {}

    bool execute(DataContext& ctx) override {
        for (auto& valve : m_valves) {
            if (!valve) continue;
            if (!valve->process(ctx)) {
                return false;  // 阀门处理失败
            }
            if (ctx.aborted) {
                return false;
            }
        }
        return true;
    }

    void addValve(IRxValve* valve) override {
        if (valve) {
            m_valves[m_valve_index++] = valve;
        }
    }

    void clearValves() override {
    }

    size_t getValveCount() const override {
        return m_valve_index;
    }

private:
    IRxValve* m_valves[4];
    uint32_t m_valve_index = 0;
};

#endif // PIPELINE_HPP
