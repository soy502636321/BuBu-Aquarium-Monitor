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

#define RX_BUF_SIZE  128
#define TX_BUF_SIZE  128
#define UART_HANDLE huart1

class UartChannel : public IAction {
public:

    ChannelDirection getDirection() const override {
        return ChannelDirection::BIDIR;
    }

    static UartChannel& getInstance() {
        static UartChannel instance;
        return instance;
    }

public:
    // 禁止拷贝和赋值
    UartChannel(const UartChannel&) = delete;
    UartChannel& operator=(const UartChannel&) = delete;

    // ==================== IDataChannel ====================

    uint8_t* getRxBuffer() {
        return m_rx_buf;
    }

    void onDataReceived() {
        // 只要环形缓冲区有数据，就尝试解析
        if  (m_rx_count > 0) {
            //1. 查找帧头 0xAA 0x55
            uint16_t header_pos = findRxHeader();
            //2. 没找到帧头，保留数据等下次
            if (header_pos == 0xFFFF) {
                return;
            };
            //3. 检查是否有足够的字节读取长度
            if (m_rx_count - header_pos < 1 + 1 + 3) {  // AA 55 + 数据类型 + 协议版本 + 数据长度
                return;  // 数据不够，等下次
            }
            //4. 读取长度字段（第3个字节，索引2）
            uint8_t data_len = peekByte(header_pos + 1 + 1 + 2);
            uint16_t data_total_len = 1 + 1 + 1 + 1 + 1 + data_len + 2;  // AA + 55 + Type + Ver + Len + Data + CRC
            //5. 检查完整帧是否已收到
            if (m_rx_count - header_pos < data_total_len) {
                return;  // 数据不够，等下次
            }
            //6.读取完整数据
            findRxData(header_pos, data_total_len);
            //7. 移除已提取的数据，继续接收新数据
            // removeDmaBuffer(data_total_len);

            // ✅ 触发回调：通知有数据了！
            if (m_callback) {
                // 构建data context
                // printf("Received Size %d bytes\n", size);  // %d 打印十进制
                // printf("Received Rx_Len %d bytes\n", rx_len);  // %d 打印十进制
                //
                for (uint16_t i = 0; i < data_total_len; i++) {
                    printf("%02X ", m_rx_data_buf[i]);
                }
                printf("\r\n");
                std::vector<uint8_t> data(m_rx_data_buf, m_rx_data_buf + data_total_len);
                DataContext context {(&data), data_total_len};
                m_callback(context);
            }
        }

    }

    void onRxIdleDMA(uint16_t size) {
        // 计算实际接收长度
        uint16_t rx_len = RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(static_cast<UART_HandleTypeDef*>(&UART_HANDLE)->hdmarx);
        if (rx_len >= m_rx_tail) {
            m_rx_count = rx_len - m_rx_tail;
        } else {
            // 绕环了
            m_rx_count = (RX_BUF_SIZE - m_rx_tail) + rx_len;
        }
        onDataReceived();
        // ★★★ 关键：重新启动 DMA 空闲接收 ★★★
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, m_rx_buf, RX_BUF_SIZE);
    }

    void onTxComplete() {
        m_tx_busy = false;
        if (m_tx_count > 0) {
            sendNext();
        }
    }

private:

    UartChannel()
        : m_huart(&UART_HANDLE)
        , m_connected(false)
        , m_tx_busy(false)
        , m_tx_head(0)
        , m_tx_tail(0)
        , m_tx_count(0) {
        // 保险起见 休闲中断 开启首次DMA
        __HAL_UART_ENABLE_IT(&UART_HANDLE, UART_IT_IDLE);
        HAL_UARTEx_ReceiveToIdle_DMA(
            &UART_HANDLE,
            getRxBuffer(),
            RX_BUF_SIZE
        );
    }

    ~UartChannel() = default;

    uint8_t peekByte(uint16_t offset) {
        if (offset >= m_rx_count) return 0;
        uint16_t index = (m_rx_tail + offset) % RX_BUF_SIZE;  // ✅ 计算物理位置
        return m_rx_buf[index];
    }

    uint16_t findRxHeader() {
        if (m_rx_count < 2) return 0xFFFF;
        for (uint16_t i = 0; i < m_rx_count - 1; i++) {
            uint8_t byte1 = peekByte(i);
            uint8_t byte2 = peekByte(i + 1);
            if (byte1 == 0xAA && byte2 == 0x55) {
                return i;  // 找到帧头位置
            }
        }
        return 0xFFFF;  // 没找到
    }

    void findRxData(uint16_t start_pos, uint16_t len) {
        for (uint16_t i = 0; i < len; i++) {
            // 从环形缓冲区读取，从 start_pos 开始
            uint16_t index = (m_rx_tail + start_pos + i) % RX_BUF_SIZE;
            m_rx_data_buf[i] = m_rx_buf[index];  // 存到帧缓冲区
        }
        m_rx_tail = (m_rx_tail + len) % RX_BUF_SIZE;// tail 向后移动 len
        m_rx_count -= len;
        m_rx_data_len = len;
    }

    // void removeDmaBuffer(uint16_t count) {
    //     if (count > m_rx_count) {
    //         count = m_rx_count;  // 限制最大移除数量
    //     }
    //     m_rx_tail = (m_rx_tail + count) % RX_BUF_SIZE;
    //     m_rx_count -= count;
    // }

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

    // 回调
    // std::function<void(const uint8_t*, uint16_t)> m_callback;

    // 状态
    bool m_connected;
    bool m_tx_busy;

    // 接收缓冲区
    uint8_t m_rx_buf[RX_BUF_SIZE]{0};
    uint16_t m_rx_head = 0;
    uint16_t m_rx_tail = 0;
    uint16_t m_rx_count = 0;
    // 完整数据
    uint8_t m_rx_data_buf[RX_BUF_SIZE]{0};
    uint16_t m_rx_data_len = 0;

    // 发送缓冲区
    uint8_t m_dma_tx_buf[TX_BUF_SIZE]{};
    uint16_t m_tx_head;
    uint16_t m_tx_tail;
    uint16_t m_tx_count;
};

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    // printf("HAL_UARTEx_RxEventCallback\r\n");
    if (huart->Instance == USART1) {
        // 通知 UartChannel
        UartChannel::getInstance().onRxIdleDMA(Size);
    }
}
// ★★★ 错误回调 ★★★
extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // printf("HAL_UART_ErrorCallback\r\n");
    if (huart->Instance == USART1) {
        // 清除错误标志
        __HAL_UART_CLEAR_OREFLAG(huart);
        // 重新启动
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, UartChannel::getInstance().getRxBuffer(), RX_BUF_SIZE);
    }
}

#endif // UART_CHANNEL_HPP