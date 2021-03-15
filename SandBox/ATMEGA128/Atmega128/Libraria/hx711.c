/*************************************************************************
	HX711
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License     
Hardware: Atmega 128
Date: 08032021_start
Comment:
	Nice
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
#define HX711_ticks 110 // 16Mhz between 100 and 200, make a macro for this value dependent on CPU clock.
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
uint8_t HX711_check_readflag(HX711* self);
uint8_t HX711_read_bit(void);
void HX711_set_amplify(HX711* self, uint8_t amplify);
int32_t HX711_readraw(HX711* self);
float HX711_average(HX711* self, int32_t raw_reading,  uint8_t n);
uint8_t HX711_hl(uint8_t xi, uint8_t xf);
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
	*hx711_DDR |= (ONE<<clkpin);
	*hx711_PORT |= (ONE<<datapin);
	hx711.readflag=ZERO;
	hx711.trigger=ZERO;
	hx711.amplify=ONE;
	hx711.ampcount=ONE;
	hx711.bitcount=HX711_ADC_bits;
	hx711.buffer[0]=ZERO;
	hx711.buffer[1]=ZERO;
	hx711.buffer[2]=ZERO;
	hx711.buffer[3]=ZERO;
	hx711.bufferindex=HX711_VECT_SIZE-ONE;
	hx711.raw_reading=ZERO;
	// offset para mesa usada.
	hx711.cal.offset_32=37122; // to subtract B
	hx711.cal.offset_64=74320; // to subtract A 64
	hx711.cal.offset_128=147500; // to subtract A 128
	//div factor
	hx711.cal.divfactor_32=23; // to divide
	hx711.cal.divfactor_64=46; // to divide
	hx711.cal.divfactor_128=92; // to divide
	//Direccionar apontadores para PROTOTIPOS
	hx711.set_readflag=HX711_set_readflag;
	hx711.check_readflag=HX711_check_readflag;
	hx711.read_bit=HX711_read_bit;
	hx711.set_amplify=HX711_set_amplify;
	hx711.readraw=HX711_readraw;
	hx711.average=HX711_average;
	SREG=tSREG;
	// returns a copy
	return hx711;
}
void HX711_set_readflag(HX711* self)
{
	self->readflag=ONE;
}
void HX711_reset_readflag(HX711* self)
{
	self->readflag=ZERO;
}
uint8_t HX711_check_readflag(HX711* self)
{
	return self->readflag;	
}
uint8_t HX711_read_bit(void)
{	
	uint16_t bool;
	*hx711_PORT|=(ONE<<hx711_clkpin);
	/**0.1us minimum**/
	for(bool=0;bool<HX711_ticks;bool++);
	bool=*hx711_PIN & (ONE<<hx711_datapin);
	*hx711_PORT&=~(ONE<<hx711_clkpin);
	return bool;
}
// Gain selector
// AVDD connected to 5V, channel B gain=32
void HX711_set_amplify(HX711* self, uint8_t amplify)
{
	switch(amplify){
		case 128:
			self->amplify=ONE; //channel A
			self->ampcount=ONE;
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
			self->amplify=ONE;
			self->ampcount=ONE;
			break;
	}
}
/***
Function to be used in the interrupt routine with appropriate cycle period.
***/
int32_t HX711_readraw(HX711* self)
{
	uint8_t aindex, bindex;
	int32_t value;
	aindex = self->bufferindex-ONE;
	bindex = self->bitcount-ONE;
	ptr=(int32_t*)self->buffer;
	/***Detect query for reading***/
	if((!(*hx711_PIN & ONE << hx711_datapin)) && !self->readflag){
		HX711_set_readflag(self);
		PORTC&=~(1<<0); // indicator remove when finished
	}
	/***Interrupt 24 times sequence***/
	if(self->readflag){
		if(self->bitcount){
			if (HX711_read_bit())
				self->buffer[aindex] |= ONE<<(bindex-(aindex*8));
			self->bitcount--;
			if(self->bitcount==16)
				self->bufferindex=2;
			if(self->bitcount==8)
				self->bufferindex=ONE;
		}else{
			if(self->ampcount){
				HX711_read_bit();
				self->ampcount--;
			}else{
				value=*(ptr);
				self->raw_reading=value;
				self->bitcount=HX711_ADC_bits;
				self->bufferindex=HX711_VECT_SIZE-ONE;
				self->ampcount=self->amplify;
				self->buffer[0]=ZERO;
				self->buffer[1]=ZERO;
				self->buffer[2]=ZERO;
				self->buffer[3]=ZERO;
				self->trigger=ONE;
				/***Reset ready for next query***/
				HX711_reset_readflag(self);
			}
		}
	}
	return self->raw_reading;
}
float HX711_average(HX711* self, int32_t raw_reading ,uint8_t n)
{
	if(self->trigger){
		if(self->av_n < n){
			self->sum+=raw_reading;
			self->av_n++;
			}else{
			self->av_n=0;
			self->mean=self->sum/n;
			self->sum=0;
			self->sum+=raw_reading;
			self->av_n++;
		}
		self->trigger=ZERO;
	}
	return self->mean;
}
uint8_t HX711_hl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
/***Interrupt***/
/****comment:
Have to use vector to store 32 bit size word, then do a cast (int32_t*) to retrieve the value.
*************/
/***EOF***/
