/*
 * message.cpp
 *
 *  Created on: 2026年6月29日
 *      Author: Hu
 */
 
#include "message.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "string.h"
#include "stdio.h"

// 消息结构
typedef struct {
    char text[64];
    char time[16];
    int type;
} message_t;

// 消息缓存

#define MAX_MESSAGE_SIZE 25

static message_t buffer[MAX_MESSAGE_SIZE];
static int MESSAGE_SIZE = 0;
static QueueHandle_t ui_queue = NULL; // 队列

static void message_update_queue(message_t *message) {
	
}

static void message_update_ui(message_t *message) {
	
}


void message_task(void *arg) {
	message_t message;
	
	while(true) {
		// 1.等待队列消息
		if (xQueueReceive(ui_queue, &message, portMAX_DELAY)) {
			// 缓存消息
			message_update_queue(&message);
			// UI更新
			message_update_ui(&message);
		}
	}
}




