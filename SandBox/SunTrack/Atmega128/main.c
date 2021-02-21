/************************************************************************
Title: SUNTRACK.c
Author: Sergio Manuel Santos 
	<sergio.salazar.santos@gmail.com>
File: $Id: MAIN,v 1.8.2.1 21/10/2020 Exp $
Software: Atmel Studio 7 (ver 7.0.129)
Hardware: Atmega128 by ETT ET-BASE
	-PORTA LCD
	-PORTE Keyboard
	-PF0 Sensor LDR
	-PB6 Servo Motor
	-PORTD RTC and UART1->FTDI chip->PC Putty
	-PORTG HC595
License: GNU General Public License
Comment:
	Nice
************************************************************************/
#define F_CPU 16000000UL
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "atmega128analog.h"
#include "atmega128timer.h"
#include "function.h"
#include "lcd.h"
#include "pcf8563rtc.h"
#include "keypad.h"
#include "74hc595.h"
#include "znpid.h"
#include "atmega128uart.h"
/*
** Constant and Macro
*/
#define TRUE 1
#define ZERO 0
#define Min 500     // 450 PWM servo motor
#define Max 2350    // 2450 PWM servo motor
/*
** Global File variable
*/
struct time tm; // time struct RTC
struct date dt; // date struct RTC
HC595 shift;
FUNC function;
ZNPID pid;
uint8_t count=0; // 1Hz
uint8_t increment=0; // 1Hz
char* ptr=NULL; // pointing to analog reading string
uint16_t adcvalue; // analog reading
float pid_out;
/*
** Header
*/
void PORTINIT();
/****MAIN****/
int main(void)
{
	PORTINIT(); // Inic Ports
	/***INICIALIZE OBJECTS***/
	function= FUNCenable(); // Function Library
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA); // LCD Display 4X20
	KEYPAD keypad = KEYPADenable(&DDRE,&PINE,&PORTE); // Keyboard
	ANALOG analog = ANALOGenable(1, 128, 1, 0); // Channel 0 for Position
	TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(2,2); // 1Hz to HC595
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(9,0); // PWM Positioning
	shift = HC595enable(&DDRG,&PORTG,2,0,1);
	pid = ZNPIDenable();
	/******/
	char Menu='1'; // Main menu selector
	char str[6]="0"; // analog vector
	int16_t mvalue=90; // manual position reading
	int16_t m_value; // manual positioning
	char mstr[6]="90"; // manual position vector
	ptr=str;
	/***Parameters timers***/
	timer0.compare(249);
	timer0.start(64);
	timer1.compoutmodeB(2);
	timer1.compareA(20000);
	timer1.start(8);
	pid.set_kc(&pid, 1);
	pid.set_kd(&pid, 1); //
	pid.set_ki(&pid, 0.1); // will provoke overshoot, to much acceleration limit max value and minimum value.
	pid.set_SP(&pid, 520);
	/**********/
	//TODO:: Please write your application code
	while(TRUE){
		/***PREAMBLE***/
		lcd0.reboot();
		keypad.read();
		//uartreceive=uart.read();
		/***Reading input***/
		lcd0.gotoxy(3,13);
		lcd0.putch(':');
		lcd0.string_size(keypad.get().printstring,6);
		/***ENTRY END***/
		switch(Menu){
			/***MENU 1***/
			case '1': // Main Program Menu
				if(!strcmp(keypad.get().string,"A")){Menu='2';keypad.flush();lcd0.clear();break;}
				if(!strcmp(keypad.get().string,"B")){Menu='3';keypad.flush();lcd0.clear();break;}
					/***Reading analog***/
					adcvalue=analog.read(0);
					lcd0.gotoxy(0,0);
					lcd0.string_size("Sense: ",7);
					strcpy(str,function.i16toa(adcvalue));
					lcd0.string_size(str,4);
					
					// PID output
					lcd0.gotoxy(0,13);
					strcpy(str,function.i32toa(pid_out));
					lcd0.string_size(str,6);
					
					
					
				break;
			/***MENU 2***/
			case '2': // Manual position override 
				if(!strcmp(keypad.get().string,"A")){Menu='3';keypad.flush();lcd0.clear();break;}
				if(!strcmp(keypad.get().string,"B")){Menu='1';keypad.flush();lcd0.clear();break;}
				if(!strcmp(keypad.get().string,"C")){Menu='1';keypad.flush();lcd0.clear();break;}
					lcd0.gotoxy(0,0);
					lcd0.string_size("Manual: ",8);
					lcd0.string_size(mstr,3);
					lcd0.gotoxy(1,0);
					lcd0.string_size("Enter Angle",11);
					if(keypad.get().character==KEYPADENTERKEY){
						strncpy(mstr,keypad.get().string,6);
						mvalue=function.strToInt(mstr);
						if(mvalue >=0 && mvalue <181){
							m_value=mvalue;
							timer1.compareB(function.trimmer(m_value,0,180,Min,Max));
							lcd0.hspace(5);
						}else{
							lcd0.string_size("  err",5);
						}
						keypad.flush();
					}
					//else
					//	timer1.compareB(function.trimmer(m_value,0,180,Min,Max));
					lcd0.gotoxy(3,0);
					lcd0.string_size("C - exit",8);
				break;
			/***MENU 3***/
			case '3': //Set Time and Date
				if(!strcmp(keypad.get().string,"A")){Menu='1';keypad.flush();lcd0.clear();break;}
				if(!strcmp(keypad.get().string,"B")){Menu='2';keypad.flush();lcd0.clear();break;}
				if(!strcmp(keypad.get().string,"C")){Menu='1';keypad.flush();lcd0.clear();break;}
					lcd0.gotoxy(0,0);
					lcd0.string_size("Not being used",19);
				break;
				/********************************************************************/
			default:
				Menu='1';
				break;
		};
	}
}
/*
** procedure and function
*/
void PORTINIT(void)
{
	//INPUT
	DDRF=0x00;
	PORTF=0x0F;
	//OUTPUT
	DDRB|=(1<<5) | (1<<6) | (1<<7);
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect) // 1Hz and usart Tx
{
	uint8_t Sreg;
	Sreg=SREG;
	SREG&=~(1<<7);
	if(count>59){ //59 -> 1Hz
		pid_out=pid.output(&pid,adcvalue,1);
		increment++;
		if((increment & 0x0F) < 8){
			shift.bit(0);
			shift.out();
		}else{
			shift.bit(1);
			shift.out();
		}
		count=0;
	}else
		count++;
	SREG=Sreg;
}
/***EOF***/
/**** Comment:
The past only exists if the present comes to be. There is no future only possibilities.
****/
