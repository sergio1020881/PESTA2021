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
#ifndef STATUS_REGISTER
	#define STATUS_REGISTER SREG
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define OFF 0
#define ONE 1
#define ON 0xFF
#define HX711_ticks 110 // 16Mhz between 100 and 200, make a macro for this value dependent on CPU clock. 55 -> 50% duty cycle
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
void HX711_query(HX711* self);
int32_t HX711_read_raw(HX711* self);
float HX711_raw_average(HX711* self, uint8_t n);
struct HX711_calibration* HX711_get_cal(HX711* self);
/***Procedure & Function***/
HX711 HX711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG = STATUS_REGISTER;
	STATUS_REGISTER &= ~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	HX711 hx711;
	//import parametros
	hx711_DDR = ddr;
	hx711_PIN = pin;
	hx711_PORT = port;
	hx711_datapin = datapin;
	hx711_clkpin = clkpin;
	//inic variables
	*hx711_DDR |= (ONE<<clkpin);
	*hx711_PORT |= (ONE<<datapin);
	hx711.readflag = ZERO;
	hx711.trigger = ZERO;
	hx711.amplify = ONE;
	hx711.ampcount = ONE;
	hx711.bitcount = HX711_ADC_bits;
	hx711.buffer[0] = ZERO;
	hx711.buffer[1] = ZERO;
	hx711.buffer[2] = ZERO;
	hx711.buffer[3] = ZERO;
	hx711.bufferindex = HX711_VECT_SIZE-ONE;
	hx711.raw_reading = ZERO;
	hx711.sum = ZERO;
	hx711.av_n = ZERO;
	hx711.raw_mean = ZERO;
	// offset para mesa usada.
	hx711.cal_data.offset_32 = 36800; // to subtract B
	hx711.cal_data.offset_64 = 72700; // to subtract A 64
	hx711.cal_data.offset_128 = 147200; // to subtract A 128
	//div factor
	hx711.cal_data.divfactor_32=23; // to divide
	hx711.cal_data.divfactor_64=46; // to divide
	hx711.cal_data.divfactor_128=92; // to divide
	hx711.cal_data.status=ZERO;
	//Direccionar apontadores para PROTOTIPOS
	//hx711.set_readflag=HX711_set_readflag;
	//hx711.check_readflag=HX711_check_readflag;
	hx711.read_bit=HX711_read_bit;
	hx711.set_amplify=HX711_set_amplify;
	hx711.query=HX711_query;
	hx711.read_raw=HX711_read_raw;
	hx711.raw_average=HX711_raw_average;
	hx711.get_cal=HX711_get_cal;
	STATUS_REGISTER = tSREG;
	// returns a copy
	return hx711;
}
void HX711_set_readflag(HX711* self)
{
	self->readflag=ON;
}
void HX711_reset_readflag(HX711* self)
{
	self->readflag=OFF;
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
			self->amplify = ONE; //channel A
			self->ampcount = ONE;
			break;
		case 32:
			self->amplify = 2; //channel B
			self->ampcount = 2; 
			break;
		case 64:
			self->amplify = 3; //channel A
			self->ampcount = 3;
			break;
		default:
			self->amplify = ONE;
			self->ampcount = ONE;
			break;
	}
}
void HX711_query(HX711* self)
{
	if((!(*hx711_PIN & ONE << hx711_datapin)) && !self->readflag){
		HX711_set_readflag(self);
	}
}
/***
Function to be used in the interrupt routine with appropriate cycle period.
***/
int32_t HX711_read_raw(HX711* self)
{
	uint8_t aindex, bindex;
	int32_t value;
	aindex = self->bufferindex-ONE;
	bindex = self->bitcount-ONE;
	ptr=(int32_t*)self->buffer;
	/***Detect query for reading***/
	//if((!(*hx711_PIN & ONE << hx711_datapin)) && !self->readflag){
		//HX711_set_readflag(self);
	//}
	//HX711_query(self);
	/***Interrupt 24 times sequence***/
	if(self->readflag){
		if(self->bitcount){
			if (HX711_read_bit())
				self->buffer[aindex] |= ONE<<(bindex-(aindex*8));
			self->bitcount--;
			if(self->bitcount == 16)
				self->bufferindex=2;
			if(self->bitcount == 8)
				self->bufferindex=ONE;
		}else{
			if(self->ampcount){
				HX711_read_bit();
				self->ampcount--;
			}else{
				value = *(ptr);
				self->raw_reading = value;
				self->bitcount = HX711_ADC_bits;
				self->bufferindex = HX711_VECT_SIZE-ONE;
				self->ampcount = self->amplify;
				self->buffer[0] = ZERO;
				self->buffer[1] = ZERO;
				self->buffer[2] = ZERO;
				self->buffer[3] = ZERO;
				self->trigger = ONE;
				/***Reset ready for next query***/
				HX711_reset_readflag(self);
			}
		}
	}
	return self->raw_reading;
}
float HX711_raw_average(HX711* self, uint8_t n)
{
	if(self->trigger){
		if(self->av_n < n){
			self->sum += self->raw_reading;
			self->av_n++;
			}else{
			self->av_n = ZERO;
			self->raw_mean = self->sum / n;
			self->sum = ZERO;
			self->sum += self->raw_reading;
			self->av_n++;
		}
		self->trigger = ZERO;
	}
	return self->raw_mean;
}
struct HX711_calibration* HX711_get_cal(HX711* self)
{
	return &(self->cal_data);
}
/***Interrupt***/
/***comment***
Have to use vector to store 32 bit size word, then do a cast (int32_t*) to retrieve the value.
*************/
/***EOF***/
