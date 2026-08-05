//
// Created by Hu on 2026-08-04.
//
#include "ChannelManager.hpp"

#include "UartChannel.hpp"
#include "BleChannel.hpp"
#include "DataGateway.hpp"

void ChannelManager::init()
{
    if(m_initialized) {
        return;
    }
    UartChannel& uart_channel = UartChannel::getInstance();
    uart_channel.setCallback([this](DataContext& ctx) {
            // 通道有数据到达！
            this->onReceiveData(ctx);
    });
    addChannel(&uart_channel);

    static BleChannel ble;
    addChannel(&ble);

    for(auto channel:m_channels)
    {
        // channel->init();
    }
    m_initialized = true;
}

void ChannelManager::start()
{
    for(auto channel:m_channels)
    {
        // channel->start();
    }
}

void ChannelManager::stop()
{
    for(auto channel:m_channels)
    {
        // channel->stop();
    }
}

void ChannelManager::addChannel(
        IDataChannel* channel)
{
    if(channel) {
        m_channels[m_channel_count++] = channel;
    }
}

void ChannelManager::onReceiveData(DataContext &ctx) {
    DataGateway::getInstance().getRxPipeline().execute(ctx);
}
