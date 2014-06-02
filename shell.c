#include "shell.h"
#include "usart.h"
#include "usart_buffer.h"

#include <string.h>
#include "jefax_xmega128.h"

static void printString(char *string);
static void printHeader();
static void printNewLine();
static int parseMessage(message *msg);

void shellTask()
{
	initLED();
	initUsart();
	
	printNewLine();
	
	while (1) {
		message *msg;
		int running = 1;
		
		printHeader();
		
		while (running) {
			msg = receiveMessageUsart();
			running = parseMessage(msg);
			destroyMessage(msg);
		}
	}
	
	stopUsart();
}

// TODO
static int parseMessage(message *msg)
{
	if (msg == 0)
		return 1;
		
	char *data = getMessageData(msg);
	
	if (strcmp("ledOn", data) == 0) {
		setLED(0XFE);
	} else if (strcmp("ledOff", data) == 0) {
		setLED(0xFF);
	}
	
	return 0;
}

static void printHeader()
{
	print(PRINT_HEADER);	
}

static void printNewLine()
{
	print("\r\n");
}