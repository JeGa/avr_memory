#include "memory.h"
#include "usart_buffer.h"
#include "usart.h"
#include "shell.h"

#include <util/delay.h>

void init32MHzClock()
{
	uint8_t clkCtrl;
	
	// Enable 32Mhz internal clock source
	OSC.CTRL |= OSC_RC32MEN_bm;
	
	// Wait until clock source is stable
	while (!(OSC.STATUS & OSC_RC32MRDY_bm));
	
	// Select main clock source
	clkCtrl = (CLK.CTRL & ~CLK_SCLKSEL_gm) | CLK_SCLKSEL_RC32M_gc;
	
	asm volatile(
		"movw r30,  %0"	      "\n\t"
		"ldi  r16,  %2"	      "\n\t"
		"out   %3, r16"	      "\n\t"
		"st     Z,  %1"       "\n\t"
		:
		: "r" (&CLK.CTRL), "r" (clkCtrl), "M" (CCP_IOREG_gc), "i" (&CCP)
		: "r16", "r30", "r31"
	);
	
	// Disable 2Mhz default clock source
	//OSC.CTRL &= ~OSC_RC2MEN_bm;
}

void doMemoryTest()
{
	volatile char *mem1, *mem2;
	volatile memoryInfo info1, info2, info3;
	
	mem1 = (char *) allocateMemory(9);
	info1 = dumpMemory();
	
	mem2 = (char *) allocateMemory(9);
	info2 = dumpMemory();
	
	freeMemory(mem1);
	freeMemory(mem2);
	info3 = dumpMemory();
	
	mem1 = (char *) allocateMemory(10);
	info1 = dumpMemory();
	freeMemory(mem1);
}

void doBufferTest()
{
	message *msg1, *msg2;
	messageQueue *queue;
	
	char *msg1_data;
	memoryInfo info;
	
	info = dumpMemory();
	
	queue = getMessageQueue();
	
	msg1 = getMessage(10);
	setMessageData(msg1, "Hello\n", 7);
	msg2 = getMessage(20);
	setMessageData(msg2, "World\n", 7);
	
	enqueue(queue, msg1);
	enqueue(queue, msg2);
	info = dumpMemory();
	
	destroyMessageQueue(queue);
	info = dumpMemory();
}

int main(void)
{
	char *data = "Hallo\r\n";

	init32MHzClock();
	
	shellTask();
}