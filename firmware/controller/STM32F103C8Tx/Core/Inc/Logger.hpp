//
// Created by Hu on 2026/8/9.
//

#ifndef STM32F103C8TX_LOGGER_HPP
#define STM32F103C8TX_LOGGER_HPP

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// ============================================================
// 日志级别定义
// ============================================================
#define LOG_LEVEL_NONE      0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4
#define LOG_LEVEL_VERBOSE   5

// ============================================================
// ★ 在这里切换日志级别 ★
// ============================================================
#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_DEBUG   // Debug 模式
    // #define LOG_LEVEL LOG_LEVEL_ERROR   // Release 模式
#endif

// ============================================================
// 日志宏定义
// ============================================================
#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(fmt, ...)  printf("[ERROR] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
    #define LOG_WARN(fmt, ...)   printf("[WARN]  " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(fmt, ...)   printf("[INFO]  " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(fmt, ...)  printf("[DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
    #define LOG_VERBOSE(fmt, ...) printf("[VERB]  " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG_VERBOSE(fmt, ...)
#endif

// ============================================================
// ★ 带时间和任务名的日志（调试更友好）★
// ============================================================
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG_TASK(fmt, ...) \
        printf("[%s][%lu] " fmt "\r\n", \
               pcTaskGetName(NULL), \
               (unsigned long)xTaskGetTickCount(), \
               ##__VA_ARGS__)
#else
    #define LOG_DEBUG_TASK(fmt, ...)
#endif

// ============================================================
// ★ 条件日志（只在特定条件下打印）★
// ============================================================
#define LOG_IF(condition, fmt, ...) \
    do { if (condition) { printf("[COND] " fmt "\r\n", ##__VA_ARGS__); } } while(0)

#endif //STM32F103C8TX_LOGGER_HPP