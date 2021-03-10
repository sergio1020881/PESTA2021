/*************************************************************************
	HX711
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
#include "hx711.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define ONE 1
#define HX711_ticks 110 // 16Mhz between 100 and 200
#define HX711_ADC_bits 24
#define HX711_VECT_SIZE 4
/***Global File Variable***/
volatile uint8_t *hx711_DDR;
volatile uint8_t *hx711_PIN;
volatile uint8_t *hx711_PORT;
uint8_t hx711_datapin;
uint8_t hx711_clkpin;
int32_t* ptr;
/***Header***/
void HX711_set_readflag(HX711* self);
void HX711_reset_readflag(HX711* self);
uint8_t HX711_read_bit(void);
void HX711_set_amplify(HX711* self, uint8_t amplify);
uint32_t HX711_read(HX711* self);
/***Procedure & Function***/
HX711 HX711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	HX711 hx711;
	//import parametros
	hx711_DDR=ddr;
	hx711_PIN=pin;
	hx711_PORT=port;
	hx711_datapin=datapin;
	hx711_clkpin=clkpin;
	//inic variables
	*hx711_DDR |= (1<<clkpin);
	*hx711_PORT |= (1<<datapin);
	hx711.readflag=0;
	hx711.amplify=1;
	hx711.ampcount=1;
	hx711.bitcount=HX711_ADC_bits;
	hx711.buffer[3]=0;
	hx711.bufferindex=3;
	hx711.reading=0;
	//Direccionar apontadores para PROTOTIPOS
	hx711.set_readflag=HX711_set_readflag;
	hx711.read_bit=HX711_read_bit;
	hx711.set_amplify=HX711_set_amplify;
	hx711.read=HX711_read;
	SREG=tSREG;
	//
	return hx711;
}
void HX711_set_readflag(HX711* self)
{
	self->readflag=1;
}
void HX711_reset_readflag(HX711* self)
{
	self->readflag=0;
}
uint8_t HX711_read_bit(void)
{	
	uint16_t bool;
	*hx711_PORT|=(1<<hx711_clkpin);
	/**0.1us minimum**/
	for(bool=0;bool<HX711_ticks;bool++);
	bool=*hx711_PIN & (1<<hx711_datapin);
	*hx711_PORT&=~(1<<hx711_clkpin);
	return bool;
}
// Gain selector
// AVDD connected to 5V, channel B gain=32
void HX711_set_amplify(HX711* self, uint8_t amplify)
{
	switch(amplify){
		case 128:
			self->amplify=1; //channel A
			self->ampcount=1;
			break;
		case 32:
			self->amplify=2; //channel B
			self->ampcount=2; 
			break;
		case 64:
			self->amplify=3; //channel A
			self->ampcount=3;
			break;
		default:
			self->amplify=1;
			self->ampcount=1;
			break;
	}
}
uint32_t HX711_read(HX711* self)
{
	uint8_t aindex, bindex;
	uint32_t value;
	aindex = self->bufferindex-1;
	bindex = self->bitcount-1;
	ptr=(int32_t*)self->buffer;
	/***Detect query for reading***/
	if((!(*hx711_PIN & ONE << hx711_datapin)) && !self->readflag){
		HX711_set_readflag(self);
		PORTC&=~(1<<0);
	}
	/***Interrupt 24 times sequence***/
	if(self->readflag){
		if(self->bitcount){
			if (HX711_read_bit()) self->buffer[aindex] |= ONE<<(bindex-(aindex*8));
			self->bitcount--;
			if(self->bitcount==16)
				self->bufferindex=2;
			if(self->bitcount==8)
				self->bufferindex=1;
		}else{
			if(self->ampcount){
				HX711_read_bit();
				self->ampcount--;
			}else{
				value=*(ptr);
				if(value>8388607)
					value-=16777216;
				self->reading=value;
				self->bitcount=HX711_ADC_bits;
				self->bufferindex=HX711_VECT_SIZE-ONE;
				self->ampcount=self->amplify;
				self->buffer[0]=ZERO;
				self->buffer[1]=ZERO;
				self->buffer[2]=ZERO;
				self->buffer[3]=ZERO;
				/***Reset ready for next query***/
				HX711_reset_readflag(self);
			}
		}
	}
	return self->reading;
}
/***Interrupt***/
/****comment:
*************/
/***EOF***/
