#include "shell.h"
#include "usart.h"
#include "usart_buffer.h"

#include "jefax_xmega128.h"

static void printString(char *string);
static void printHeader();
static void printNewLine();
static int parseMessage(message *msg);

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
			running = parseMessage(msg);
			destroyMessage(msg);
		}
	}
	
	stopUsart();
}

void print(char *string)
{
	printHeader();
	printString(string);
}

// TODO
char read()
{
	char ret;
		
	return ret;
}

// TODO
static int parseMessage(message *msg)
{

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