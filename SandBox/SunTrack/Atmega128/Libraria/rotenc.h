/************************************************************************
	ROTENC
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
Hardware: Rotary encoder Potenciometer
Date: 25102020
Comment:
	Stable
************************************************************************/
/***Preamble Inic***/
#ifndef _ROTENC_H_
	#define _ROTENC_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
/***Global variable***/
struct rotenc{
	/***VARIABLIES***/
	uint8_t PinChnA;
	uint8_t PinChnB;
	uint8_t pchn;
	uint8_t chn;
	uint16_t num;
	/***PROTOTYPES VTABLE***/
	struct rotenc (*rte)(struct rotenc *self, uint8_t data);
};
typedef struct rotenc ROTENC;
/***Header***/
ROTENC ROTENCenable(uint8_t ChnApin, uint8_t ChnBpin);
#endif
/***EOF***/
