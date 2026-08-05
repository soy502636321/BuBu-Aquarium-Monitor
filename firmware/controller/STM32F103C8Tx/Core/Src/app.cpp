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
            // context.packet = nullptr;
            DataGateway::DataGateway::getInstance().transmit(context);
        });
        timerScheduler.start();
        // 启用数据网关，通一管理数据的接收和发送
        while (1) {
            HAL_Delay(5000);  // 加一点延时，让发送有足够时间完成
        }
    }

#ifdef __cplusplus
}
#endif