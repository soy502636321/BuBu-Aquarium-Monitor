#include "main.h"
#include "stdio.h"

#include "DataGateway.hpp"
#include "Device.hpp"
#include "SensorManager.hpp"

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
        // LOG_INFO("Sensor task executed at tick: %lu",  TimerScheduler::getInstance().getTickSeconds());
    }

    void main_bridge(void) {
        printf("START 1.2 ...\r\n");
        // ★ 只创建这一个任务 ★
        // xTaskCreate(TestTask, "Test", 128, NULL, 1, NULL);

        ChannelManager::getInstance().init();
        DataGateway::getInstance().init();
        ActionExecutor::getInstance().init();
        SensorManager::getInstance().init();
    }
#ifdef __cplusplus
}
#endif