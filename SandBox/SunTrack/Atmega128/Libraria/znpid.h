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
	float kc;
	float ki;
	float kd;
	float dy; // difference error y axis points.
	float sy; // sum error points
	float Ep; // Last Error reading
	float dx; // difference time x axis points.
	float derivative; // rate of growth (tangent), or derivative
	float integral;
	float SetP; // desired output
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
	result=kc*Ef;
	tmp=ki*integral;
	result+=tmp
	tmp=kd*derivative;
	result+=tmp; // do step by step, because of mcu, always two variable equation.
	
	The main objective is to prevent overshoot, that is feedback > setpoint, but to smoothly go to zero, preventing oscillation.
	*******/
	/******/
	void (*set_kc)(struct znpid* self, float kc);
	void (*set_ki)(struct znpid* self, float ki);
	void (*set_kd)(struct znpid* self, float kd);
	void (*setpoint)(struct znpid* self, float setpoint);
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
