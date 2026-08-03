#include "CollectionTask.hpp"
#include "main.h"
#include "stdio.h"

#include "DataGateway.hpp"
#include "TimerScheduler.hpp"
#include "Device.hpp"

using namespace Components;

#ifdef __cplusplus
extern "C" {
#endif

    extern TIM_HandleTypeDef htim1;  // 全局传感器定时采集调度定时器
    extern TIM_HandleTypeDef htim2;
    extern UART_HandleTypeDef huart1;
    extern UART_HandleTypeDef huart3;

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if (htim->Instance == TIM1) {
            Components::TimerScheduler::getInstance().onTick();
        }
    }

    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
        if (huart->Instance == USART1) {
            // printf("[TEST] TxCpltCallback executed!\r\n");
            // 注意：gState 已经在 HAL_UART_IRQHandler 中被重置为 READY 了
            // 这里只需要处理你自己的业务逻辑
        }
    }

    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
        // 1. 判断是哪个串口触发了回调（如果你使用了多个串口）
        if (huart->Instance == USART1) {
            printf("[TEST] RxEventCallback executed!\r\n");
            // 2. 【重要】处理接收到的数据
            // Size 是本次实际接收到的字节数，这是空闲中断的好处
            // 你的数据存放在 m_rx_buf 中，长度为 Size
            // 这里可以调用你的数据处理函数，比如解析协议、放入队列等
            // 例如：processReceivedData(m_rx_buf, Size);

            // 3. 【非常重要】重新启动接收，为下一帧数据做准备
            // 这正是你之前“只收到一次”的原因所在
            // 再次调用此函数，DMA 接收会立即重新开始
            // HAL_UARTEx_ReceiveToIdle_DMA(huart, m_rx_buf, sizeof(m_rx_buf));
        }
    }

    void app_main_entry(void) {
        HAL_TIM_Base_Start_IT(&htim1);
        HAL_TIM_Base_Start_IT(&htim2);

        // DataGateway::getInstance().init();

        auto& timerScheduler = Components::TimerScheduler::getInstance();

        // 注册临时任务，用于测试数据发送处理
        timerScheduler.addTask("TEST", 10, []() {
            printf("[TEST] Task executed!\r\n");
            // 你的任务代码
            DeviceRecord record;
            record.device_id = "sensor_01";
            record.device_name = "水温传感器";

            // 添加数据点
            DataPoint point;
            point.setValue<float>(25.5);
            record.addDataPoint(point);

            DataContext context;
            context.packet = nullptr;
            DataGateway::DataGateway::getInstance().transmit(context);
        });
        timerScheduler.start();


        uint8_t  m_rx_buf[2] = {};
        HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(&huart3, m_rx_buf, sizeof(m_rx_buf));
        if (status != HAL_OK) {
            // 启动失败，可以在这里添加错误处理
        }

        // 启用数据网关，通一管理数据的接收和发送
        while (1) {
            printf("TEST-8:\r\n");

            uint8_t test_msg[] = "DMA Send Test OK!\r\n";

            // 强制重置串口状态（临时解决方案）
            huart1.gState = HAL_UART_STATE_READY;

            if (HAL_UART_Transmit_DMA(&huart1, test_msg, sizeof(test_msg) - 1) != HAL_OK) {
                Error_Handler();
            } else {
                printf("SEND OK\r\n");
            }

            HAL_Delay(1000);  // 加一点延时，让发送有足够时间完成
        }
    }

#ifdef __cplusplus
}
#endif