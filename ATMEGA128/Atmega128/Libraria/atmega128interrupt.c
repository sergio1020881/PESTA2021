/*************************************************************************
	ATMEGA128INTERRUPT
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: ATmega128
Date: 25102020
Comment:
   Stable
*************************************************************************/
/***Preamble Inic***/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include "atmega128interrupt.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
    #define GLOBAL_INTERRUPT_ENABLE 7
#endif
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)	
	/***/
	#define ATMEGA_INTERRUPT
	#define External_Interrupt_Control_Register_A EICRA
	#define External_Interrupt_Control_Register_B EICRB
	#define External_Interrupt_Mask_Register EIMSK
	#define External_Interrupt_Flag_Register EIFR
	#define MCU_Control_Status_Register MCUCSR
	#define MCU_Control_Status_Register_Mask 0X1F
#else
 	#error "Not Atmega 128"
#endif
/***Gloabal File Variables***/
/***Header***/
void INTERRUPT_set(uint8_t channel, uint8_t sense);
void INTERRUPT_off(uint8_t channel);
void INTERRUPT_on(uint8_t channel);
uint8_t INTERRUPT_reset_status(void);
/***Procedure & Function***/
INTERRUPT INTERRUPTenable(void)
/***
Setup blank
***/
{
	INTERRUPT interrupt;
	External_Interrupt_Mask_Register = 0X00;
	/******/
	interrupt.set=INTERRUPT_set;
	interrupt.off=INTERRUPT_off;
	interrupt.on=INTERRUPT_on;
	interrupt.reset_status=INTERRUPT_reset_status;
	return interrupt;
}
uint8_t INTERRUPT_reset_status(void)
{
	uint8_t reset, ret=0;
	reset=(MCU_Control_Status_Register & MCU_Control_Status_Register_Mask);
	switch(reset){
		case 1: // Power-On Reset Flag
			ret=0;
			MCU_Control_Status_Register &= ~(1<<PORF);
			break;
		case 2: // External Reset Flag
			MCU_Control_Status_Register &= ~(1<<EXTRF);
			ret=1;
			break;
		case 4: // Brown-out Reset Flag
			MCU_Control_Status_Register &= ~(1<<BORF);
			ret=2;
			break;
		case 8: // Watchdog Reset Flag
			MCU_Control_Status_Register &= ~(1<<WDRF);
			ret=3;
			break;
		case 16: // JTAG Reset Flag
			MCU_Control_Status_Register &= ~(1<<JTRF);
			ret=4;
			break;
		default: // clear all status
			MCU_Control_Status_Register &= ~(MCU_Control_Status_Register_Mask);
			break;
	}
	return ret;
}
void INTERRUPT_set(uint8_t channel, uint8_t sense)
{
	switch( channel ){
		case 0: 
			External_Interrupt_Mask_Register &= ~(1<<INT0);
			External_Interrupt_Control_Register_A &= ~((1<<ISC01) | (1<<ISC00));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= (1<<ISC01);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= ((1<<ISC01) | (1<<ISC00));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT0);
			break;
		case 1:
			External_Interrupt_Mask_Register &= ~(1<<INT1);
			External_Interrupt_Control_Register_A &= ~((1<<ISC11) | (1<<ISC10));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= (1<<ISC11);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= ((1<<ISC11) | (1<<ISC10));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT1);
			break;
		case 2:
			External_Interrupt_Mask_Register &= ~(1<<INT2);
			External_Interrupt_Control_Register_A &= ~((1<<ISC21) | (1<<ISC20));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= (1<<ISC21);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= ((1<<ISC21) | (1<<ISC20));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT2);
			break;
		case 3:
			External_Interrupt_Mask_Register &= ~(1<<INT3);
			External_Interrupt_Control_Register_A &= ~((1<<ISC31) | (1<<ISC30));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= (1<<ISC31);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A |= ((1<<ISC31) | (1<<ISC30));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT3);
			break;
		case 4:
			External_Interrupt_Mask_Register &= ~(1<<INT4);
			External_Interrupt_Control_Register_B &= ~((1<<ISC41) | (1<<ISC40));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B |= (1<<ISC40);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= (1<<ISC41);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= ((1<<ISC41) | (1<<ISC40));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT4);
			break;
		case 5:
			External_Interrupt_Mask_Register &= ~(1<<INT5);
			External_Interrupt_Control_Register_B &= ~((1<<ISC51) | (1<<ISC50));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B |= (1<<ISC50);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= (1<<ISC51);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= ((1<<ISC51) | (1<<ISC50));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT5);
			break;
		case 6:
			External_Interrupt_Mask_Register &= ~(1<<INT6);
			External_Interrupt_Control_Register_B &= ~((1<<ISC61) | (1<<ISC60));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B |= (1<<ISC60);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= (1<<ISC61);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= ((1<<ISC61) | (1<<ISC60));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT6);
			break;
		case 7:
			External_Interrupt_Mask_Register &= ~(1<<INT7);
			External_Interrupt_Control_Register_B &= ~((1<<ISC71) | (1<<ISC70));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B |= (1<<ISC70);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= (1<<ISC71);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B |= ((1<<ISC71) | (1<<ISC70));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register |= (1<<INT7);
			break;
		default:
			External_Interrupt_Mask_Register = 0X00;
			break;
	}
}
void INTERRUPT_off(uint8_t channel)
{
	switch( channel ){
		case 0: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT0);
			break;
		case 1: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT1);
			break;
		case 2: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT2);
			break;
		case 3: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT3);
			break;
		case 4: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT4);
			break;
		case 5: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT5);
			break;
		case 6: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT6);
			break;
		case 7: // disable
			External_Interrupt_Mask_Register &= ~(1<<INT7);
			break;
		default: // all disable
			External_Interrupt_Mask_Register = 0X00;
			break;
	}
}
void INTERRUPT_on(uint8_t channel)
{
	switch( channel ){
		case 0:
			External_Interrupt_Mask_Register |= (1<<INT0);
			break;
		case 1:
			External_Interrupt_Mask_Register |= (1<<INT1);
			break;
		case 2:
			External_Interrupt_Mask_Register |= (1<<INT2);
			break;
		case 3:
			External_Interrupt_Mask_Register |= (1<<INT3);
			break;
		case 4:
			External_Interrupt_Mask_Register |= (1<<INT4);
			break;
		case 5:
			External_Interrupt_Mask_Register |= (1<<INT5);
			break;
		case 6:
			External_Interrupt_Mask_Register |= (1<<INT6);
			break;
		case 7:
			External_Interrupt_Mask_Register |= (1<<INT7);
			break;
		default: // all disable
			External_Interrupt_Mask_Register = 0X00;
			break;
	}
}
/***Interrupt***/
// cross out the ones being used and redefine in main
ISR(INT0_vect){ }
//ISR(INT1_vect){ }
ISR(INT2_vect){ }
ISR(INT3_vect){ }
ISR(INT4_vect){ }
ISR(INT5_vect){ }
ISR(INT6_vect){ }
ISR(INT7_vect){ }
/***EOF***/
