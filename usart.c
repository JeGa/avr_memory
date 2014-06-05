#include "usart.h"
#include "usart_ascii.h"
#include "shell.h"
#include <avr/interrupt.h>

static messageQueue *rxQueue;
static messageQueue *txQueue;

/**
 * Because you can send only one byte at once
 * via serial, this pointer is used to point to
 * the current message that should be send.
 */
static message *currentSendMessage;

/**
 * Because you can receive only one byte at once
 * via serial, this pointer is used to point to
 * the current receiving message (which is data
 * until a newline).
 */
#define MAX_RECEIVE_MSG_SIZE 50
static message *currentReceiveMessage;
static message *escapeReceiveMessage;
char del = 0;

// Prototypes
static message *stripMessage(message *msg);
static void processNewLine();
static void processData(char data);
static int processEscapeData(char data);

int initUsart()
{
    rxQueue = getMessageQueue();
    if (rxQueue == 0)
        return 0;

    txQueue = getMessageQueue();
    if (txQueue == 0)
        return 0;

    currentSendMessage = 0;
    currentReceiveMessage = 0;

    USART_PORT.OUTSET = PIN3_bm;

    USART_PORT.DIRSET = PIN3_bm; // Pin 3 as TX
    USART_PORT.DIRCLR = PIN2_bm; // Pin 2 as RX

    // Frame format: Asynchronous, no parity, 1 Stop Bit, 8 Bit data
    USART.CTRLC = USART_CHSIZE_8BIT_gc;

    // Baudrate
    USART.BAUDCTRLB = 0;
    USART.BAUDCTRLA = BSEL;

    // Enable rx and tx.
    USART.CTRLB |= USART_RXEN_bm;
    USART.CTRLB |= USART_TXEN_bm;

    // Enable receive IR
    USART.CTRLA |= USART_RXCINTLVL_LO_gc;

    return 1;
}

void stopUsart()
{
    USART.CTRLA &= ~(USART_RXCINTLVL_LO_gc | USART_DREINTLVL_LO_gc);

    USART.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);

    destroyMessageQueue(rxQueue);
    destroyMessageQueue(txQueue);
}

void sendMessageUsart(message *msg)
{
    enqueue(txQueue, msg);

    // Enable DRE IR
    USART.CTRLA |= USART_DREINTLVL_LO_gc;
}

message *receiveMessageUsart()
{
    return dequeue(rxQueue);
}

void printChar(char character)
{
    message *msgSendBack = getMessage(1, TX_MSG);
    setMessageData(msgSendBack, &character, 1);
    sendMessageUsart(msgSendBack);
}

void print(char *string)
{
    message *msg;
    int stringSize = 0;

    // Count string size
    while (string[stringSize])
        ++stringSize;

    if (stringSize == 0)
        return;

    msg = getMessage(stringSize, TX_MSG);
    setMessageData(msg, string, stringSize);
    sendMessageUsart(msg);
}

static message *stripMessage(message *msg)
{
    message *ret = getMessage(msg->stackIndex, RX_MSG);
    setMessageData(ret, getMessageData(msg), msg->stackIndex);

    return ret;
}

static void processNewLine()
{
    message *finalMsg;

    pushMessageData(currentReceiveMessage, '\0');

    // Copy message to a message with suitable size.
    finalMsg = stripMessage(currentReceiveMessage);
    enqueue(rxQueue, finalMsg);

    destroyMessage(currentReceiveMessage);
    currentReceiveMessage = 0;

    // Send new line back
    print("\r\n");
}

static void processData(char data)
{
    pushMessageData(currentReceiveMessage, data);

    printChar(data);

    if ((getMessageStackIndex(currentReceiveMessage) + 1) == MAX_RECEIVE_MSG_SIZE) {
        pushMessageData(currentReceiveMessage, '\0');
        enqueue(rxQueue, currentReceiveMessage);
        currentReceiveMessage = 0;
        print("\r\n");
    }
}

static int processEscapeData(char data)
{
    pushMessageData(escapeReceiveMessage, data);

    // When a character is received, the escape sequence is received completely.
    if ((data >= CHAR_BIG_START && data <= CHAR_BIG_END) ||
            (data >= CHAR_SMALL_START && data <= CHAR_SMALL_END)) {

        if (data == 'R') { // Received cursor position: [{ROW};{COLUMN}R
            if (del) { // Check if delete is allowed
                // Check cursor position
                int stackIndex, i = 1, rowOrColumn = 1;
                int rowChars = 0, columnChars = 0;

                stackIndex = getMessageStackIndex(escapeReceiveMessage);
                char *escapeData = getMessageData(escapeReceiveMessage);

                while (i < (stackIndex - 1)) {
                    if (escapeData[i] == ';') {
                        rowOrColumn = 0; // Switch from row to column
                    } else if (rowOrColumn) {
                        ++rowChars;
                    } else if (!rowOrColumn) {
                        ++columnChars;
                    }
                    ++i;
                }

                // TODO: This is not so nice ...
                if (!(columnChars == 1 && escapeData[rowChars + 2] <= HEADER_LIMIT))
                    printChar(DEL);

                del = 0;
            }
        }

        return 1;
    }

    return 0;
}

/**
 * Is called from the receive ISR.
 *
 * It copies the received byte into a message.
 * If the MAX_RECEIVE_MSG_SIZE is reached or if a new line is recognized,
 * the message is put into the receive queue.
 */
static void receive()
{
    char data;
    static char parseEscape = 0;

    if (currentReceiveMessage == 0) {
        currentReceiveMessage = getMessage(MAX_RECEIVE_MSG_SIZE, RX_MSG);

        if (currentReceiveMessage == 0)
            return;
    }

    data = USART.DATA;

    if (data == ESC) {
        parseEscape = 1;
        escapeReceiveMessage = getMessage(15, RX_MSG);
    } else if (parseEscape) {
        if (processEscapeData(data)) { // If finished receiving escape sequence
            destroyMessage(escapeReceiveMessage);
            parseEscape = 0;
        }
    } else if (data == CR) {
        processNewLine();
    } else if (data == DEL) {
        print(QUERY_CURSOR_POS); // Query cursor position
        del = 1;
    } else {
        processData(data);
    }
}

/**
 * Is called from the data register empty ISR.
 * When the data is send, the message is destroyed.
 */
static void send()
{
    // Disable DRE IR if buffer is empty.
    if (isMessageQueueEmpty(txQueue) && currentSendMessage == 0)
        USART.CTRLA = (USART.CTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
    else {
        char data;

        // Transmit msg from send buffer

        if (currentSendMessage == 0)
            currentSendMessage = dequeue(txQueue);

        // Send one byte from current message
        data = popMessageData(currentSendMessage);
        USART.DATA = data;

        // If message has no data left destroy it
        if (isMessageStackEmpty(currentSendMessage)) {
            destroyMessage(currentSendMessage);
            currentSendMessage = 0;
        }
    }
}

ISR(USARTC0_RXC_vect)
{
    receive();
}

ISR(USARTC0_DRE_vect)
{
    send();
}