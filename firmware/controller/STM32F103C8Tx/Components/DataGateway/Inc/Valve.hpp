// DataGateway/inc/Valve.hpp
#ifndef VALVE_HPP
#define VALVE_HPP

#include <cstdint>
#include <string>

#include "ValveContext.hpp"

namespace DataGateway {

// ==================== Valve 接口 ====================
class IValve {
public:
    virtual ~IValve() = default;

    // 核心方法：处理
    virtual bool invoke(ValveContext& ctx) = 0;

    // 获取名称
    virtual std::string getName() const = 0;

    virtual void reset() const = 0;

    // 链式管理
    virtual IValve* getNext() const = 0;
    virtual void setNext(IValve* valve) = 0;
};

// ==================== 基础 Valve（抽象类） ====================
class BaseValve : public IValve {
protected:
    IValve* next_;
    std::string name_;

public:
    BaseValve(const std::string& name = "Valve")
        : next_(nullptr)
        , name_(name) {}

    virtual ~BaseValve() = default;

    // 实现接口
    IValve* getNext() const override { return next_; }
    void setNext(IValve* valve) override { next_ = valve; }
    std::string getName() const override { return name_; }

    // 子类必须实现
    virtual bool invoke(ValveContext& ctx) override = 0;
};

} // namespace DataGateway

#endif // VALVE_HPP
