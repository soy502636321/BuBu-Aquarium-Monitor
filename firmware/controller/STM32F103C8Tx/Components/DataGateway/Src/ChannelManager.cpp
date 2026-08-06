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
                printf("ChannelManager::addChannel setCallback\r\n");

                printf("[LAMBDA] 1: entered\n");
                fflush(stdout);

                // 检查 this 指针
                printf("[LAMBDA] 2: this = 0x%p\n", this);
                fflush(stdout);

                // 检查 this 指向的内存是否可读
                volatile uint8_t* test_ptr = reinterpret_cast<volatile uint8_t*>(this);
                uint8_t test_byte = *test_ptr;  // 如果这里卡死，说明内存被破坏
                printf("[LAMBDA] 3: this->first byte = 0x%02X\n", test_byte);
                fflush(stdout);

                printf("ChannelManager::addChannel setCallback\r\n");
                fflush(stdout);

                printf("[LAMBDA] 4: before onReceive\n");
                fflush(stdout);
            this->onReceive(ctx);
        });
        m_channels[m_channel_count++] = channel;
    }
}

void ChannelManager::onReceive(DataContext &ctx) {
    printf("ChannelManager::onReceive\r\n");
    DataGateway::getInstance().onReceiveData(ctx);
}

void ChannelManager::onTransmit(DataContext &ctx) {
    printf("Payload (%zu bytes): ", ctx.packet.getLength());
    const uint8_t *payload = ctx.getPayload();
    for (size_t i = 0; i < ctx.packet.getLength(); i++) {
        printf("%02X ", payload[i]);
    }
    printf("\n");
}
