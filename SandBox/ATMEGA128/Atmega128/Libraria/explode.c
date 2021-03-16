/*************************************************************************
	EXPLODE
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com> 
License: GNU General Public License
Hardware: all
Date: 16032021
Comment:
    Pin Analises
*************************************************************************/
/***Library***/
#include <avr/io.h>
#include <inttypes.h>
#include"explode.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/***Global File Variable***/
/***Header***/
/************/
void EXPLODEboot(EXPLODE* self, uint8_t x);
uint8_t EXPLODEhh(EXPLODE* self);
uint8_t EXPLODEll(EXPLODE* self);
uint8_t EXPLODElh(EXPLODE* self);
uint8_t EXPLODEhl(EXPLODE* self);
uint8_t EXPLODEdiff(EXPLODE* self);
uint8_t EXPLODEdata(EXPLODE* self);
/***Procedure & Function***/
EXPLODE EXPLODEenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	struct expld explode;
	// inic VAR
	explode.XI=0;
	explode.XF=0;
	// function pointers
	explode.boot=EXPLODEboot;
	explode.hh=EXPLODEhh;
	explode.ll=EXPLODEll;
	explode.lh=EXPLODElh;
	explode.hl=EXPLODEhl;
	explode.diff=EXPLODEdiff;
	explode.data=EXPLODEdata;
	SREG=tSREG;
	/******/
	return explode;
}
// boot
void EXPLODEboot(EXPLODE* self, uint8_t x)
{
	self->XI = self->XF;
	self->XF = x;
}
// hh
uint8_t EXPLODEhh(EXPLODE* self)
{
	uint8_t i;
	i=self->XI&self->XF;
	return i;
}
// ll
uint8_t EXPLODEll(EXPLODE* self)
{
	uint8_t i;
	i=self->XI|self->XF;
	return ~i;
}
// lh
uint8_t EXPLODElh(EXPLODE* self)
{
	uint8_t i;
	i=self->XI^self->XF;
	i&=self->XF;
	return i;
}
// hl
uint8_t EXPLODEhl(EXPLODE* self)
{
	uint8_t i;
	i=self->XF^self->XI;
	i&=self->XI;
	return i;
}
// diff
uint8_t EXPLODEdiff(EXPLODE* self)
{
	return self->XF^self->XI;
}
uint8_t EXPLODEdata(EXPLODE* self)
{
	return self->XF;	
}
/***Interrupt***/
/***EOF***/
