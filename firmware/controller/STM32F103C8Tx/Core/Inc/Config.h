// config.h
#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// ★ ★ 1. 用户选择芯片（取消注释对应的） ★ ★
// ============================================================
// #define CONFIG_CHIP_F103C8
// #define CONFIG_CHIP_F103ZE
// #define CONFIG_CHIP_F407VG
#define CONFIG_CHIP_F103C8   // ← 默认使用 F103C8

// ============================================================
// ★ ★ 2. 默认配置（所有芯片通用） ★ ★
// ============================================================
// -------- 内存 --------
#define CONFIG_RAM_SIZE         20      // KB
#define CONFIG_FLASH_SIZE       64      // KB
#define CONFIG_FREERTOS_HEAP    (40 * 1024)

// -------- 内存监测 --------
#define MEMORY_MONITOR_INTERVAL     10 * 1000

// -------- 任务栈（字） --------
#define CONFIG_STACK_GATEWAY_RX     512
#define CONFIG_STACK_GATEWAY_TX     512
#define CONFIG_STACK_ACTION_EXEC    512
#define CONFIG_STACK_SENSOR_MON     512

// -------- 任务优先级 --------
#define CONFIG_PRIO_GATEWAY_RX      3
#define CONFIG_PRIO_GATEWAY_TX      2
#define CONFIG_PRIO_ACTION_EXEC     2
#define CONFIG_PRIO_SENSOR_MON      1

// -------- 队列 --------
#define CONFIG_QUEUE_RX             4
#define CONFIG_QUEUE_TX             4
#define CONFIG_QUEUE_ACTION         6

// -------- 传感器 --------
#define CONFIG_MAX_SENSOR_SIZE          4
#define CONFIG_OBJECT_POOL_SIZE     4

// ============================================================
// ★ ★ 4. 编译时检查 ★ ★
// ============================================================
#ifdef __cplusplus
static_assert(CONFIG_RAM_SIZE >= 16, "RAM too small!");
static_assert(CONFIG_FLASH_SIZE >= 32, "Flash too small!");
#endif

#endif // CONFIG_H