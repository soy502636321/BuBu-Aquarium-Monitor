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

extern UART_HandleTypeDef huart1;

#define RX_BUF_SIZE  64
#define TX_BUF_SIZE  64
#define UART_HANDLE huart1

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

    uint8_t* getDmaRxBuffer() {
        return m_dma_rx_buf;
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
            m_dma_tx_buf[m_tx_tail] = data[i];
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
            buffer[read_count] = m_dma_rx_buf[m_rx_tail];
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

    void onRxIdleDMA(uint16_t size) {
        // size 是本次接收到的总字节数
        // 但 idle 模式实际长度需要用 DMA 计数器计算
        // 注意：这里 size 参数是 HAL 传进来的，但为了精确，我们用 DMA 计数器

        // 计算实际接收长度
        UART_HandleTypeDef* huart = static_cast<UART_HandleTypeDef*>(m_huart);
        uint16_t rx_len = RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

        // 将 DMA 缓冲区数据拷贝到环形缓冲区
        for (uint16_t i = 0; i < rx_len && i < RX_BUF_SIZE; i++) {
            if (m_rx_count < RX_BUF_SIZE) {
                m_rx_buf[m_rx_head] = m_dma_rx_buf[i];
                m_rx_head = (m_rx_head + 1) % RX_BUF_SIZE;
                m_rx_count++;
            }
        }

        // ★★★ 关键：重新启动 DMA 空闲接收 ★★★
        HAL_UARTEx_ReceiveToIdle_DMA(huart, m_dma_rx_buf, RX_BUF_SIZE);
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
        , m_tx_busy(false)
        , m_rx_head(0)
        , m_rx_tail(0)
        , m_rx_count(0)
        , m_tx_head(0)
        , m_tx_tail(0)
        , m_tx_count(0) {
        std::memset(m_dma_rx_buf, 0, sizeof(m_dma_rx_buf));
        std::memset(m_dma_tx_buf, 0, sizeof(m_dma_tx_buf));
    }

    ~UartChannel() = default;

    bool sendNext() {
        if (m_tx_count == 0) {
            m_tx_busy = false;
            return true;
        }

        uint8_t data = m_dma_tx_buf[m_tx_head];
        m_tx_head = (m_tx_head + 1) % TX_BUF_SIZE;
        m_tx_count--;
        m_tx_busy = true;

        return true;
    }

private:
    // 硬件
    void* m_huart;

    // 状态
    bool m_connected;
    bool m_tx_busy;

    // 接收缓冲区
    uint8_t m_dma_rx_buf[RX_BUF_SIZE]{0};
    uint8_t m_rx_buf[RX_BUF_SIZE]{0};
    uint16_t m_rx_head;
    uint16_t m_rx_tail;
    uint16_t m_rx_count;

    // 发送缓冲区
    uint8_t m_dma_tx_buf[TX_BUF_SIZE]{};
    uint16_t m_tx_head;
    uint16_t m_tx_tail;
    uint16_t m_tx_count;
};

inline void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    printf("HAL_UARTEx_RxEventCallback\r\n");
    if (huart->Instance == USART1) {
        // 通知 UartChannel
        UartChannel::getInstance().onRxIdleDMA(Size);
    }
}
// ★★★ 错误回调 ★★★
inline void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    printf("HAL_UART_ErrorCallback\r\n");
    if (huart->Instance == USART1) {
        // 清除错误标志
        __HAL_UART_CLEAR_OREFLAG(huart);
        // 重新启动
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, UartChannel::getInstance().getDmaRxBuffer(), RX_BUF_SIZE);
    }
}

#endif // UART_CHANNEL_HPP