/************************************************************************
	ZNPID
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: Atmega 128
Date: 17022021_start
Comment:

************************************************************************/
#ifndef _ZNPID_H_
	#define _ZNPID_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
/***Global Variable***/
struct znpid{
	float kp;
	float ki;
	float kd;
	float dy; // the error.
	float py; // inicial value as zero, previous error value.
	float dx; // inicial value as zero.
	float delta; // rate of growth (tangent.
	float setpoint;
	float feedback; //inicial value as zero.
	/******
	Taking down notes for what is desired to build library:
	dx=tf-ti;
	dy=setpoint-feedback;
	integral+=((py+dy)*dx)/2; // put watchdog on this value, if above setpoint*dx*1.5 do not let it integrate anymore and wait.
	py=dy;
	delta=dy/dx; //put watchdog on this value, if overshoot specified rate put in standby until drops bellow.
	
	The main objective is to prevent overshoot, that is feedback > setpoint, but to smoothly go to zero, preventing oscillation.
	*******/
	/******/
	void (*bit)(uint8_t bool);
	void (*byte)(uint8_t byte);
	void (*out)(void);
};
typedef struct znpid ZNPID;
/***Header***/
ZNPID ZNPIDenable(ZNPID* self, uint8_t datapin, uint8_t clkpin, uint8_t outpin);
#endif
/***EOF***/
/***COMMENT***
If Error=0 (dy) implies that feedback is equal to setpoint.
lets see how this goes to work out ???


*************/
