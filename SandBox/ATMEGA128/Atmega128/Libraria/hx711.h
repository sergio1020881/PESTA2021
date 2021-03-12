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
//calibration
struct HX711_calibration{
	int32_t offset; // ZERO set point
	uint8_t divfactor_1; // interval 1
	uint8_t divfactor_2; // interval 2
	uint8_t divfactor_3; // interval 3
};
//device
struct hx711{
	uint8_t readflag; // indicate start of bit shifting
	uint8_t amplify; // number of end clock cycles
	uint8_t ampcount; // count down final amplify pulses
	uint8_t bitcount; // count down 24 bit value
	uint8_t buffer[4]; // reading buffer
	uint8_t bufferindex; // buffer index
	int32_t reading; // reading to be published
	struct HX711_calibration cal;
	/******/
	void (*set_readflag)(struct hx711* self);
	uint8_t (*read_bit)(void);
	void (*set_amplify)(struct hx711* self, uint8_t amplify);
	int32_t (*read)(struct hx711* self);
};
typedef struct hx711 HX711;
/***Header***/
HX711 HX711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin);
#endif
/***EOF***/
/***COMMENT***
*************/
