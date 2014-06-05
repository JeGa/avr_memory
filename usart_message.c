#include "usart_message.h"
#include "memory.h"

// TODO: Write own memcpy
#include <string.h> // memcpy

message *getMessage(int dataSize, MSG_TYPE type)
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
    msg->type = type;
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

char getMessageDataCopy(message *msg, char **data, int *size)
{
    if (msg == 0)
        return 0;

    *size = msg->size;
    memcpy(*data, msg->data, msg->size);

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

    if (msg->type != TX_MSG)
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

    if (msg->type != TX_MSG)
        return 0;

    if (msg->stackIndex == msg->size)
        return 1; // "Empty"

    return 0;
}

char pushMessageData(message *msg, char data)
{
    if (msg == 0)
        return 0;

    if (msg->type != RX_MSG)
        return 0;

    if (msg->stackIndex == msg->size)
        return 0; // "Full"

    msg->data[msg->stackIndex] = data;
    ++msg->stackIndex;

    return 1;
}

char isMessageStackFull(message *msg)
{
    if (msg == 0)
        return 0;

    if (msg->type != RX_MSG)
        return 0;

    if (msg->stackIndex == msg->size)
        return 1; // "Full"

    return 0;
}

message *copyMessage(message *msg)
{
    message *newMsg;

    if (msg == 0)
        return 0;

    newMsg = getMessage(msg->size, msg->type);
    if (newMsg == 0)
        return 0;

    setMessageData(newMsg, getMessageData(msg), msg->size);

    return newMsg;
}

int getMessageStackIndex(message *msg)
{
    if (msg == 0)
        return 0;

    return msg->stackIndex;
}