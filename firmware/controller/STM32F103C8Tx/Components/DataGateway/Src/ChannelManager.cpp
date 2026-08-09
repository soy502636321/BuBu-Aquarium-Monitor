//
// Created by Hu on 2026-08-04.
//
#include "ChannelManager.hpp"

#include "Logger.hpp"
#include "UartChannel.hpp"
#include "BleChannel.hpp"
#include "DataGateway.hpp"

void ChannelManager::init()
{
    if(m_initialized) {
        return;
    }
    addChannel(&UartChannel::getInstance());
    // static BleChannel ble;
    // addChannel(&ble);
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
        channel->setCallback(onReceive);
        m_channels[m_channel_count++] = channel;
    }
}

void ChannelManager::onReceive(DataContext &ctx) {
    LOG_INFO("ChannelManager::onReceive\r\n");
    DataGateway::getInstance().onReceiveFromISR(&ctx);
}

void ChannelManager::onTransmit(DataContext &ctx) {
    printf("Payload (%zu bytes): ", ctx.packet.getLength());
    const uint8_t *payload = ctx.getPayload();
    for (size_t i = 0; i < ctx.packet.getLength(); i++) {
        printf("%02X ", payload[i]);
    }
    printf("\n");
}

