// DataGateway/src/DataGateway.cpp
#include "DataGateway.hpp"
#include <cstdio>

#include "ChannelManager.hpp"
#include "PacketV1DecoderRxValve.hpp"
#include "PacketValidatorRxValve.hpp"

// -------- 初始化 --------
void DataGateway::init() {
    if (m_initialized) {
        return;
    }
    m_channel_manager = &ChannelManager::getInstance();
    setupRxPipeline();
    // setupTxPipeline();
    m_initialized = true;
}

// -------- 设置 Rx Pipeline --------
void DataGateway::setupRxPipeline() {
    // 验证阀门
    static PacketValidatorRxValve validator_rx_valve;
    m_rx_pipeline.addValve(&validator_rx_valve);
    // V1 版本解码HEX
    static PacketV1DecoderRxValve packet_v1_decoder_rx_valve;
    m_rx_pipeline.addValve(&packet_v1_decoder_rx_valve);
}

// -------- 设置 Tx Pipeline --------
void DataGateway::setupTxPipeline() {
    // m_tx_pipeline->clearValves();
    // TODO: 添加阀门
    // pipeline.addValve(new LogValve());
}

// -------- 传输 --------
void DataGateway::transmit(DataContext &ctx) {
    m_tx_pipeline.execute(ctx);
}

void DataGateway::receive(DataContext &ctx) {
    m_rx_pipeline.execute( ctx);
}
