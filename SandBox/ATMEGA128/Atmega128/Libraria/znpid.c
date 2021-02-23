/*************************************************************************
	ZNPID
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License     
Hardware: Atmega 128
Date: 17022021_start
Comment:

************************************************************************/
#ifndef F_CPU
/***Mandatory to use util/delay.h***/
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <inttypes.h>
#include "znpid.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define ZNPID_outMAX 1023
#define ZNPID_outMIN -1023
/***Global File Variable***/
/***************************
struct ZNPID_DATA{
	float proportional;
	float integral;
	float derivative;
}reading;
***************************/
float tmp;
/***Header***/
void ZNPID_set_kc(ZNPID* self, float kc);
void ZNPID_set_ki(ZNPID* self, float ki);
void ZNPID_set_kd(ZNPID* self, float kp);
void ZNPID_set_SP(ZNPID* self, float setpoint);
float delta(float present_value, float past_value);
float sum(float value_1, float value_2);
float product(float value_1, float value_2);
float integral(ZNPID* self, float PV, float timelapse);
float derivative(ZNPID* self, float PV, float timelapse);
float ZNPID_output(ZNPID* self, float PV, float timelapse);
/***Procedure & Function***/
ZNPID ZNPIDenable(void)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	ZNPID znpid;
	//import parametros
	//inic variables
	znpid.kc=1;
	znpid.ki=0;
	znpid.kd=0;
	znpid.Err_past=0;
	znpid.dy=0;
	znpid.dx=0;
	znpid.integral=0;
	znpid.derivative=0;
	znpid.SetPoint=0;
	znpid.PV=0;
	znpid.OP=0;
	//Direccionar apontadores para PROTOTIPOS
	znpid.set_kc=ZNPID_set_kc;
	znpid.set_ki=ZNPID_set_ki;
	znpid.set_kd=ZNPID_set_kd;
	znpid.set_SP=ZNPID_set_SP;
	znpid.output=ZNPID_output;
	SREG=tSREG;
	//
	return znpid;
}
void ZNPID_set_kc(ZNPID* self, float kc)
{
	self->kc=kc;
}
void ZNPID_set_ki(ZNPID* self, float ki)
{
	self->ki=ki;
}
void ZNPID_set_kd(ZNPID* self, float kd)
{	
	self->kd=kd;
}
void ZNPID_set_SP(ZNPID* self, float setpoint)
{
	self->SetPoint=setpoint;
}
float delta(float present_value, float past_value)
{
	return (present_value - past_value);
}
float sum(float value_1, float value_2)
{
	return (value_1 + value_2);
}
float product(float value_1, float value_2)
{
	return (value_1 * value_2);
}
float integral(ZNPID* self, float PV, float timelapse)
{
	tmp=product(sum(delta(self->SetPoint, PV), self->Err_past), timelapse);
	tmp/=2;
	return (self->integral += tmp);
}
float derivative(ZNPID* self, float PV, float timelapse)
{
	tmp=delta(delta(self->SetPoint, PV), self->Err_past);
	return (self->derivative = (tmp / timelapse));
}
float ZNPID_output(ZNPID* self, float PV, float timelapse)
{
	float result;
	self->PV=PV;
	self->dy=delta(self->SetPoint, PV);
	self->dx=timelapse;
	result=product(self->kc, self->dy);
	tmp=product(self->ki, integral(self, PV, timelapse));
	result=sum(result, tmp);
	tmp=product(self->kd, derivative(self, PV, timelapse));
	result=sum(result, tmp);
	self->Err_past = self->dy;
	self->OP=result;
	if(result > ZNPID_outMAX)
		self->integral=ZNPID_outMAX - (self->dy * self->dx) - (self->derivative * self->dx * self->dx);
	else if(result < ZNPID_outMIN)
		self->integral=ZNPID_outMIN + (self->dy * self->dx) + (self->derivative * self->dx * self->dx);
	return result;
}
/***Interrupt***/
/****comment:
nicely done !
The output then has to be filtered in order to comply with the parameters of the controlled system, 
in other words the linear curve may have to be shifted up to become only positive values and scaled
to have different minimum and maximum values that the controlled system accepts.
*************/
/***EOF***/
