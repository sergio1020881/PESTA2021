/*************************************************************************
	ATMEGA128TIMER
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
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
#include "atmega128timer.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define STATUS_REGISTER SREG
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	/***0***/
	#define ATMEGA_TIMER_COUNTER
	#define TIMER_COUNTER0_CONTROL_REGISTER TCCR0
	#define TIMER_COUNTER0_REGISTER TCNT0
	#define TIMER_COUNTER0_COMPARE_REGISTER OCR0
	#define TIMER_COUNTER0_COMPARE_MATCH_INTERRUPT TIMER0_COMP_vect
	#define TIMER_COUNTER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
	/***1***/
	#define TIMER_COUNTER1A_CONTROL_REGISTER TCCR1A
	#define TIMER_COUNTER1B_CONTROL_REGISTER TCCR1B
	#define TIMER_COUNTER1C_CONTROL_REGISTER TCCR1C
	#define TIMER_COUNTER1_REGISTER TCNT1 // H and L register
	#define TIMER_COUNTER1A_COMPARE_REGISTER OCR1A
	#define TIMER_COUNTER1B_COMPARE_REGISTER OCR1B
	#define TIMER_COUNTER1C_COMPARE_REGISTER OCR1C
	#define TIMER_COUNTER1_INPUT_CAPTURE_REGISTER ICR1
	#define TIMER_COUNTER1A_COMPARE_MATCH_INTERRUPT TIMER1_COMPA_vect
	#define TIMER_COUNTER1B_COMPARE_MATCH_INTERRUPT TIMER1_COMPB_vect
	#define TIMER_COUNTER1C_COMPARE_MATCH_INTERRUPT TIMER1_COMPC_vect
	#define TIMER_COUNTER1_CAPTURE_EVENT_INTERRUPT TIMER1_CAPT_vect
	#define TIMER_COUNTER1_OVERFLOW_INTERRUPT TIMER1_OVF_vect
	/***2***/
	#define TIMER_COUNTER2_CONTROL_REGISTER TCCR2
	#define TIMER_COUNTER2_REGISTER TCNT2
	#define TIMER_COUNTER2_COMPARE_REGISTER OCR2
	#define TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT TIMER2_COMP_vect
	#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
	/***3***/
	#define TIMER_COUNTER3A_CONTROL_REGISTER TCCR3A
	#define TIMER_COUNTER3B_CONTROL_REGISTER TCCR3B
	#define TIMER_COUNTER3C_CONTROL_REGISTER TCCR3C
	#define TIMER_COUNTER3_REGISTER TCNT3 // H and L register
	#define TIMER_COUNTER3A_COMPARE_REGISTER OCR3A
	#define TIMER_COUNTER3B_COMPARE_REGISTER OCR3B
	#define TIMER_COUNTER3C_COMPARE_REGISTER OCR3C
	#define TIMER_COUNTER3_INPUT_CAPTURE_REGISTER ICR3
	#define TIMER_COUNTER3A_COMPARE_MATCH_INTERRUPT TIMER3_COMPA_vect
	#define TIMER_COUNTER3B_COMPARE_MATCH_INTERRUPT TIMER3_COMPB_vect
	#define TIMER_COUNTER3C_COMPARE_MATCH_INTERRUPT TIMER3_COMPC_vect
	#define TIMER_COUNTER3_CAPTURE_EVENT_INTERRUPT TIMER3_CAPT_vect
	#define TIMER_COUNTER3_OVERFLOW_INTERRUPT TIMER3_OVF_vect
	/***COMMON***/
	#define TIMER_COUNTER_STATUS_REGISTER ASSR
	#define TIMER_COUNTER_INTERRUPT_MASK_REGISTER TIMSK
	#define EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER ETIMSK
	#define TIMER_COUNTER_INTERRUPT_FLAG_REGISTER TIFR
	#define EXTENDED_TIMER_COUNTER_INTERRUPT_FLAG_REGISTER ETIFR
	#define TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER SFIOR
	#define ASYNCHRONOUS_STATUS_REGISTER ASSR
	#define SPECIAL_FUNCTION_IO_REGISTER SFIOR
#else
	#error "Not Atmega 128"
#endif
/***Global File Variables***/
unsigned char timer0_state;
unsigned char timer1_state;
unsigned char timer2_state;
unsigned char timer3_state;
/***Header***/
void TIMER_COUNTER0_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER0_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER0_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER0_compare(unsigned char compare);
void TIMER_COUNTER0_compareA(unsigned char compare);
void TIMER_COUNTER0_compareB(unsigned char compare);
void TIMER_COUNTER0_start(unsigned int prescaler);
void TIMER_COUNTER0_stop(void);
/******/
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeC(unsigned char compoutmode);
void TIMER_COUNTER1_compareA(uint16_t compare);
void TIMER_COUNTER1_compareB(uint16_t compare);
void TIMER_COUNTER1_compareC(uint16_t compare);
void TIMER_COUNTER1_start(unsigned int prescaler);
void TIMER_COUNTER1_stop(void);
/******/
void TIMER_COUNTER2_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER2_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER2_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER2_compare(unsigned char compare);
void TIMER_COUNTER2_compareA(unsigned char compare);
void TIMER_COUNTER2_compareB(unsigned char compare);
void TIMER_COUNTER2_start(unsigned int prescaler);
void TIMER_COUNTER2_stop(void);
/******/
void TIMER_COUNTER3_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER3_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER3_compoutmodeC(unsigned char compoutmode);
void TIMER_COUNTER3_compareA(uint16_t compare);
void TIMER_COUNTER3_compareB(uint16_t compare);
void TIMER_COUNTER3_compareC(uint16_t compare);
void TIMER_COUNTER3_start(unsigned int prescaler);
void TIMER_COUNTER3_stop(void);
/***Procedure & Function***/
TIMER_COUNTER0 TIMER_COUNTER0enable(unsigned char wavegenmode, unsigned char interrupt)
/***
PARAMETER SETTING
wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
interrupt: off; overflow; output compare; both; default - non.
***/
{
	TIMER_COUNTER0 timer0;
	timer0_state=0;
	TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<WGM00) | (1<<WGM01));
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM00);
			break;
		case 2: // CTC
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM01);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM00) | (1<<WGM01);
			break;
		default:
			break;
	}
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<TOIE0);
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<OCIE0);
	switch(interrupt){
		case 0: 
			break;
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE0);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE0);
			break;
		default:
			break;
	}
	timer0.compoutmode=TIMER_COUNTER0_compoutmode;
	timer0.compare=TIMER_COUNTER0_compare;
	timer0.start=TIMER_COUNTER0_start;
	timer0.stop=TIMER_COUNTER0_stop;
	return timer0;
}
/*****************************************************************************************/
TIMER_COUNTER1 TIMER_COUNTER1enable(unsigned char wavegenmode, unsigned char interrupt)
/***
PARAMETER SETTING
wavegen mode: Normal; PWM, Phase Correct, 8-bit; PWM, Phase Correct, 9-bit; PWM, Phase Correct, 10-bit;
CTC; Fast PWM, 8-bit; Fast PWM, 9-bit; Fast PWM, 10-bit; PWM, Phase and Frequency Correct; PWM, Phase and Frequency Correct;
PWM, Phase Correct; PWM, Phase Correct; CTC; (Reserved); Fast PWM; Fast PWM.
interrupt: off; overflow; output compare; both; default - non.
for more information read datasheet.
***/
{
	TIMER_COUNTER1 timer1;
	timer1_state=0;
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<WGM11) | (1<<WGM10));
	TIMER_COUNTER1B_CONTROL_REGISTER&=~((1<<WGM13) | (1<<WGM12));
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct, 8-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			break;
		case 2:	// PWM, Phase Correct, 9-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			break;
		case 3:	// PWM, Phase Correct, 10-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			break;
		case 4:	// CTC
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 5:	// Fast PWM, 8-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 6:	// Fast PWM, 9-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 7:	// Fast PWM, 10-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 8:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 9:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 10: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 11: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 12: // CTC
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 13: // (Reserved)
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 14: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 15: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		default:
			break;
	}
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((3<<COM1A0) | (3<<COM1B0) | (3<<COM1C0));
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TICIE1) | (1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1));
	EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<OCIE1C);
	switch(interrupt){
		case 0:
			break;
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B);
			break;
		case 4:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
			break;
		case 5:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE1);
			break;
		case 6:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<TOIE1);
			break;
		case 7:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B) | (1<<TOIE1);
			break;
		case 8:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
			break;
		case 9:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE1) | (1<<TOIE1);
			break;
		case 10:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
			break;
		case 11:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
			break;
		case 12:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B);
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
			break;
		default:
			break;
	}
	timer1.compoutmodeA=TIMER_COUNTER1_compoutmodeA;
	timer1.compoutmodeB=TIMER_COUNTER1_compoutmodeB;
	timer1.compoutmodeC=TIMER_COUNTER1_compoutmodeC;
	timer1.compareA=TIMER_COUNTER1_compareA;
	timer1.compareB=TIMER_COUNTER1_compareB;
	timer1.compareC=TIMER_COUNTER1_compareC;
	timer1.start=TIMER_COUNTER1_start;
	timer1.stop=TIMER_COUNTER1_stop;
	return timer1;
}
/*****************************************************************************************/
TIMER_COUNTER2 TIMER_COUNTER2enable(unsigned char wavegenmode, unsigned char interrupt)
/***
PARAMETER SETTING
wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
interrupt: off; overflow; output compare; both; default - non.
***/
{
	TIMER_COUNTER2 timer2;
	timer2_state=0;
	TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<WGM20) | (1<<WGM21));
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM20);
			break;
		case 2: // CTC
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM21);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM20) | (1<<WGM21);
			break;
		default:
			break;
	}
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TOIE2) | (1<<OCIE2));
	switch(interrupt){
		case 0: 
			break;
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
			break;
		default:
			break;
	}
	timer2.compoutmode=TIMER_COUNTER2_compoutmode;
	timer2.compare=TIMER_COUNTER2_compare;
	timer2.start=TIMER_COUNTER2_start;
	timer2.stop=TIMER_COUNTER2_stop;
	return timer2;
}
void TIMER_COUNTER0_start(unsigned int prescaler)
/***
PARAMETER SETTING
Frequency oscilator devision factor or prescaler.
prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
***/
{
	if(timer0_state==0){ // oneshot
		TIMER_COUNTER0_COMPARE_REGISTER=0XFF;
		TIMER_COUNTER0_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clk T0S /(No prescaling)
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<CS00);
				break;
			case 8: // clk T0S /8 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<CS01);
				break;
			case 32: // clk T0S /32 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(3<<CS00);
				break;
			case 64: // clk T0S /64 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(4<<CS00);
				break;
			case 128: // clk T0S /128 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(5<<CS00);
				break;
			case 256: // clk T 0 S /256 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(6<<CS00);
				break;
			case 1024: // clk T 0 S /1024 (From prescaler)
				TIMER_COUNTER0_CONTROL_REGISTER|=(7<<CS00);
				break;
			default:
				TIMER_COUNTER0_CONTROL_REGISTER|=(7<<CS00);
				break;
		}
		timer0_state=1;
	}	
}
void TIMER_COUNTER0_compoutmode(unsigned char compoutmode)
/***
compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
default-Normal port operation, OC0 disconnected.
***/
{
	TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<COM00) | (1<<COM01));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM00);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM01);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM00) | (1<<COM01);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER0_compare(unsigned char compare)
{
	TIMER_COUNTER0_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER0_stop(void)
/***
stops timer by setting prescaler to zero
***/
{
	TIMER_COUNTER0_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER0_REGISTER=0X00;
	timer0_state=0;
}
/*****************************************************************************************/
void TIMER_COUNTER1_start(unsigned int prescaler)
/***
PARAMETER SETTING
Frequency oscilator devision factor or prescaler.
prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
***/
{
	if(timer1_state==0){ // oneshot
		TIMER_COUNTER1A_COMPARE_REGISTER=0XFFFF;
		TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/1 (No prescaling
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS10);
				break;
			case 8: // clkI/O/8 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS11);
				break;
			case 64: // clkI/O/64 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(3<<CS10);
				break;
			case 256: // clkI/O/256 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS12);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
				break;
			case 3: // External clock source on Tn pin. Clock on falling edge
				TIMER_COUNTER1B_CONTROL_REGISTER|=(6<<CS10);
				break;
			case 5: // External clock source on Tn pin. Clock on rising edge
				TIMER_COUNTER1B_CONTROL_REGISTER|=(7<<CS10);
				break;
			default:
				TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
				break;
		}
		timer1_state=1;
	}	
}
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1A0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0) | (1<<COM1A1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1B0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0) | (1<<COM1B1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER1_compoutmodeC(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1C0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C0) | (1<<COM1C1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER1_compareA(uint16_t compare)
{
	TIMER_COUNTER1A_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER1_compareB(uint16_t compare)
{
	TIMER_COUNTER1B_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER1_compareC(uint16_t compare)
{
	TIMER_COUNTER1C_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER1_stop(void)
/***
stops timer by setting prescaler to zero
***/
{
	TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER1_REGISTER=0X0000;
	timer1_state=0;
}
/*****************************************************************************************/
void TIMER_COUNTER2_start(unsigned int prescaler)
/***
PARAMETER SETTING
Frequency oscilator devision factor or prescaler.
prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
***/
{
	if(timer2_state==0){ // oneshot
		TIMER_COUNTER2_COMPARE_REGISTER=0XFF;
		TIMER_COUNTER2_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/(No prescaling)
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS20);
				break;
			case 8: // clkI/O/8 (From prescaler)
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS21);
				break;
			case 64: // clkI/O/64 (From prescaler)
				TIMER_COUNTER2_CONTROL_REGISTER|=(3<<CS20);
				break;
			case 256: // clkI/O/256 (From prescaler)
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS22);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				TIMER_COUNTER2_CONTROL_REGISTER|=(5<<CS20);
				break;
			case 3: // External clock source on T2 pin. Clock on falling edge
				TIMER_COUNTER2_CONTROL_REGISTER|=(6<<CS20);
				break;
			case 5: // External clock source on T2 pin. Clock on rising edge
				TIMER_COUNTER2_CONTROL_REGISTER|=(7<<CS20);
				break;
			default:
				TIMER_COUNTER2_CONTROL_REGISTER|=(5<<CS20);
				break;
		}
		timer2_state=1;
	}	
}
void TIMER_COUNTER2_compoutmode(unsigned char compoutmode)
/***
compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
default-Normal port operation, OC0 disconnected.
***/
{
	TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<COM20) | (1<<COM21));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM21);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20) | (1<<COM21);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER2_compare(unsigned char compare)
{
	TIMER_COUNTER2_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER2_stop(void)
/***
stops timer by setting prescaler to zero
***/
{
	TIMER_COUNTER2_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER2_REGISTER=0X00;
	timer2_state=0;
}
/*****************************************************************************************/
TIMER_COUNTER3 TIMER_COUNTER3enable(unsigned char wavegenmode, unsigned char interrupt)
/***
PARAMETER SETTING
wavegen mode: Normal; PWM, Phase Correct, 8-bit; PWM, Phase Correct, 9-bit; PWM, Phase Correct, 10-bit;
CTC; Fast PWM, 8-bit; Fast PWM, 9-bit; Fast PWM, 10-bit; PWM, Phase and Frequency Correct; PWM, Phase and Frequency Correct;
PWM, Phase Correct; PWM, Phase Correct; CTC; (Reserved); Fast PWM; Fast PWM.
interrupt: off; overflow; output compare; both; default - non.
for more information read datasheet.
***/
{
	TIMER_COUNTER3 timer3;
	timer3_state=0;
	TIMER_COUNTER3A_CONTROL_REGISTER&=~((1<<WGM31) | (1<<WGM30));
	TIMER_COUNTER3B_CONTROL_REGISTER&=~((1<<WGM33) | (1<<WGM32));
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct, 8-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
			break;
		case 2:	// PWM, Phase Correct, 9-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
			break;
		case 3:	// PWM, Phase Correct, 10-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
			break;
		case 4:	// CTC
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
			break;
		case 5:	// Fast PWM, 8-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
			break;
		case 6:	// Fast PWM, 9-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
			break;
		case 7:	// Fast PWM, 10-bit
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
			break;
		case 8:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
			break;
		case 9:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
			break;
		case 10: // PWM, Phase Correct
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
			break;
		case 11: // PWM, Phase Correct
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
			break;
		case 12: // CTC
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
			break;
		case 13: // (Reserved)
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
			break;
		case 14: // Fast PWM
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
			break;
		case 15: // Fast PWM
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
			TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
			break;
		default:
			break;
	}
	TIMER_COUNTER3A_CONTROL_REGISTER&=~((3<<COM3A0) | (3<<COM3B0)| (3<<COM3C0));
	EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TICIE3) | (1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3) | (1<<OCIE3C));
	switch(interrupt){
		case 0:
			break;
		case 1:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER=(1<<TOIE3);
			break;
		case 2:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A);
			break;
		case 3:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3B);
			break;
		case 4:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3C);
			break;
		case 5:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE3);
			break;
		case 6:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<TOIE3);
			break;
		case 7:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3B) | (1<<TOIE3);
			break;
		case 8:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE3) | (1<<OCIE3C);
			break;
		case 9:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE3) | (1<<TOIE3);
			break;
		case 10:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3);
			break;
		case 11:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3) | (1<<OCIE3C);
			break;
		case 12:
			EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C);
			break;
		default:
			break;
	}
	//
	timer3.compoutmodeA=TIMER_COUNTER3_compoutmodeA;
	timer3.compoutmodeB=TIMER_COUNTER3_compoutmodeB;
	timer3.compoutmodeC=TIMER_COUNTER3_compoutmodeC;
	timer3.compareA=TIMER_COUNTER3_compareA;
	timer3.compareB=TIMER_COUNTER3_compareB;
	timer3.compareC=TIMER_COUNTER3_compareC;
	timer3.start=TIMER_COUNTER3_start;
	timer3.stop=TIMER_COUNTER3_stop;
	return timer3;
}
void TIMER_COUNTER3_start(unsigned int prescaler)
/***
PARAMETER SETTING
Frequency oscilator devision factor or prescaler.
prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
***/
{
	if(timer3_state==0){ // oneshot
		TIMER_COUNTER3A_COMPARE_REGISTER=0XFFFF;
		TIMER_COUNTER3B_CONTROL_REGISTER&=~(7<<CS30); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/1 (No prescaling
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS30);
				break;
			case 8: // clkI/O/8 (From prescaler)
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS31);
				break;
			case 64: // clkI/O/64 (From prescaler)
				TIMER_COUNTER3B_CONTROL_REGISTER|=(3<<CS30);
				break;
			case 256: // clkI/O/256 (From prescaler)
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS32);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				TIMER_COUNTER3B_CONTROL_REGISTER|=(5<<CS30);
				break;
			case 3: // External clock source on Tn pin. Clock on falling edge
				TIMER_COUNTER3B_CONTROL_REGISTER|=(6<<CS30);
				break;
			case 5: // External clock source on Tn pin. Clock on rising edge
				TIMER_COUNTER3B_CONTROL_REGISTER|=(7<<CS30);
				break;
			default:
				TIMER_COUNTER3B_CONTROL_REGISTER|=(5<<CS30);
				break;
		}
		timer3_state=1;
	}	
}
void TIMER_COUNTER3_compoutmodeA(unsigned char compoutmode)
{
	TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3A0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A0) | (1<<COM3A1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER3_compoutmodeB(unsigned char compoutmode)
{
	TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3B0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B0) | (1<<COM3B1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER3_compoutmodeC(unsigned char compoutmode)
{
	TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3C0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C0) | (1<<COM3C1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER3_compareA(uint16_t compare)
{
	TIMER_COUNTER3A_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER3_compareB(uint16_t compare)
{
	TIMER_COUNTER3B_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER3_compareC(uint16_t compare)
{
	TIMER_COUNTER3C_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER3_stop(void)
/***
stops timer by setting prescaler to zero
***/
{
	TIMER_COUNTER3B_CONTROL_REGISTER&=~(7<<CS30); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER3_REGISTER=0X0000;
	timer3_state=0;
}
/***Interrupt***/
// So that main will not freeze when not defined, therefore exclude here! before use.
//ISR(TIMER0_COMP_vect){ }
ISR(TIMER0_OVF_vect){ }
ISR(TIMER1_CAPT_vect){ }
ISR(TIMER1_COMPA_vect){ }
ISR(TIMER1_COMPB_vect){ }
ISR(TIMER1_COMPC_vect){ }
ISR(TIMER1_OVF_vect){ }
ISR(TIMER2_OVF_vect){ }
ISR(TIMER3_CAPT_vect){ }
ISR(TIMER3_COMPA_vect){ }
ISR(TIMER3_COMPB_vect){ }
ISR(TIMER3_COMPC_vect){ }
ISR(TIMER3_OVF_vect){ }
/***EOF***/
