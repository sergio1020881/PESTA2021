/************************************************************************
Title: HX711_TEST.c
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
File: $Id: MAIN,v 1.8.2.1 21/02/2021 Exp $
Software: Atmel Studio 7 (ver 7.0.129)
Hardware: Atmega128 by ETT ET-BASE
	-PORTA LCD
	-PORTE Keyboard
License: GNU General Public License
Comment:

************************************************************************/
#define F_CPU 16000000UL
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "atmega128timer.h"
#include "function.h"
#include "lcd.h"
#include "hx711.h"
/*
** Constant and Macro
*/
#define TRUE 1
#define ZERO 0
#define repeat 59
/*
** Global File variable
*/
FUNC function;
LCD0 lcd0;
HX711 hx;
uint8_t count_1=0;
uint8_t count_2=0;
uint8_t ADD0;
uint32_t hx_adc;
//uint8_t vector[4];
/*
** Header
*/
void PORTINIT();
/****MAIN****/
int main(void)
{
	//SREG|=1<<7;
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	function = FUNCenable();
	lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(2,2); //2,2
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(4,0);
	hx = HX711enable(&DDRF, &PINF, &PORTF, 6, 7);
	/******/
	char Menu='1'; // Main menu selector
	/***Parameters timers***/
	//vector[0]=255;
	//vector[1]=255;
	//vector[2]=255;
	//uint8_t* ptr=vector; 
	timer0.compare(100);
	timer0.start(1);//1	1024
	timer0.compoutmode(1);
	timer1.compareA(50);
	timer1.compoutmodeA(1);
	timer1.start(256);
	hx.set_amplify(&hx,64);
	/**********/
	//TODO:: Please write your application code
	while(TRUE){
		/***PREAMBLE***/
		lcd0.reboot();
		switch(Menu){
			/***MENU 1***/
			case '1': // Main Program Menu
				//if(!strcmp(keypad.get().string,"A")){Menu='2';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"B")){Menu='3';keypad.flush();lcd0.clear();break;}					
		
				//lcd0.gotoxy(0,0);
				//lcd0.string_size("program running",15);
				
				/***Play around***/
				//if(!(PINF & 64)){
					//hx.set_readflag(&hx);
					//PORTC&=~(1<<0);
				//}
				
				
				//lcd0.gotoxy(3,0);
				//lcd0.string_size(function.i32toa(*((int32_t*)ptr)),15);
					
				break;
			/***MENU 2***/
			case '2': //  
				//if(!strcmp(keypad.get().string,"A")){Menu='3';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"B")){Menu='1';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"C")){Menu='1';keypad.flush();lcd0.clear();break;}
					lcd0.gotoxy(0,0);
					lcd0.string_size("Not being used",19);
					/***Play around***/
				break;
			/***MENU 3***/
			case '3': //Set Time and Date
				//if(!strcmp(keypad.get().string,"A")){Menu='1';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"B")){Menu='2';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"C")){Menu='1';keypad.flush();lcd0.clear();break;}
					lcd0.gotoxy(0,0);
					lcd0.string_size("Not being used",19);
					/***Play around***/
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
	DDRC=0xFF;
	PORTC=0xFF;
	//DDRB=0x10;
	//PORTB=0x10;
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect)
{
	uint8_t Sreg;
	Sreg=SREG;
	SREG&=~(1<<7);
	uint32_t value;
	
	value=(hx.read(&hx)/100)-607;
	
	if(count_1 > 0){
		count_2++;
		lcd0.gotoxy(0,0);
		lcd0.string_size(function.i16toa(count_2), 15);
		lcd0.gotoxy(1,0);
		lcd0.string_size(function.i32toa(value), 15);
		PORTC|=(1<<0);
		
		
		
		count_1=0;
	}
	count_1++;
	SREG=Sreg;
}
/***EOF***/
/**** Comment:
The past only exists if the present comes to be. There is no future only possibilities.
because 24 bit will have to create a vector pointer of the size of 32 bit, then at the end do a cast to *((int32_t*)ptr)
****/
