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
	void (*set_kc)(struct znpid* self, float kc);
	void (*set_ki)(struct znpid* self, float ki);
	void (*set_kd)(struct znpid* self, float kd);
	void (*set_SP)(struct znpid* self, float setpoint);
	float (*output)(struct znpid* self, float PV, float timelapse);
};
typedef struct znpid ZNPID;
/***Header***/
ZNPID ZNPIDenable(void);
#endif
/***EOF***/
/***COMMENT***
If Error=0 (Ef) implies that feedback is equal to setpoint.
lets see how this goes to work out ???
Taking down notes for what is desired to build library:
Ef=SetP-PV;
dy=Ef-Ep;
sy=Ef+Ep;
dx=Xf-Xp; // timelapse
tmp=sy*dx;
tmp/=2;
integral+=tmp; // put watchdog on this value, if above setpoint*dx*1.5 do not let it integrate anymore and wait. Trapezio.
derivative=dy/dx; //put watchdog on this value, if overshoot above specified rate (power rate (open loop)) put in standby until drops bellow.
result=kc*Ef;
tmp=ki*integral;
result+=tmp
tmp=kd*derivative;
result+=tmp; // do step by step, because of mcu, always two variable equation.
Ep=Ef;
Xp=Xf;
The main objective is to prevent overshoot, that is feedback > setpoint, but to smoothly go to zero, preventing oscillation.
*************/
