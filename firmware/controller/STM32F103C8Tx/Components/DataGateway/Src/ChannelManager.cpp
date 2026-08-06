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
    addChannel(&UartChannel::getInstance());

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
        IChannel* channel)
{
    if(channel) {
        channel->setCallback([this](DataContext& ctx) {
            // 通道有数据到达！
            this->onReceive(ctx);
        });
        m_channels[m_channel_count++] = channel;
    }
}

void ChannelManager::onReceive(DataContext &ctx) {
    DataGateway::getInstance().onReceiveData(ctx);
}

void ChannelManager::onTransmit(DataContext &ctx) {
    printf("Payload (%zu bytes): ", ctx.getPayload().size());
    std::vector<uint8_t> payload = ctx.getPayload();
    for (size_t i = 0; i < payload.size(); i++) {
        printf("%02X ", payload[i]);
    }
    printf("\n");
}
