#include "jefax_xmega128.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void enableInterrupts()
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();
}

void initLED()
{
	PORTE.DIR = 0xFF;
	PORTE.OUT = 0xFF;
}

void setLED(uint8_t status)
{
	PORTE.OUT = status;
}
