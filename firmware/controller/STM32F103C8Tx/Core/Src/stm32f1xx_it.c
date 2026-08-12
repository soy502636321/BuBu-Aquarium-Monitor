/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "core_cm3.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
    printf("\r\n");
    printf("========================================\r\n");
    printf("[错误] HardFault_Handler 触发！\r\n");
    printf("========================================\r\n");
    fflush(stdout);

    // ============================================================
    // ★ 1. 获取栈信息 ★
    // ============================================================
    uint32_t lr_value;
    __asm volatile("MOV %0, LR" : "=r"(lr_value));

    uint32_t sp;
    if (lr_value & 0x4) {
        sp = __get_PSP();
        printf("使用进程栈 (PSP)\r\n");
    } else {
        sp = __get_MSP();
        printf("使用主栈 (MSP)\r\n");
    }
    printf("SP: 0x%08X\r\n", sp);
    printf("LR: 0x%08X\r\n", lr_value);
    fflush(stdout);

    // ============================================================
    // ★ 2. 读取栈内容 ★
    // ============================================================
    if (sp >= 0x20000000 && sp < 0x20005000) {
        uint32_t* stack = (uint32_t*)sp;
        printf("\r\n--- 寄存器 ---\r\n");
        printf("R0:  0x%08X\r\n", stack[0]);
        printf("R1:  0x%08X\r\n", stack[1]);
        printf("R2:  0x%08X\r\n", stack[2]);
        printf("R3:  0x%08X\r\n", stack[3]);
        printf("R12: 0x%08X\r\n", stack[4]);
        printf("LR:  0x%08X\r\n", stack[5]);
        printf("PC:  0x%08X  <-- 崩溃地址\r\n", stack[6]);
        printf("PSR: 0x%08X\r\n", stack[7]);
        fflush(stdout);
    } else {
        printf("[警告] 栈指针无效: 0x%08X\r\n", sp);
    }

    // ============================================================
    // ★ 3. 读取故障状态寄存器 ★
    // ============================================================
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;
    uint32_t bfar = SCB->BFAR;
    uint32_t mmar = SCB->MMFAR;

    printf("\r\n--- 故障寄存器 ---\r\n");
    printf("CFSR:  0x%08X\r\n", cfsr);
    printf("HFSR:  0x%08X\r\n", hfsr);
    printf("BFAR:  0x%08X\r\n", bfar);
    printf("MMFAR: 0x%08X\r\n", mmar);
    fflush(stdout);

    // ============================================================
    // ★ 4. 错误诊断 ★
    // ============================================================
    printf("\r\n--- 错误诊断 ---\r\n");
    if (cfsr & (1 << 0))  printf("  ✗ IACCVIOL: 指令访问违规\r\n");
    if (cfsr & (1 << 1))  printf("  ✗ DACCVIOL: 数据访问违规\r\n");
    if (cfsr & (1 << 3))  printf("  ✗ MSTKERR: 入栈时总线错误\r\n");
    if (cfsr & (1 << 8))  printf("  ✗ IBUSERR: 指令总线错误\r\n");
    if (cfsr & (1 << 9))  printf("  ✗ PRECISERR: 精确数据总线错误\r\n");
    if (cfsr & (1 << 10)) printf("  ✗ IMPRECISERR: 非精确数据总线错误\r\n");
    if (cfsr & (1 << 16)) printf("  ✗ UNDEFINSTR: 未定义指令\r\n");
    if (cfsr & (1 << 17)) printf("  ✗ INVSTATE: 无效状态\r\n");
    if (cfsr & (1 << 24)) printf("  ✗ UNALIGNED: 未对齐访问\r\n");
    if (cfsr & (1 << 25)) printf("  ✗ DIVBYZERO: 除以零\r\n");
    if (hfsr & (1 << 30)) printf("  ✗ FORCED: 由其他故障引发\r\n");
    if (hfsr & (1 << 31)) printf("  ✗ VECTBL: 向量表读取错误\r\n");
    fflush(stdout);

    // ============================================================
    // ★ 5. 内存状态 ★
    // ============================================================
    printf("\r\n--- 内存状态 ---\r\n");

    // 链接脚本符号
    extern uint32_t _estack;
    extern uint32_t _sdata;
    extern uint32_t _edata;
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    extern uint32_t _end;

    uint32_t ram_start = 0x20000000;
    uint32_t ram_end = (uint32_t)&_estack;
    uint32_t total_ram = ram_end - ram_start;
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint32_t heap_start = (uint32_t)&_end;
    uint32_t heap_size = ram_end - heap_start;

    // FreeRTOS 堆
    extern uint8_t ucHeap[];
    size_t freeHeap = xPortGetFreeHeapSize();
    size_t minFree = xPortGetMinimumEverFreeHeapSize();
    size_t totalHeap = configTOTAL_HEAP_SIZE;

    printf("RAM:       %lu / %lu bytes (%.2f KB)\r\n",
           total_ram, total_ram, (float)total_ram / 1024.0f);
    printf("  .data:   %lu bytes (%.2f KB)\r\n",
           data_size, (float)data_size / 1024.0f);
    printf("  .bss:    %lu bytes (%.2f KB)\r\n",
           bss_size, (float)bss_size / 1024.0f);
    printf("  .heap:   %lu bytes (%.2f KB)\r\n",
           heap_size, (float)heap_size / 1024.0f);
    printf("FreeRTOS Heap: %lu / %lu bytes (%.2f KB)\r\n",
           freeHeap, totalHeap, (float)freeHeap / 1024.0f);
    printf("Min Free:  %lu bytes (%.2f KB)\r\n",
           minFree, (float)minFree / 1024.0f);

    // ★ 内存使用率 ★
    uint32_t totalUsed = data_size + bss_size + (totalHeap - freeHeap);
    uint32_t usedPercent = (totalUsed * 100) / total_ram;
    printf("Total Used: %lu / %lu bytes (%.2f KB) [%lu%%]\r\n",
           totalUsed, total_ram, (float)totalUsed / 1024.0f, usedPercent);
    fflush(stdout);

    // ============================================================
    // ★ 6. 内存警告 ★
    // ============================================================
    printf("\r\n--- 内存警告 ---\r\n");
    if (freeHeap < 512) {
        printf("  ✗ CRITICAL: FreeRTOS heap < 512 bytes!\r\n");
    } else if (freeHeap < 1024) {
        printf("  ⚠ WARNING: FreeRTOS heap < 1KB!\r\n");
    } else if (freeHeap < 2048) {
        printf("  ⚠ WARNING: FreeRTOS heap < 2KB!\r\n");
    } else {
        printf("  ✓ FreeRTOS heap OK\r\n");
    }

    if (usedPercent > 95) {
        printf("  ✗ CRITICAL: Overall RAM > 95%% used!\r\n");
    } else if (usedPercent > 85) {
        printf("  ⚠ WARNING: Overall RAM > 85%% used!\r\n");
    } else {
        printf("  ✓ Overall RAM OK\r\n");
    }

    if (bss_size > 15000) {
        printf("  ⚠ WARNING: .bss > 15KB, check global variables!\r\n");
    }

    if (sp > ram_end - 256) {
        printf("  ✗ STACK OVERFLOW: SP near RAM end!\r\n");
    }
    fflush(stdout);

    // ============================================================
    // ★ 7. 当前任务信息 ★
    // ============================================================
    printf("\r\n--- 任务信息 ---\r\n");
    TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
    if (currentTask != NULL) {
        const char* taskName = pcTaskGetName(currentTask);
        printf("  Current Task: %s\r\n", taskName ? taskName : "Unknown");
        UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(currentTask);
        printf("  Stack Free: %u words (%u bytes)\r\n",
               highWaterMark, highWaterMark * 4);
        if (highWaterMark < 50) {
            printf("  ✗ Task stack nearly overflow!\r\n");
        }
    }

    UBaseType_t taskCount = uxTaskGetNumberOfTasks();
    printf("  Active Tasks: %lu\r\n", taskCount);
    fflush(stdout);

    printf("========================================\r\n");
    fflush(stdout);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  /* Explicitly clear COUNTFLAG to avoid timing jitter in CMSIS-RTOS V2 */
#if (configUSE_TICKLESS_IDLE == 0)
  (void)SysTick->CTRL;
#endif
HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
