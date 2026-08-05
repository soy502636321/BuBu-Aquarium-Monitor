// DataGateway/src/DataGateway.cpp
#include "DataGateway.hpp"
#include <cstdio>

#include "ChannelManager.hpp"
#include "PacketV1DecoderRxValve.hpp"
#include "PacketV1EncoderTxValve.hpp"
#include "PacketValidatorRxValve.hpp"

// -------- 初始化 --------
void DataGateway::init() {
    if (m_initialized) {
        return;
    }
    m_channel_manager = &ChannelManager::getInstance();
    m_action_manager = &ActionManager::getInstance();
    setupRxPipeline();
    setupTxPipeline();
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
    // 添加
    static PacketV1EncoderTxValve packet_v1_encoder_tx_valve;
    m_tx_pipeline.addValve(&packet_v1_encoder_tx_valve);
}

// -------- 传输 --------
void DataGateway::transmit(DataContext &ctx) {
    if (!m_tx_pipeline.execute(ctx)) {
        // 没有通过管道
        printf("DataGateway::transmit() failed\n");
    };
    // 通过频道发送
    if (m_channel_manager) {
        m_channel_manager->onTransmit(ctx);
    }
}

void DataGateway::onReceiveData(DataContext &ctx) {
    if (!m_rx_pipeline.execute( ctx)) {
        printf("DataGateway::onReceiveData() failed\n");
    }
    // 分发动作
    if (m_action_manager) {
        m_action_manager->onDispatch(ctx);
    }
}
