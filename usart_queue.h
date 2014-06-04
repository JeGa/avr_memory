/**
 * @file usart_queue.h
 *
 * This file contains the messageQueue buffer
 * type and functions used for USART and the jefax shell.
 *
 * It uses the dynamic memory management for buffer memory allocation.
 * Heap utilization:
 * - 6 Byte messageQueue overhead + 1 memory management
 * - 8 Byte message overhead + 1 memory management
 *
 * Example:
 * 1 messageQueue with 2 messages (10 and 20 Byte):
 * Queue (6 + [1]) + message1 (8 + 10 + [2]) + message2 (8 + 20 + [2])
 * = 57 Byte (Overhead from memory management is in squared brackets)
 */

#pragma once

#include "usart_message.h"

typedef struct messageQueue messageQueue;

struct messageQueue {
    message *tail;
    message *head;
    int size;
};

/**
 * Allocate memory for a new message queue.
 *
 * @retval 0 Error creating new message queue.
 */
messageQueue *getMessageQueue();

/**
 * Frees the messageQueue memory and the memory
 * of its elements (messages).
 */
void destroyMessageQueue(messageQueue *queue);

int isMessageQueueEmpty(messageQueue *queue);

void enqueue(messageQueue *queue, message *msg);

message *dequeue(messageQueue *queue);