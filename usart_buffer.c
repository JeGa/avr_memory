#include "usart_buffer.h"
#include "memory.h"

#include <string.h>

// Message

message *getMessage(int dataSize)
{
	message *msg;
	
	if (dataSize <= 0)
		return 0;
		
	msg = allocateMemory(sizeof(message));
	
	if (msg == 0)
		return msg;
		
	msg->data = allocateMemory(dataSize);
	
	if (msg->data == 0) {
		freeMemory(msg);
		return 0;
	}
	
	msg->size = dataSize;
	msg->stackIndex = 0;
	msg->next = 0;
	
	return msg;
}

void destroyMessage(message *msg)
{
	if (msg == 0)
		return;
	
	if (msg->data)
		freeMemory(msg->data);
		
	freeMemory(msg);
}

int setMessageData(message *msg, char *data, int size)
{
	if (msg == 0)
		return 0;
	
	if (size > msg->size || size <= 0)
		return 0;
		
	memcpy(msg->data, data, size);
	
	return 1;
}

char *getMessageData(message *msg)
{
	if (msg == 0)
		return 0;
	
	return msg->data;
}

char popMessageData(message *msg)
{
	if (msg == 0)
		return 0;
	
	if (msg->stackIndex == msg->size)
		return 0; // "Empty"
	
	char data = msg->data[msg->stackIndex];
	++msg->stackIndex;
	
	return data;
}

char isMessageStackEmpty(message *msg)
{
	if (msg == 0)
		return 0;
		
	if (msg->stackIndex == msg->size)
		return 1; // "Empty"
		
	return 0;
}

// MessageQueue

messageQueue *getMessageQueue()
{
	messageQueue *queue = allocateMemory(sizeof(messageQueue));
	
	if (queue != 0) {
		queue->tail = 0;
		queue->head = 0;
		queue->size = 0;
	}
	
	return queue;
}

void destroyMessageQueue(messageQueue *queue)
{
	if (queue == 0)
		return;
		
	if (queue->size > 0) {	
		message *tmp = queue->tail;
		
		while (tmp) {
			message *next = tmp->next;
			
			destroyMessage(tmp);
			tmp = next;
		}
	}
	
	freeMemory(queue);
}

int isMessageQueueEmpty(messageQueue *queue)
{
	if (queue->size == 0)
		return 1;
	return 0;
}

void enqueue(messageQueue *queue, message *msg)
{
	if (msg == 0)
		return;
		
	if (queue == 0)
		return;
		
	if (queue->size == 0) {
		// First element to enqueue
		queue->head = msg;
		queue->tail = msg;
	} else {
		queue->head->next = msg;
		queue->head = msg;
	}
	
	++queue->size;
}

message *dequeue(messageQueue *queue)
{
	message *tmp;
	
	if (queue == 0)
		return 0;
		
	if (queue->size == 0)
		return 0;
	
	tmp = queue->tail;
	queue->tail = queue->tail->next;
	
	// If it was the last element
	if (queue->tail == 0)
		queue->head = 0;
	
	tmp->next = 0;
	--queue->size;
	
	return tmp;
}