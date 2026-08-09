#include "TimerTask.hpp"
#include "main.h"
#include "stdio.h"

#include "DataGateway.hpp"
#include "TimerScheduler.hpp"
#include "Device.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    extern TIM_HandleTypeDef htim1;  // 全局传感器定时采集调度定时器
    extern TIM_HandleTypeDef htim2;

    // void TestTask(void *pvParameters) {
    //     while (1) {
    //         printf("Tick: %lu\n", xTaskGetTickCount());
    //         vTaskDelay(pdMS_TO_TICKS(1000));
    //     }
    // }

    void sensorCallback(void* arg) {
        // ★ 在 FreeRTOS 定时器任务上下文中执行 ★
        // 注意：这里不能调用阻塞函数（vTaskDelay、xQueueReceive 等）
        // 但可以调用 xQueueSend (如果队列有空)
        LOG_INFO("Sensor task executed at tick: %lu",  TimerScheduler::getInstance().getTickSeconds());
    }

    void main_bridge(void) {
        printf("START 1.2 ...\r\n");
        // ★ 只创建这一个任务 ★
        // xTaskCreate(TestTask, "Test", 128, NULL, 1, NULL);

        ChannelManager::getInstance().init();
        DataGateway::getInstance().init();
        ActionExecutor::getInstance().init();

        TimerScheduler::getInstance().init();

        // 2. 添加定时任务
        TimerScheduler::getInstance().addTask(
            "SensorRead",           // 任务名
            5,                      // 每 5 秒执行一次
            sensorCallback,         // 回调函数
            nullptr                 // 回调参数
        );

        TimerScheduler::getInstance().start();
    }

    void app_main_entry(void) {
        printf("START 1.1 ...\r\n");
        HAL_TIM_Base_Start_IT(&htim1);
        HAL_TIM_Base_Start_IT(&htim2);

        DataGateway::getInstance().init();

        auto& timerScheduler = TimerScheduler::getInstance();
        static bool b = false;
        // 注册临时任务，用于测试数据发送处理
        // timerScheduler.addTask("TEST", 10, []() {
        //     printf("[TEST] Task executed!\r\n");
        //     // 你的任务代码
        //     DeviceRecord record;
        //     record.setDeviceId("sensor_01");
        //     record.setDeviceName("水温传感器");
        //     // // 添加数据点
        //     DataPoint point;
        //     point.setValue<float>(25.5);
        //     record.addDataPoint(point);
        //
        //     DataContext context {};
        //     context.packet.setData<DeviceRecord>(record);
        //     DataGateway::DataGateway::getInstance().transmit(context);
        //
        //     if (b) {
        //         HAL_GPIO_WritePin(SWITCH_1_GPIO_Port, SWITCH_1_Pin, GPIO_PIN_SET);
        //     } else {
        //         HAL_GPIO_WritePin(SWITCH_1_GPIO_Port, SWITCH_1_Pin, GPIO_PIN_RESET);
        //     }
        //     b = !b;
        // });
        // timerScheduler.start();

        // 启用数据网关，通一管理数据的接收和发送
        while (1) {
            HAL_Delay(5000);  // 加一点延时，让发送有足够时间完成
        }
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        // 所有定时器的处理集中在这里
        if (htim->Instance == TIM1) {
            // TimerScheduler::getInstance().onTick();
        }
    }
#ifdef __cplusplus
}
#endif