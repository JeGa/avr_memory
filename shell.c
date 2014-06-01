#include "shell.h"
#include "usart.h"
#include "usart_buffer.h"

// TODO: Remove that
#include <util/delay.h>
#include "jefax_xmega128.h"

static void printString(char *string);
static void printHeader();
static void printNewLine();

void delay(int t)
{
	int i = 0;
	for (i = 0; i < t; ++i) {
		_delay_ms(1);
	}
}

void shellTask()
{
	initLED();
	initUsart();
	
	while (1) {
		message *msg;
		int running = 1;
		
		printNewLine();
		printHeader();
		
		while (running) {
			msg = receiveMessageUsart();
			if (msg != 0) {
				if (*getMessageData(msg) == 'x')
					running = 0;
				else if (*getMessageData(msg) == 'l') {
					setLED(1);
				} else if (*getMessageData(msg) == 'o') {
					setLED(0);
				}
				
				destroyMessage(msg);
			}

			//delay(1000);
		}
	}
	
	stopUsart();
}

void print(char *string)
{
	printHeader();
	printString(string);
}

// TODO: Memory leak
char *read()
{
	message *msg = receiveMessageUsart();
	
	if (msg != 0)
		return getMessageData(msg);
	return 0;
}

static void printString(char *string)
{
	message *msg;
	int stringSize = 0;
	
	// Count string size
	while (string[stringSize])
		++stringSize;
			
	if (stringSize == 0)
		return;
		
	msg = getMessage(stringSize);
	setMessageData(msg, string, stringSize);
	
	sendMessageUsart(msg);
}

static void printHeader()
{
	printString(PRINT_HEADER);	
}

static void printNewLine()
{
	printString("\r\n");
}