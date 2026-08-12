// memory_monitor.cpp
#include "memory_monitor.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "stm32f1xx_hal.h"
#include "Config.h"
#include "main.h"

// 外部符号
extern "C" {
    extern uint32_t _estack;
    extern uint32_t _sdata;
    extern uint32_t _edata;
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    extern uint32_t _end;
}

static volatile uint8_t g_monitorFlags = 0;
#define MONITOR_FLAG_STACK_OVERFLOW  (1 << 0)
static char g_overflowTaskName[16] = {0};

// ============================================================
// ★ ★ 栈溢出检测钩子 ★ ★
// ============================================================
extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    g_monitorFlags |= MONITOR_FLAG_STACK_OVERFLOW;
    strncpy(g_overflowTaskName, pcTaskName, sizeof(g_overflowTaskName) - 1);
    g_overflowTaskName[sizeof(g_overflowTaskName) - 1] = '\0';
}

// ============================================================
// ★ ★ 内存监控任务 ★ ★
// ============================================================
void MemoryMonitorTask(void *argument) {
    uint32_t printCount = 0;

    // 计算内存大小
    uint32_t ram_start = 0x20000000;
    uint32_t ram_end = (uint32_t)&_estack;
    uint32_t total_ram = ram_end - ram_start;
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    size_t totalFreeRTOSHeap = configTOTAL_HEAP_SIZE;

    // ★ 精简：只显示一次初始化信息
    printf("\r\n");
    printf("=== MEMORY MONITOR ===\r\n");
    printf("RAM: %lu B (%.1f KB), .data: %lu B, .bss: %lu B\r\n",
           total_ram, (float)total_ram / 1024.0f, data_size, bss_size);
    printf("FreeRTOS Heap: %lu B (%.1f KB)\r\n",
           totalFreeRTOSHeap, (float)totalFreeRTOSHeap / 1024.0f);
    printf("======================\r\n\n");

    size_t initialFree = xPortGetFreeHeapSize();
    uint32_t maxUsed = totalFreeRTOSHeap - initialFree;
    uint32_t minFree = initialFree;

    while (1) {
        // ★ 检查栈溢出
        if (g_monitorFlags & MONITOR_FLAG_STACK_OVERFLOW) {
            g_monitorFlags &= ~MONITOR_FLAG_STACK_OVERFLOW;
            printf("\n!!! STACK OVERFLOW: %s !!!\n", g_overflowTaskName);
            while (1) {
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }

        // 获取内存信息
        size_t freeHeap = xPortGetFreeHeapSize();
        size_t minFreeHeap = xPortGetMinimumEverFreeHeapSize();
        size_t usedHeap = totalFreeRTOSHeap - freeHeap;

        if (usedHeap > maxUsed) maxUsed = usedHeap;
        if (freeHeap < minFree) minFree = freeHeap;

        // ★ ★ ★ 精简输出 ★ ★ ★
        uint32_t usedPercent = (totalFreeRTOSHeap > 0) ?
            (usedHeap * 100) / totalFreeRTOSHeap : 0;
        uint32_t totalUsed = data_size + bss_size;
        uint32_t overallPercent = total_ram > 0 ? (totalUsed * 100) / total_ram : 0;

        // ★ 只有 4 行关键信息
        printf("[%lu] Heap: %lu/%lu B (%lu%%), RAM: %lu%%, Tasks: %lu, Tick: %lu\r\n",
               printCount++,
               freeHeap, totalFreeRTOSHeap, usedPercent,
               overallPercent,
               uxTaskGetNumberOfTasks(),
               xTaskGetTickCount());

        // ★ 低内存警告
        if (freeHeap < 512) {
            printf("*** CRITICAL: FreeRTOS heap < 512 B! ***\n");
        }

        vTaskDelay(pdMS_TO_TICKS(MEMORY_MONITOR_INTERVAL));
    }
}

void CreateMemoryMonitorTask(void) {
    xTaskCreate(MemoryMonitorTask, "Mon", 256, NULL, 0, NULL);
}