#include "usart.h"
#include "jefax_xmega128.h"
#include <avr/interrupt.h>

static messageQueue *rxQueue;
static messageQueue *txQueue;

/**
 * Because you can send only one byte at once
 * via serial, this pointer is used to point to
 * the current message that should be send.
 */
static message *currentMessage;

int initUsart()
{
	rxQueue = getMessageQueue();
	if (rxQueue == 0)
		return 0;
	
	txQueue = getMessageQueue();
	if (txQueue == 0)
		return 0;
		
	currentMessage = 0;
	
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
	
	enableInterrupts();
	
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
	// TODO: Check size (has to be 1 byte at the moment)
	enqueue(txQueue, msg);
	
	// Enable DRE IR
	USART.CTRLA |= USART_DREINTLVL_LO_gc;
}

message *receiveMessageUsart()
{
	return dequeue(rxQueue);
}

/**
 * Is called from the receive ISR.
 *
 * It copies the received byte into a message
 * and puts the message into the receive queue.
 */
static void receive()
{
	message *msg = getMessage(2);
	if (msg == 0)
		return;
	
	char data = USART.DATA;
	
	setMessageData(msg, &data, 1);
	enqueue(rxQueue, msg);
}

/**
 * Is called from the data register empty ISR.
 *
 * When the data is send, the message is destroyed.
 */
static void send()
{
	// Disable DRE IR if buffer is empty.
	if (isMessageQueueEmpty(txQueue))
		USART.CTRLA = (USART.CTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
	else {
		// Transmit msg from send buffer
		
		if (currentMessage == 0)
			currentMessage = dequeue(txQueue);
			
		// Send one byte from current message
		USART.DATA = popMessageData(currentMessage);
		
		// If message has no data left destroy it
		if (isMessageStackEmpty(currentMessage)) {
			destroyMessage(currentMessage);	
			currentMessage = 0;
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