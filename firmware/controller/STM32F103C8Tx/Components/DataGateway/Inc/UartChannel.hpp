//
// Created by Hu on 2026-08-03.
//
#ifndef UART_CHANNEL_HPP
#define UART_CHANNEL_HPP
#include "ActionExecutor.hpp"
#include "ObjectPool.h"
#include "Logger.hpp"

extern "C" {
    #include "stm32f1xx_hal.h"
}

#include "Channel.hpp"
#include <malloc.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define RX_BUF_SIZE  128
#define TX_BUF_SIZE  128
#define UART_HANDLE huart3

extern uint32_t _estack;  // 栈顶（在链接脚本中定义）
uint32_t* stackTop = &_estack;

extern char _heap_start;
extern char _heap_end;

static DataContext context;
static bool context_valid = false;

static ObjectPool<DataContext, 8> g_data_context_pool;

class UartChannel : public IChannel {
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
        LOG_INFO("TEST -DATA \r\n");
        // printf("get DATA %d \r\n", m_rx_count);
        // 只要环形缓冲区有数据，就尝试解析
        if  (m_rx_count > 0) {
            //1. 查找帧头 0xAA 0x55
            uint16_t header_pos = findRxHeader();
            //2. 没找到帧头，保留数据等下次
            if (header_pos == 0xFFFF) {
                LOG_WARN("没找到帧头，保留数据等下次\r\n");
                return;
            };
            //3. 检查是否有足够的字节读取长度
            if (m_rx_count - header_pos < 1 + 1 + 3) {  // AA 55 + 协议版本 + 数据类型 + 数据长度
                LOG_WARN("数据不够\r\n");
                return;  // 数据不够，等下次
            }
            //4. 读取长度字段（第3个字节，索引2）
            uint8_t data_len = peekByte(header_pos + 1 + 1 + 2);
            uint16_t data_total_len = 1 + 1 + 1 + 1 + 1 + data_len + 2;  // AA + 55 + Type + Ver + Len + Data + CRC
            //5. 检查完整帧是否已收到
            if (m_rx_count - header_pos < data_total_len) {
                // printf("NOT COMPTED\r\n");
                return;  // 数据不够，等下次
            }
            //6.读取完整数据
            findRxData(header_pos, data_total_len);

            DataContext* ctx = g_data_context_pool.allocate();
            ctx->packet.setPayload(m_rx_data_buf, data_total_len);
            ctx->packet.setLength(data_total_len);
            // 3. ★ 交给 DataGateway 入队（不再做任何解析） ★
            // DataGateway::getInstance().onReceiveFromISR(ctx);

            // ✅ 触发回调：通知有数据了！
            // if (m_callback) {
            //     for (uint16_t i = 0; i < data_total_len; i++) {
            //         printf("%02X ", m_rx_data_buf[i]);
            //     }
            //     printf("\r\n");
            //     uint32_t sp;
            //     __asm volatile("MOV %0, SP" : "=r"(sp));
            //     uint32_t stackUsed = (uint32_t)&_estack - sp;
            //     printf("Stack used: %u bytes\r\n", stackUsed);
            //     if (stackUsed > 2000) {  // 如果栈使用超过2KB
            //         printf("!!! Stack overflow risk !!!\r\n");
            //     }
            //
            //     struct mallinfo mi = mallinfo();
            //     printf("Heap used: %u bytes\r\n", mi.uordblks);
            //     printf("Heap free: %u bytes\r\n", mi.fordblks);
            //     printf("Heap total: %u bytes\r\n", mi.arena);
            //
            //     // DataContext context(m_rx_data_buf, data_total_len);
            //     // if (!context_valid) {
            //     //     context.packet.setPayload(m_rx_data_buf, data_total_len);
            //     //     context_valid = true;
            //     // }
            //     DataContext* ctx = g_data_context_pool.allocate();
            //     ctx->packet.setPayload(m_rx_data_buf, data_total_len);
            //
            //     printf("new CONTEXT (stack)\r\n");
            m_callback(*ctx);
            g_data_context_pool.release(ctx);
            // }
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
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE,getRxBuffer(),RX_BUF_SIZE);
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
        if (len > 0) {
            for (uint16_t i = 0; i < len; i++) {
                // 从环形缓冲区读取，从 start_pos 开始
                uint16_t index = (m_rx_tail + start_pos + i) % RX_BUF_SIZE;
                m_rx_data_buf[i] = m_rx_buf[index];  // 存到帧缓冲区
            }
            m_rx_head = (m_rx_head + len) % RX_BUF_SIZE;
            m_rx_count -= len;
            m_rx_data_len = len;
// ========== 加入日志追踪 ==========
// printf("[Buffer] === State Update ===\n");
// printf("  len = %u\n", (unsigned int)len);
// printf("  m_rx_head = %u\n", (unsigned int)m_rx_head);
// printf("  m_rx_count = %u (was %u)\n",
       // (unsigned int)m_rx_count,
       // (unsigned int)(m_rx_count + len));  // 显示更新前的值
// printf("  m_rx_data_len = %u\n", (unsigned int)m_rx_data_len);
// printf("  m_rx_tail = %u\n", (unsigned int)m_rx_tail);
// fflush(stdout);
        }


    }

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
    if (huart->Instance == USART3) {
        // 通知 UartChannel
        UartChannel::getInstance().onRxIdleDMA(Size);
    }
}
// ★★★ 错误回调 ★★★
extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        // 清除错误标志
        __HAL_UART_CLEAR_OREFLAG(huart);
        // 重新启动
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, UartChannel::getInstance().getRxBuffer(), RX_BUF_SIZE);
    }
}

#endif // UART_CHANNEL_HPP