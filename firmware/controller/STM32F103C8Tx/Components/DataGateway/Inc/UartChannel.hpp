//
// Created by Hu on 2026-08-03.
//
#ifndef UART_CHANNEL_HPP
#define UART_CHANNEL_HPP

extern "C" {
    #include "stm32f1xx_hal.h"
}

#include "Channel.hpp"
#include <cstring>

extern UART_HandleTypeDef huart3;

#define RX_BUF_SIZE  12
#define TX_BUF_SIZE  12
#define UART_HANDLE huart3

class UartChannel : public IBidirChannel {
public:
    // 禁止拷贝和赋值
    UartChannel(const UartChannel&) = delete;
    UartChannel& operator=(const UartChannel&) = delete;

    // 获取单例实例
    static UartChannel& getInstance() {
        static UartChannel instance;
        return instance;
    }

    // ==================== IChannel ====================
    ChannelType getType() const override {
        return ChannelType::UART;
    }

    bool isConnected() const override {
        return m_connected;
    }

    // ==================== IOutputChannel ====================
    bool send(const uint8_t* data, uint16_t length) override {
        if (!m_connected || data == nullptr || length == 0) {
            return false;
        }

        if (m_tx_count + length > TX_BUF_SIZE) {
            return false;
        }

        for (uint16_t i = 0; i < length; i++) {
            m_tx_buf[m_tx_tail] = data[i];
            m_tx_tail = (m_tx_tail + 1) % TX_BUF_SIZE;
            m_tx_count++;
        }

        if (!m_tx_busy) {
            return sendNext();
        }
        return true;
    }

    bool send(const char* str) override {
        if (str == nullptr) {
            return false;
        }
        return send(reinterpret_cast<const uint8_t*>(str), std::strlen(str));
    }

    bool send(const DataContext& ctx) override {
        // return send(ctx.data, ctx.length);
        return false;
    }

    // ==================== IInputChannel ====================
    uint16_t read(uint8_t* buffer, uint16_t max_len) override {
        if (buffer == nullptr || max_len == 0) {
            return 0;
        }

        uint16_t read_count = 0;
        while (read_count < max_len && m_rx_count > 0) {
            buffer[read_count] = m_rx_buf[m_rx_tail];
            m_rx_tail = (m_rx_tail + 1) % RX_BUF_SIZE;
            m_rx_count--;
            read_count++;
        }
        return read_count;
    }

    bool hasData() const override {
        return m_rx_count > 0;
    }

    void flush() override {
        m_rx_head = 0;
        m_rx_tail = 0;
        m_rx_count = 0;
        m_tx_head = 0;
        m_tx_tail = 0;
        m_tx_count = 0;
        m_tx_busy = false;
    }

    // ==================== UART 特有 ====================
    void setHardware(void* huart) {
        m_huart = huart;
    }

    void* getHardware() const {
        return m_huart;
    }

    void connect() {
        if (m_huart == nullptr) {
            return;
        }
        m_connected = true;
    }

    void disconnect() {
        m_connected = false;
    }

    void onRxData(uint8_t data) {
        printf("Test Reve!\r\n");
        if (m_rx_count < RX_BUF_SIZE) {
            m_rx_buf[m_rx_head] = data;
            m_rx_head = (m_rx_head + 1) % RX_BUF_SIZE;
            m_rx_count++;
        }
    }

    void onTxComplete() {
        m_tx_busy = false;
        if (m_tx_count > 0) {
            sendNext();
        }
    }

private:
    // 私有构造函数
    UartChannel()
        : m_huart(&UART_HANDLE)
        , m_connected(false)
        // , m_enabled(true)
        , m_tx_busy(false)
        , m_rx_head(0)
        , m_rx_tail(0)
        , m_rx_count(0)
        , m_tx_head(0)
        , m_tx_tail(0)
        , m_tx_count(0) {
        std::memset(m_rx_buf, 0, sizeof(m_rx_buf));
        std::memset(m_tx_buf, 0, sizeof(m_tx_buf));
        printf("Enable HAL_UARTEx_ReceiveToIdle_DMA\r\n");
        HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(&huart3, m_rx_buf, sizeof(m_rx_buf));
        if (status != HAL_OK) {
            // 在这里处理启动失败的情况，例如点亮一个错误LED
            // 或者通过串口打印一条错误信息（但此时 printf 可能还没好）
            printf("ERROR HAL_UARTEx_ReceiveToIdle_DMA\r\n");
        }
    }

    ~UartChannel() = default;

    bool sendNext() {
        if (m_tx_count == 0) {
            m_tx_busy = false;
            return true;
        }

        uint8_t data = m_tx_buf[m_tx_head];
        m_tx_head = (m_tx_head + 1) % TX_BUF_SIZE;
        m_tx_count--;
        m_tx_busy = true;

        // TODO: 硬件发送
        return true;
    }

private:
    // 硬件
    void* m_huart;

    // 状态
    bool m_connected;
    bool m_tx_busy;

    // 接收缓冲区
    uint8_t m_rx_buf[RX_BUF_SIZE]{};
    uint16_t m_rx_head;
    uint16_t m_rx_tail;
    uint16_t m_rx_count;

    // 发送缓冲区
    uint8_t m_tx_buf[TX_BUF_SIZE]{};
    uint16_t m_tx_head;
    uint16_t m_tx_tail;
    uint16_t m_tx_count;
};

// extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
//     printf("HAL_UARTEx_RxEventCallback start\r\n");
//     if (huart->Instance == USART3) {
//         // 通知 UartChannel
//         UartChannel::getInstance().onRxData(Size);
//     }
// }

#endif // UART_CHANNEL_HPP