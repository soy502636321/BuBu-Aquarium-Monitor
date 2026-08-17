// DataGateway/src/DataGateway.cpp
#include "DataGateway.hpp"
#include <cstdio>

#include "Config.h"
#include "ChannelManager.hpp"
#include "ObjectPool.hpp"
#include "PacketV1DecoderRxValve.hpp"
#include "PacketV1EncoderTxValve.hpp"
#include "PacketValidatorRxValve.hpp"

// -------- 初始化 --------
void DataGateway::init() {
    if (m_initialized) {
        return;
    }
    setupRxPipeline();
    setupTxPipeline();

    m_rx_pipeline_queue = xQueueCreate(2, sizeof(DataContext*));
    m_tx_pipeline_queue = xQueueCreate(2, sizeof(DataContext*));

    configASSERT(m_rx_pipeline_queue != nullptr);
    configASSERT(m_tx_pipeline_queue != nullptr);
    BaseType_t ret;

    ret = xTaskCreate(rxTaskEntry, "GatewayRx", CONFIG_STACK_GATEWAY_RX, this, 3, &m_rx_task_handle);
    if (ret != pdPASS) {
        LOG_ERROR("GatewayRx task creation FAILED! err: %d", ret);
        // ★ 如果这里失败，说明栈太大或堆内存不足 ★
        while (1);
    } else {
        LOG_INFO("GatewayRx task created, handle: 0x%p", m_rx_task_handle);
    }

    ret = xTaskCreate(txTaskEntry, "GatewayTx", CONFIG_STACK_GATEWAY_TX, this, 2, &m_tx_task_handle);
    if (ret != pdPASS) {
        LOG_ERROR("GatewayTx task creation FAILED! err: %d", ret);
        while (1);
    } else {
        LOG_INFO("GatewayTx task created, handle: 0x%p", m_tx_task_handle);
    }

    // m_channel_manager = &ChannelManager::getInstance();
    // m_action_manager = &ActionExecutor::getInstance();
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
void DataGateway::transmitFromISR(DataContext *ctx) {
    if (m_tx_pipeline_queue == nullptr) {
        if (ctx != nullptr) {
            g_tx_data_context_pool.release(ctx);
        }
        return;
    }

    if (ctx == nullptr) {
        return;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret = xQueueSendFromISR(m_tx_pipeline_queue, &ctx, &xHigherPriorityTaskWoken);

    if (ret != pdPASS) {
        LOG_WARN("TX queue full in ISR! Dropping data");
        g_tx_data_context_pool.release(ctx);
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// void DataGateway::processRxData(DataContext* ctx) {
//     printf("DataGateway::onReceiveData\r\n");
//     if (!m_rx_pipeline.execute( ctx)) {
//         printf("DataGateway::onReceiveData() failed\n");
//     }
//     // 分发动作
//     // if (m_action_manager) {
//     //     m_action_manager->onDispatch(&ctx);
//     // }
// }
// UART 中断调用：只负责数据入队
void DataGateway::onReceiveFromISR(DataContext *ctx) {
    if (m_rx_pipeline_queue == nullptr) {
        LOG_INFO("DataGateway::onReceiveFromISR(DataContext *ctx)  - 1\r\n");
        return;
    }
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(m_rx_pipeline_queue, &ctx, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    LOG_INFO("DataGateway::onReceiveFromISR(DataContext *ctx)  - 2\r\n");
}

// Rx 任务入口
void DataGateway::rxTaskEntry(void* pvParameters) {
    auto* self = static_cast<DataGateway*>(pvParameters);
    self->rxProcessLoop();
}

void DataGateway::txTaskEntry(void *pvParameters) {
    auto* self = static_cast<DataGateway*>(pvParameters);
    self->txProcessLoop();  // ★ 调用处理循环 ★
}

// Rx 处理循环
void DataGateway::rxProcessLoop() {
    DataContext* ctx = nullptr;
    // Action action;
    LOG_INFO("TEST-rxProcessLoop\r\n");

    while (1) {
        // ★ 从 Rx Pipeline 队列取数据 ★
        LOG_INFO("TEST-rxProcessLoop - 1\r\n");
        if (xQueueReceive(m_rx_pipeline_queue, &ctx, portMAX_DELAY) != pdTRUE) {
            LOG_INFO("TEST-rxProcessLoop - 2\r\n");
            continue;
        }
        printf("DataGateway::onReceiveData\r\n");
        if (!m_rx_pipeline.execute( *ctx)) {
            printf("DataGateway::onReceiveData() failed\n");
        }
        // 分发动作
        ActionExecutor::getInstance().onDispatch(ctx);
        // g_rx_data_context_pool.release(ctx);
        // ★ ★ 在这里释放 ★ ★
        LOG_DEBUG("DataContext released in Gateway");
    }
}

void DataGateway::txProcessLoop() {
    DataContext* ctx = nullptr;

    while (1) {
        if (xQueueReceive(m_tx_pipeline_queue, &ctx, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        g_tx_data_context_pool.release(ctx);
        printf("DataGateway::发送数据\r\n");
        if (ctx == nullptr) {
            continue;
        }
        // ★ 通过 Pipeline 打包
        if (!m_tx_pipeline.execute(*ctx)) {
            LOG_WARN("TX Pipeline failed!");
            g_tx_data_context_pool.release(ctx);
            continue;
        }

        // ★ 通过 UART 发送
        // UartChannel::getInstance().send(ctx);
        // 注意：UartChannel 发送完成后会释放 ctx
    }
}

bool DataGateway::processTxData(DataContext *ctx) {
}