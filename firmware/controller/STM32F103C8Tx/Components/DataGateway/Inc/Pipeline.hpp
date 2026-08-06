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
     ~ITxPipeline() = default;

    // 处理发送数据包
     bool execute(DataContext& ctx);

    // 阀门管理
    void addValve(ITxValve* valve);
     void clearValves();
     size_t getValveCount() const;
};

// ==================== Rx Pipeline ====================
class IRxPipeline {
public:
     ~IRxPipeline() = default;
    // 处理接收数据包
     bool execute(DataContext& ctx);
    // 阀门管理
     void addValve(IRxValve* valve) ;
     void clearValves();
     size_t getValveCount() const ;
};

// ==================== Tx Pipeline 默认实现 ====================
class TxPipeline : public ITxPipeline {
public:
    TxPipeline() {}
    bool execute(DataContext& ctx) {
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

    void addValve(ITxValve* valve)  {
        if (valve) {
            m_valves[m_valve_index++] = valve;
        }
    }

    void clearValves() {
    }

    size_t getValveCount() const {
        return m_valve_index;
    }

private:
    ITxValve* m_valves[4];
    uint32_t m_valve_index = 0;
};

// ==================== Rx Pipeline 默认实现 ====================
class RxPipeline : public IRxPipeline {
public:
    RxPipeline() : m_valves{} {
    }

    bool execute(DataContext& ctx)  {
        printf("RxPipeline::execute()\r\n");
        for (auto& valve : m_valves) {
            if (!valve) {
                continue;
            }
            if (!valve->process(ctx)) {
                return false;  // 阀门处理失败
            }
            if (ctx.aborted) {
                return false;
            }
        }
        return true;
    }

    void addValve(IRxValve* valve)  {
        if (valve) {
            m_valves[m_valve_index++] = valve;
        }
    }

    void clearValves()  {
    }

    size_t getValveCount() const  {
        return m_valve_index;
    }

private:
    IRxValve* m_valves[4];
    uint32_t    m_valve_index = 0;
};

#endif // PIPELINE_HPP
