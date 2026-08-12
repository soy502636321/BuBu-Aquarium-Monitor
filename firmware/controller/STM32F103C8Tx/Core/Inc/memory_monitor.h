// memory_monitor.h
#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

    void MemoryMonitorTask(void *argument);
    void CreateMemoryMonitorTask(void);

#ifdef __cplusplus
}
#endif

#endif