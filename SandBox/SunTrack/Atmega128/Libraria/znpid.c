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
/***Global File Variable***/
/***Header***/
void ZNPID_set_kp(ZNPID* self, float kp);
void ZNPID_set_ki(ZNPID* self, float ki);
void ZNPID_set_kd(ZNPID* self, float kp);
void ZNPID_setpoint(ZNPID* self, float setpoint);
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
	znpid.Ep=0;
	znpid.kp=1;
	znpid.ki=0;
	znpid.kd=0;
	znpid.integral=0;
	znpid.derivative=0;
	znpid.OP=0;
	//inic variables   
	//Direccionar apontadores para PROTOTIPOS
	znpid.set_kp=ZNPID_set_kp;
	znpid.set_ki=ZNPID_set_ki;
	znpid.set_kd=ZNPID_set_kd;
	znpid.setpoint=ZNPID_setpoint;
	SREG=tSREG;
	//
	return znpid;
}
void ZNPID_set_kp(ZNPID* self, float kp)
{
	self->kp=kp;
}
void ZNPID_set_ki(ZNPID* self, float ki)
{
	self->ki=ki;
}
void ZNPID_set_kd(ZNPID* self, float kd)
{	
	self->kd=kd;
}
void ZNPID_setpoint(ZNPID* self, float setpoint)
{
	self->SP=setpoint;
}
/***Interrupt***/
/***EOF***/
