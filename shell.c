#include "shell.h"
#include "usart.h"
#include "usart_message.h"
#include "memory.h"

// TODO: Remove that
#include <string.h>
#include <stdlib.h>
#include "jefax_xmega128.h"

static void printHeader();
static void printNewLine();
static void parseMessage(message *msg);
static message *waitForMessage();

void shellTask()
{
    initLED();

    printNewLine();

    while (1) {
        message *msg;

        printHeader();

        // Blocking busy loop
        msg = waitForMessage();

        // Got message from queue
        parseMessage(msg);
        destroyMessage(msg);
    }

    stopUsart();
}

static void parseMessage(message *msg)
{
    char *data = getMessageData(msg);

    if (strcmp("ledOn", data) == 0) {
        setLED(0XFE);
    } else if (strcmp("ledOff", data) == 0) {
        setLED(0xFF);
    } else if (strcmp("memDump", data) == 0) {
        char out[80] = {0};

        char heapStart[10] = {0};
        char nextFreeMemory[10] = {0};
        char heapAllocated[10] = {0};
        char freeListEntries[10] = {0};

        memoryInfo info = dumpMemory();

        itoa(info.heapAllocated, heapAllocated, 10);
        itoa(info.freeListEntries, freeListEntries, 10);
        itoa((int) info.heapStart, heapStart, 10);
        itoa((int) info.nextFreeMemory, nextFreeMemory, 10);

        strcat(out, "Allocated: ");
        strcat(out, heapAllocated);
        strcat(out, " FreeListEntries: ");
        strcat(out, freeListEntries);
        strcat(out, " HeapStart: ");
        strcat(out, heapStart);
        strcat(out, " NextFreeMemory: ");
        strcat(out, nextFreeMemory);

        print(out);
        printNewLine();
    }
}

static message *waitForMessage()
{
    message *msg;

    while (1) {
        msg = receiveMessageUsart();
        if (msg != 0)
            break;
    }

    return msg;
}

static void printHeader()
{
    print(PRINT_HEADER);
}

static void printNewLine()
{
    print("\r\n");
}