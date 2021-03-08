/************************************************************************
	X711
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: Atmega 128
Date: 08032021_start
Comment:

************************************************************************/
#ifndef _X711_H_
	#define _X711_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/

/***Global Variable***/


struct x711{
	uint8_t readflag; // indicate start of bit shifting
	uint8_t n_clk; // number of clock cycles
	uint8_t bitcount; // index shifted bit
	float Err_past; // Last Error reading
	float dy; // difference error y axis points.
	float dx; // difference time x axis points.
	float integral; // progression
	float derivative; // rate of growth (tangent), or derivative
	float PV; // output feedback
	uint32_t OP; // output signal
	/******/
	void (*set_readflag)(struct x711* self);
	void (*reset_readflag)(struct x711* self);
	uint8_t (*read_bit)(void);
	void (*set_amplify)(struct x711* self, uint8_t amplify);
	float (*shift_bit)(struct x711* self, float PV, float timelapse);
};
typedef struct x711 X711;
/***Header***/
X711 X711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin);
#endif
/***EOF***/
/***COMMENT***
*************/
