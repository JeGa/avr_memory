/**
 * @file usart.h
 *
 * Low level IRs are used for USART communication.
 *
 * The xmega128 xplained evaluation board supports usart to usb on USARTC0
 * with a baudrate of 115200.
 */

#pragma once

#include <avr/io.h>
#include <stdint.h>
#include "usart_buffer.h"

#define USART USARTC0
#define USART_PORT PORTC
#define BAUDRATE 115200

/**
 * BSEL = (F_CPU / (16 * Baudrate)) - 1
 */
#define BSEL ((uint8_t) (F_CPU / (16 * BAUDRATE)) - 1)

int initUsart();
void stopUsart();

void sendMessageUsart(message *msg);
message *receiveMessageUsart();