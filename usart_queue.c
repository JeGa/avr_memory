#include "usart_queue.h"
#include "memory.h"

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