/************************************************************************
	ATMEGA128ANALOG
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
Hardware: ATmega128
Date: 25102020
Comment:
	Stable
************************************************************************/
/***Preamble Inic***/
#ifndef _ATMEGA128ANALOG_H_
	#define _ATMEGA128ANALOG_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
// ADC_NUMBER_SAMPLE^2 gives number of samples, note values can only range from 0 to 4.
#define ADC_NUMBER_SAMPLE 2
/***Global Variable***/
struct nlg{
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	int (*read)(int selection);
};
typedef struct nlg ANALOG;
/***Header***/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
#endif
/***EOF***/
