/************************************************************************
	HX711
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: Atmega 128
Date: 08032021_start
Comment:

************************************************************************/
#ifndef _HX711_H_
	#define _HX711_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/

/***Global Variable***/

struct hx711{
	uint8_t readflag; // indicate start of bit shifting
	uint8_t amplify; // number of clock cycles
	uint8_t ampcount; // count down final amplify pulses
	uint8_t bitcount; // count down 24 bit value
	uint8_t buffer[4]; // reading buffer
	uint8_t bufferindex; // buffer index
	int32_t reading; // reading to be published
	/******/
	void (*set_readflag)(struct hx711* self);
	void (*reset_readflag)(struct hx711* self);
	uint8_t (*read_bit)(void);
	void (*set_amplify)(struct hx711* self, uint8_t amplify);
	uint32_t (*shift_bits)(struct hx711* self);
};
typedef struct hx711 HX711;
/***Header***/
HX711 HX711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin);
#endif
/***EOF***/
/***COMMENT***
*************/
