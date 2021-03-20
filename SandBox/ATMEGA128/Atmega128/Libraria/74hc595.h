/************************************************************************
	74HC595
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: 74HC595
Date: 25102020
Comment:
	tested Atemga128 16Mhz and Atmega328 8Mhz
************************************************************************/
#ifndef _74HC595_H_
	#define _74HC595_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
/***Global Variable***/
struct hc595{
	/******/
	void (*bit)(uint8_t _bool);
	void (*byte)(uint8_t byte);
	void (*out)(void);
};
typedef struct hc595 HC595;
/***Header***/
HC595 HC595enable(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin, uint8_t outpin);
#endif
/***EOF***/
