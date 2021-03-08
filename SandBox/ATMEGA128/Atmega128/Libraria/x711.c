/*************************************************************************
	X711
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License     
Hardware: Atmega 128
Date: 08032021_start
Comment:

************************************************************************/
#ifndef F_CPU
/***Mandatory to use util/delay.h***/
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <inttypes.h>
#include "x711.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define X711_outMAX 1023
#define X711_outMIN 1023
/***Global File Variable***/
volatile uint8_t *x711_DDR;
volatile uint8_t *x711_PORT;
uint8_t x711_datapin;
uint8_t x711_clkpin;
float tmp;
/***Header***/
void X711_set_kc(X711* self, float kc);
void X711_set_ki(X711* self, float ki);
void X711_set_kd(X711* self, float kp);
void X711_set_SP(X711* self, float setpoint);
float X711_delta(float present_value, float past_value);
float X711_sum(float value_1, float value_2);
float X711_product(float value_1, float value_2);
float X711_integral(X711* self, float PV, float timelapse);
float X711_derivative(X711* self, float PV, float timelapse);
float X711_output(X711* self, float PV, float timelapse);
uint8_t X711_hh(uint8_t xi, uint8_t xf);
uint8_t X711_ll(uint8_t xi, uint8_t xf);
uint8_t X711_lh(uint8_t xi, uint8_t xf);
uint8_t X711_hl(uint8_t xi, uint8_t xf);
/***Procedure & Function***/
X711 X711enable(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	X711 x711;
	//import parametros
	x711_DDR=ddr;
	x711_PORT=port;
	x711_datapin=datapin;
	x711_clkpin=clkpin;
	//inic variables
	*x711_DDR |= (1<<clkpin);
	*x711_PORT |= (1<<datapin);
	x711.kc=1;
	x711.ki=0;
	x711.kd=0;
	x711.Err_past=0;
	x711.dy=0;
	x711.dx=0;
	x711.integral=0;
	x711.derivative=0;
	x711.SetPoint=0;
	x711.PV=0;
	x711.OP=0;
	//Direccionar apontadores para PROTOTIPOS
	x711.set_kc=X711_set_kc;
	x711.set_ki=X711_set_ki;
	x711.set_kd=X711_set_kd;
	x711.set_SP=X711_set_SP;
	x711.output=X711_output;
	SREG=tSREG;
	//
	return x711;
}
void X711_set_kc(X711* self, float kc)
{
	self->kc=kc;
}
void X711_set_ki(X711* self, float ki)
{
	self->ki=ki;
}
void X711_set_kd(X711* self, float kd)
{	
	self->kd=kd;
}
void X711_set_SP(X711* self, float setpoint)
{
	self->SetPoint=setpoint;
}
float X711_delta(float present_value, float past_value)
{
	return (present_value - past_value);
}
float X711_sum(float value_1, float value_2)
{
	return (value_1 + value_2);
}
float X711_product(float value_1, float value_2)
{
	return (value_1 * value_2);
}
float X711_integral(X711* self, float PV, float timelapse)
{
	tmp=X711_product(X711_sum(X711_delta(self->SetPoint, PV), self->Err_past), timelapse);
	tmp/=2;
	return (self->integral += tmp);
}
float X711_derivative(X711* self, float PV, float timelapse)
{
	tmp=X711_delta(X711_delta(self->SetPoint, PV), self->Err_past);
	return (self->derivative = (tmp / timelapse));
}
float X711_output(X711* self, float PV, float timelapse)
{
	float result;
	self->PV=PV;
	self->dy=X711_delta(self->SetPoint, PV);
	self->dx=timelapse;
	result=X711_product(self->kc, self->dy);
	tmp=X711_product(self->ki, X711_integral(self, PV, timelapse));
	result=X711_sum(result, tmp);
	tmp=X711_product(self->kd, X711_derivative(self, PV, timelapse));
	result=X711_sum(result, tmp);
	self->Err_past = self->dy;
	self->OP=result;
	if(result > X711_outMAX)
		self->integral=X711_outMAX - (self->dy * self->dx) - (self->derivative * self->dx * self->dx);
	else if(result < X711_outMIN)
		self->integral=X711_outMIN + (self->dy * self->dx) + (self->derivative * self->dx * self->dx);
	return result;
}
// hh
uint8_t X711_hh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi&xf;
	return i;
}
// ll
uint8_t X711_ll(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi|xf;
	return ~i;
}
// lh
uint8_t X711_lh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi^xf;
	i&=xf;
	return i;
}
// hl
uint8_t X711_hl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
/***Interrupt***/
/****comment:
*************/
/***EOF***/
