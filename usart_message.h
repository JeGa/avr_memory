/**
 * @file usart_message.h
 *
 * This file contains the message buffer
 * type and functions used for USART and the jefax shell.
 *
 * It uses the dynamic memory management for buffer memory allocation.
 */

#pragma once

typedef struct message message;

typedef enum {
    TX_MSG,
    RX_MSG
} MSG_TYPE;

// TODO: Put struct declaration in source file

struct message {
    char *data;
    int size;

    /**
     * Used if the user wants to use the message data as a stack.
     * For example to use it easier in serial communication, where only one
     * byte can be send / received at once.
     *
     * The stack access (read or write) depends on the message type.
     */
    int stackIndex;
    MSG_TYPE type;

    message *next;
};

/**
 * Allocate memory for a new message.
 *
 * @param dataSize Size of the message data part
 * @param type Defines the stack access possibilities.
 *
 * @retval 0 Error creating new message queue
 */
message *getMessage(int dataSize, MSG_TYPE type);

void destroyMessage(message *msg);

/**
 * Copies the data.
 *
 * @retval 0 Error setting the data of the message.
 */
int setMessageData(message *msg, char *data, int size);

/**
 * Returns a copy of the messages data.
 *
 * @param data Address of the pointer to the copied message data
 * @param size Size of the data
 *
 * @retval 0 Error getting the data of the message.
 */
char getMessageDataCopy(message *msg, char **data, int *size);

/**
 * Returns a pointer to the messages data.
 *
 * @retval 0 Error getting pointer to data.
 */
char *getMessageData(message *msg);

/**
 * Pseudo-pops one byte from the message data.
 *
 * The data is copied and still resides in the message
 * (So it can still be accessed).
 */
char popMessageData(message *msg);

char pushMessageData(message *msg, char data);

char isMessageStackEmpty(message *msg);

char isMessageStackFull(message *msg);

message *copyMessage(message *msg);

int getMessageStackIndex(message *msg);