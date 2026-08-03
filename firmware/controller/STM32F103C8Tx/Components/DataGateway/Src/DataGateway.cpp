// DataGateway/src/DataGateway.cpp
#include "DataGateway.hpp"
#include <cstdio>

#include "UartChannel.hpp"

// -------- 初始化 --------
void DataGateway::init() {
    if (m_initialized) {
        return;
    }
    setupRxPipeline();
    setupTxPipeline();
    m_initialized = true;
    printf("[DataGateway] initialized complete\n");
}

// -------- 设置 Rx Pipeline --------
void DataGateway::setupRxPipeline() {
    // m_rx_pipeline->clearValves();
    // 添加输入阀门
    InputValve inputValve;
    UartChannel& uart_channel = UartChannel::getInstance();
    inputValve.addChannel(&uart_channel); // 添加串口输入
    m_rx_pipeline->addValve(&inputValve);
}

// -------- 设置 Tx Pipeline --------
void DataGateway::setupTxPipeline() {
    m_tx_pipeline->clearValves();
    // TODO: 添加阀门
    // pipeline.addValve(new LogValve());
}

// -------- 传输 --------
void DataGateway::transmit(const DataContext &content) {
    // if (!initialized) {
    //     printf("[DataGateway] Not initialized!\n");
    //     return;
    // }

    printf("[DataGateway] Transmitting...\n");

    // if (content.packet != nullptr) {
    //     printf("  Packet: seq=%u, data_size=%zu\n",
    //            content.packet->getSequence(),
    //            content.packet->getData().size());
    // }

    // 执行 pipeline
    // ValveContext& ctx = const_cast<ValveContext&>(content);
    // bool result = pipeline.execute(ctx);
    //
    // printf("[DataGateway] Transmit %s\n", result ? "SUCCESS" : "FAIL");
}
