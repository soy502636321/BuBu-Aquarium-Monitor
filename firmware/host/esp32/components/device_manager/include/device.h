/*
 * device.h
 *
 *  Created on: 2026年7月14日
 *      Author: Hu
 */

#ifndef COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_H_
#define COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_H_

#pragma once

#include <cstdint>
#include <string>

enum class DeviceType
{
    Unknown = 0,

    Sensor,
    Switch,
    PWM,

    Light,
    Fan,
    Pump,
    Heater,

    Virtual
};

enum class DeviceStatus
{
    Unknown = 0,
    Initializing,
    Ready,
    Busy,
    Warning,
    Error,
    Offline
};

class Device {
	public:
	    Device(uint16_t id,
	           const std::string &name,
	           DeviceType type)
	        : id_(id),
	          name_(name),
	          type_(type)
	    {}
	
	    virtual ~Device() = default;
	
	    //----------------------
	    // 生命周期
	    //----------------------
	    virtual bool init() = 0;
	
	    virtual bool update() = 0;
	
	    virtual void loop() = 0;
	
	    //----------------------
	    // 数据接口
	    //----------------------
	    virtual bool read() = 0;
	
	    virtual bool write() = 0;
	
	    //----------------------
	    // Getter
	    //----------------------
	    uint16_t id() const
	    {
	        return id_;
	    }
	
	    const std::string &name() const
	    {
	        return name_;
	    }
	
	    DeviceType type() const
	    {
	        return type_;
	    }
	
	    DeviceStatus status() const
	    {
	        return status_;
	    }
	
	protected:
	    void setStatus(DeviceStatus status)
	    {
	        status_ = status;
	    }
	
	private:
	    uint16_t id_;
	
	    std::string name_;
	
	    DeviceType type_;
	
	    DeviceStatus status_ = DeviceStatus::Unknown;
};

#endif /* COMPONENTS_DEVICE_MANAGER_INCLUDE_DEVICE_H_ */
