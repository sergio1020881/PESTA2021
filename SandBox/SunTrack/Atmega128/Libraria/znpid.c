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
uint8_t ZNPID_datapin;
uint8_t ZNPID_clkpin; 
uint8_t ZNPID_outpin;
/***Header***/
void ZNPID_shift_bit(uint8_t bool);
void ZNPID_shift_byte(uint8_t byte);
void ZNPID_shift_out(void);
/***Procedure & Function***/
ZNPID ZNPIDenable(ZNPID* self, uint8_t datapin, uint8_t clkpin, uint8_t outpin)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	ZNPID znpid;
	//import parametros
	ZNPID_datapin=datapin;
	ZNPID_clkpin=clkpin;
	ZNPID_outpin=outpin;
	//inic variables
    
	//Direccionar apontadores para PROTOTIPOS
	znpid.bit=ZNPID_shift_bit;
	znpid.byte=ZNPID_shift_byte;
	znpid.out=ZNPID_shift_out;
	SREG=tSREG;
	//
	return znpid;
}
void ZNPID_shift_bit(uint8_t bool)
{
		
}
void ZNPID_shift_byte(uint8_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
		ZNPID_shift_bit(byte & (1<<i));
	ZNPID_shift_out();
}
void ZNPID_shift_out(void)
{
	
	
}
/***Interrupt***/
/***EOF***/
