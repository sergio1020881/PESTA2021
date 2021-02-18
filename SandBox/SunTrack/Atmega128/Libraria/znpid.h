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
float tmp;
struct znpid{
	float kp;
	float ki;
	float kd;
	float dy; // difference error y axis points.
	float sy; // sum error points
	float Ep; // Last Error reading
	float dx; // difference time x axis points.
	float derivative; // rate of growth (tangent), or derivative
	float integral;
	float SP; // desired output
	float PV; // output feedback
	float OP; // output signal
	/******
	Taking down notes for what is desired to build library:
	Ef=SP-PV;
	dy=Ef-Ep;
	sy=Ef+Ep;
	dx=Xf-Xp;
	tmp=sy*dx;
	tmp/=2;
	integral+=tmp; // put watchdog on this value, if above setpoint*dx*1.5 do not let it integrate anymore and wait. Trapezio.
	derivative=dy/dx; //put watchdog on this value, if overshoot specified rate put in standby until drops bellow.
	Ep=Ef;
	result=kp*Ef;
	tmp=ki*integral;
	result+=tmp
	tmp=kd*derivative;
	result+=tmp; // do step by step, because of mcu, always two variable equation.
	
	The main objective is to prevent overshoot, that is feedback > setpoint, but to smoothly go to zero, preventing oscillation.
	*******/
	/******/
	void (*set_kp)(ZNPID* self, float kp);
	void (*set_ki)(ZNPID* self, float ki);
	void (*set_kd)(ZNPID* self, float kd);
	void (*setpoint)(ZNPID* self, float setpoint);
};
typedef struct znpid ZNPID;
/***Header***/
ZNPID ZNPIDenable(void);
#endif
/***EOF***/
/***COMMENT***
If Error=0 (Ef) implies that feedback is equal to setpoint.
lets see how this goes to work out ???


*************/
