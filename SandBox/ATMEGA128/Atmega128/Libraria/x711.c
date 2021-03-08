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
#define ONE 1
#define X711_outMAX 1023
#define X711_outMIN 1023
/***Global File Variable***/
volatile uint8_t *x711_DDR;
volatile uint8_t *x711_PIN;
volatile uint8_t *x711_PORT;
uint8_t x711_datapin;
uint8_t x711_clkpin;
float tmp;
/***Header***/
void X711_set_readflag(X711* self);
void X711_reset_readflag(X711* self);
uint8_t X711_read_bit(void);
void X711_set_amplify(X711* self, uint8_t amplify);
float X711_delta(float present_value, float past_value);
float X711_sum(float value_1, float value_2);
float X711_product(float value_1, float value_2);
float X711_integral(X711* self, float PV, float timelapse);
float X711_derivative(X711* self, float PV, float timelapse);
float X711_shift_bit(X711* self, float PV, float timelapse);
uint8_t X711_hh(uint8_t xi, uint8_t xf);
uint8_t X711_ll(uint8_t xi, uint8_t xf);
uint8_t X711_lh(uint8_t xi, uint8_t xf);
uint8_t X711_hl(uint8_t xi, uint8_t xf);
/***Procedure & Function***/
X711 X711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	X711 x711;
	//import parametros
	x711_DDR=ddr;
	x711_PIN=pin;
	x711_PORT=port;
	x711_datapin=datapin;
	x711_clkpin=clkpin;
	//inic variables
	*x711_DDR |= (1<<clkpin);
	*x711_PORT |= (1<<datapin);
	x711.readflag=0;
	x711.n_clk=24+1;
	x711.bitcount=0;
	x711.Err_past=0;
	x711.dy=0;
	x711.dx=0;
	x711.integral=0;
	x711.derivative=0;
	x711.PV=0;
	x711.OP=0;
	//Direccionar apontadores para PROTOTIPOS
	x711.set_readflag=X711_set_readflag;
	x711.reset_readflag=X711_reset_readflag;
	x711.read_bit=X711_read_bit;
	x711.set_amplify=X711_set_amplify;
	x711.shift_bit=X711_shift_bit;
	SREG=tSREG;
	//
	return x711;
}
void X711_set_readflag(X711* self)
{
	self->readflag=1;
}
void X711_reset_readflag(X711* self)
{
	self->readflag=0;
}
uint8_t X711_read_bit(void)
{	
	uint8_t bool;
	*x711_PORT|=(1<<x711_clkpin);
	/**maybe here needs rise time interval lets see**/
	bool=*x711_PIN & (1<<x711_datapin);
	*x711_PORT&=~(1<<x711_clkpin);
	return bool;
}
void X711_set_amplify(X711* self, uint8_t amplify)
{
	switch(amplify){
		case 128:
			self->bitcount=25;
			break;
		default:
			self->bitcount=25;
			break;
	}
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
	tmp=0;
	return tmp;
}
float X711_derivative(X711* self, float PV, float timelapse)
{
	tmp=0;
	return tmp;
}
float X711_shift_bit(X711* self, float PV, float timelapse)
{
	float result;
	if(self->readflag){
		if (X711_read_bit()) self->OP|=ONE<<self->bitcount;
		self->bitcount++;
	}
	/*
	for (i=0;i<24;i++){
		ADSK=1;
		Count=Count<<1;
		ADSK=0;
		if(ADDO) Count++;
	}
	ADSK=1;
	Count=Count^0x800000;
	ADSK=0;
	return(Count);
	*/
	
	
	result=0;
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
