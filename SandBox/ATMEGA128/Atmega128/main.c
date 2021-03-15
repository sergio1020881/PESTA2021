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
#define average_n 3
/*
** Global File variable
*/
HX711 hx;
int32_t tmp;

char result[20];
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
	FUNC function = FUNCenable();
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(2,2); //2,2
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(4,0);
	hx = HX711enable(&DDRF, &PINF, &PORTF, 6, 7);
	/******/
	char Menu='1'; // Main menu selector
	float value=0;
	tmp=0;
	/***Parameters timers***/
	//vector[0]=255;
	//vector[1]=255;
	//vector[2]=255;
	//uint8_t* ptr=vector;
	timer0.compoutmode(1);
	timer0.compare(79); // 1 -> 159 -> 20us, 1 -> 79 -> 10 us, 8 -> 99 -> 100 us
	timer0.start(1); // 1 -> 32 us , 8 -> 256 us , 32 64 128 256 1024
	
	// to be used to jump menu for calibration in progress
	timer1.compareA(50);
	timer1.compoutmodeA(1);
	timer1.start(256);
	hx.set_amplify(&hx,64); // 32 64 128
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
		
				value=hx.average(&hx,tmp,4);
				value=(value-hx.cal.offset_64)/hx.cal.divfactor_64;
				
				//Just to keep track
				lcd0.gotoxy(0,0);
				lcd0.string_size(function.i32toa(tmp), 8); lcd0.string_size("raw", 3); // RAW_READING
				//lcd0.gotoxy(1,0);
				//lcd0.string_size(function.ftoa(value,result,2), 8);
				//lcd0.gotoxy(2,0);
				//lcd0.string_size(function.ftoa((value-hx.cal.offset_128)/hx.cal.divfactor_128,result,0), 12); lcd0.string_size("gram", 4);
				
				
				//lcd0.gotoxy(2,0);
				//lcd0.string_size(function.ftoa(value,result,0), 12); lcd0.string_size("Kg", 4);
				//Display
				if (value > 999 || value < -999){
					value = value/1000;
					lcd0.gotoxy(3,0);
					lcd0.string_size(function.ftoa(value,result,3), 12); lcd0.string_size("Kg", 4);
				}else{
					lcd0.gotoxy(3,0);
					lcd0.string_size(function.ftoa(value,result,0), 12); lcd0.string_size("gram", 4);
				}
				
				//(value-73990)/46
					
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
	/***Block other interrupts during this procedure***/
	uint8_t Sreg;
	Sreg=SREG;
	SREG&=~(1<<7);
	/***Block other interrupts during this procedure***/	
	tmp=hx.readraw(&hx);

	PORTC^=(1<<0);
	
	/***enable interrupts again***/
	SREG=Sreg;
}
/***EOF***/
/**** Comment:
The past only exists if the present comes to be. There is no future only possibilities.
because 24 bit will have to create a vector pointer of the size of 32 bit, then at the 
end do a cast to *((int32_t*)ptr).

Have to make function for average, then offset.

After that create a calibration section.

Then it is all extras.

changed rate in HX711 to 80Hz, 10Hz is to slow. Though there is more noise.

Temos aparelhos de medida para não ter necessidade de recorrer a calculos complexos, cut out all the red tape.
****/
