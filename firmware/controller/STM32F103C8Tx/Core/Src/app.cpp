#include "CollectionTask.hpp"
#include "main.h"
#include "stdio.h"

#include "DataGateway.hpp"
#include "TimerScheduler.hpp"
#include "Device.hpp"

using namespace DataGateway;
using namespace Components;

#ifdef __cplusplus
extern "C" {
#endif

    extern TIM_HandleTypeDef htim1;  // 全局传感器定时采集调度定时器
    extern TIM_HandleTypeDef htim2;

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if (htim->Instance == TIM1) {
            Components::TimerScheduler::getInstance().onTick();
        }
    }

    void app_main_entry(void) {
        HAL_TIM_Base_Start_IT(&htim1);
        HAL_TIM_Base_Start_IT(&htim2);

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

            ValveContext context;
            context.packet = nullptr;
            DataGateway::DataGateway::getInstance().transmit(context);
        });
        timerScheduler.start();

        while (1) {
            printf("Counter While:\r\n");
            HAL_Delay(1000);
        }
    }

#ifdef __cplusplus
}
#endif