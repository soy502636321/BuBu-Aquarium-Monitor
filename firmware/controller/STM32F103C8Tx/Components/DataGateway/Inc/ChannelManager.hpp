//
// Created by Hu on 2026-08-04.
//

#ifndef STM32F103C8TX_CHANNELMANAGER_HPP
#define STM32F103C8TX_CHANNELMANAGER_HPP

#pragma once

#include <cstdint>
#include <vector>
#include "DataChannel.hpp"

#define MAX_CHANNEL_SIZE 8

class ChannelManager
{
public:

    static ChannelManager& getInstance()
    {
        static ChannelManager instance;
        return instance;
    }

    /**
     * 初始化所有Channel
     */
    void init();

    /**
     * 启动所有Channel
     */
    void start();


    /**
     * 停止所有Channel
     */
    void stop();

    /**
     * 添加Channel
     */
    void addChannel(IDataChannel* channel);

    /**
     * 根据类型获取Channel
     */
    template<typename T>
    T* getChannel()
    {
        for(auto channel : m_channels)
        {
            T* result = dynamic_cast<T*>(channel);
            if(result)
                return result;
        }
        return nullptr;
    }

    virtual void onReceiveData(DataContext &ctx);

private:

    ChannelManager() {
        init();
    };

    ~ChannelManager() = default;

    ChannelManager(const ChannelManager&) = delete;

    ChannelManager& operator=(const ChannelManager&) = delete;

private:
    IDataChannel* m_channels[MAX_CHANNEL_SIZE]{};
    uint8_t m_channel_count = 0;
    bool m_initialized = false;
};

#endif //STM32F103C8TX_CHANNELMANAGER_HPP