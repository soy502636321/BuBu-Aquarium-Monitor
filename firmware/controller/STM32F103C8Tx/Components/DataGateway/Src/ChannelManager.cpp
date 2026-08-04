//
// Created by Hu on 2026-08-04.
//
#include "ChannelManager.hpp"

#include "UartChannel.hpp"
#include "BleChannel.hpp"

void ChannelManager::init()
{
    if(m_initialized) {
        return;
    }
    static UartChannel uart;
    static BleChannel ble;
    addChannel(&uart);
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