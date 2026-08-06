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

    void app_main_entry(void) {
        printf("START 1.1 ...\r\n");
        HAL_TIM_Base_Start_IT(&htim1);
        HAL_TIM_Base_Start_IT(&htim2);

        DataGateway::getInstance().init();

        auto& timerScheduler = Components::TimerScheduler::getInstance();
        static bool b = false;
        // 注册临时任务，用于测试数据发送处理
        timerScheduler.addTask("TEST", 10, []() {
            printf("[TEST] Task executed!\r\n");
            // 你的任务代码
            DeviceRecord record;
            record.setDeviceId("sensor_01");
            record.setDeviceName("水温传感器");
            // // 添加数据点
            DataPoint point;
            point.setValue<float>(25.5);
            record.addDataPoint(point);

            DataContext context {};
            context.packet.setData<DeviceRecord>(&record);
            DataGateway::DataGateway::getInstance().transmit(context);

            if (b) {
                HAL_GPIO_WritePin(SWITCH_1_PIN_GPIO_Port, SWITCH_1_PIN_Pin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(SWITCH_1_PIN_GPIO_Port, SWITCH_1_PIN_Pin, GPIO_PIN_RESET);
            }
            b = !b;
        });
        timerScheduler.start();

        // 启用数据网关，通一管理数据的接收和发送
        while (1) {
            HAL_Delay(5000);  // 加一点延时，让发送有足够时间完成
        }
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        // 所有定时器的处理集中在这里
        if (htim->Instance == TIM1) {
            TimerScheduler::getInstance().onTick();
        }
    }
#ifdef __cplusplus
}
#endif