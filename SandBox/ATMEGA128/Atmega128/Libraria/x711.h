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
	float kc; // constant p
	float ki; // constant i
	float kd; // constant d
	float Err_past; // Last Error reading
	float dy; // difference error y axis points.
	float dx; // difference time x axis points.
	float integral; // progression
	float derivative; // rate of growth (tangent), or derivative
	float SetPoint; // desired output
	float PV; // output feedback
	float OP; // output signal
	/******/
	void (*set_kc)(struct x711* self, float kc);
	void (*set_ki)(struct x711* self, float ki);
	void (*set_kd)(struct x711* self, float kd);
	void (*set_SP)(struct x711* self, float setpoint);
	float (*output)(struct x711* self, float PV, float timelapse);
};
typedef struct x711 X711;
/***Header***/
X711 X711enable(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin);
#endif
/***EOF***/
/***COMMENT***
*************/
