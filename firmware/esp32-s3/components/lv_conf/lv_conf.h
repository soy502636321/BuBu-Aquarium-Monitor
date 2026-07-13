#pragma message("******** LV_CONF.H LOADED ********")

#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR 1

#define LV_MEM_CUSTOM 1

// 引入 ESP32 堆管理头文件
#define LV_MEM_POOL_INCLUDE "esp_heap_caps.h"

// 从 PSRAM 分配内存
#define LV_MEM_POOL_ALLOC(size) heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)

// 释放内存
#define LV_MEM_POOL_FREE(ptr) heap_caps_free(ptr)

// 可选：重新分配内存
#define LV_MEM_POOL_REALLOC(ptr, size) heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)