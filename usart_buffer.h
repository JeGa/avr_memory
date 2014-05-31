/**
 * @file usart_buffer.h
 *
 * This file contains the message and the messageQueue buffer
 * types and functions used for USART and the jefax shell.
 *
 * It uses the dynamic memory management for buffer memory allocation.
 *
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

typedef struct message message;
typedef struct messageQueue messageQueue;

// TODO: Put struc declaration in source file

struct message
{
	char *data;
	int size;

	/**
	 * Used if the user wants to use the message data as a read-only stack.
	 * For example to use it easier in serial communication, where only one
	 * byte can be send at once.
	 */	
	int stackIndex; // Pseudo-size
	
	message *next;
};

struct messageQueue
{
	message *tail;
	message *head;
	int size;
};

/**
 * @param dataSize Size of the message data part
 *
 * @retval 0 Error creating new message queue
 */
message *getMessage(int dataSize);

void destroyMessage(message *msg);

/**
 * Copies the data.
 */
int setMessageData(message *msg, char *data, int size);

char *getMessageData(message *msg);

/**
 * Pseudo-pops one byte from the message data.
 *
 * The data is copied and still resides in the message
 * (So it can still be accessed).
 */
char popMessageData(message *msg);
char isMessageStackEmpty(message *msg);

message *copyMessage(message *msg);

/**
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