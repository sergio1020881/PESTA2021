/************************************************************************
Title: HX711_TEST.c
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
File: $Id: MAIN,v 1.8.2.1 21/02/2021 Exp $
Software: Atmel Studio 7 (ver 7.0.129)
Hardware: Atmega128 by ETT ET-BASE
	-PORTA LCD
	-PORTF pin 6,7 HX711, pin 0 to 5 Buttons.
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
#include "explode.h"
#include "atmega128timer.h"
#include "function.h"
#include "lcd.h"
#include "hx711.h"
#include "eeprom.h"
/*
** Constant and Macro
*/
#define TRUE 1
#define ZERO 0
#define ONE 1
#define average_n 3
#define _5sec 5
#define _10sec 10
/*
** Global File variable
*/
EXPLODE F;
struct HX711_calibration HX711_data;
struct HX711_calibration* HX711_ptr;
const uint8_t sizeblock=sizeof(struct HX711_calibration);
HX711 hx;
int32_t tmp;
EEPROM eprom;

char result[32];
uint8_t counter_1=0;
/*
** Header
*/
void PORTINIT();
/****MAIN****/
int main(void)
{
	HX711_ptr=&HX711_data;
	
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	F = EXPLODEenable();
	FUNC function = FUNCenable();
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(2,2); //2,2
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(4,2);
	hx = HX711enable(&DDRF, &PINF, &PORTF, 6, 7);
	eprom = EEPROMenable();
	/******/
	char Menu='1'; // Main menu selector
	float value_64=0;
	float value_128=0;
	
	// Get default values to memory
	HX711_data.offset_32=hx.get_offset_32(&hx);
	HX711_data.offset_64=hx.get_offset_64(&hx);
	HX711_data.offset_128=hx.get_offset_128(&hx);
	HX711_data.divfactor_32=hx.get_divfactor_32(&hx);
	HX711_data.divfactor_64=hx.get_divfactor_64(&hx);
	HX711_data.divfactor_128=hx.get_divfactor_128(&hx);

	tmp=0;
	/***Parameters timers***/
	//vector[0]=255;
	//vector[1]=255;
	//vector[2]=255;
	//uint8_t* ptr=vector;
	timer0.compoutmode(1);
	timer0.compare(79); // 1 -> 159 -> 20 us, 1 -> 79 -> 10 us, 1 -> 15 -> 2 us, 8 -> 99 -> 100 us, 8 -> 79 -> 80 us
	timer0.start(8); // 1 -> 32 us , 8 -> 256 us , 32 64 128 256 1024
	
	// to be used to jump menu for calibration in progress
	timer1.compoutmodeA(1);
	timer1.compareA(62800); // 256 -> 62800 -> 2 s
	timer1.start(256);
	
	// HX711 Gain
	hx.set_amplify(&hx,64); // 32 64 128
	
	//Get stored calibration values and put them to effect
	eprom.read_block(HX711_ptr, (const void*) ZERO, sizeblock);
	if(HX711_ptr->status == 1){
		memcpy ( hx.ptrcal(&hx), HX711_ptr, sizeblock );
		PORTC^=(ONE<<5); // troubleshooting
	}
	/***********************************************************************************************/
	while(TRUE){
		/******PREAMBLE******/
		lcd0.reboot();
		/*******INPUT*******/
		F.boot(&F,PINF);
		
		/*******************/
		switch(Menu){
			/***MENU 1***/
			case '1': // Main Program Menu
				//if(!strcmp(keypad.get().string,"A")){Menu='2';keypad.flush();lcd0.clear();break;}
				//if(!strcmp(keypad.get().string,"B")){Menu='3';keypad.flush();lcd0.clear();break;}					
				
				//Just to keep track
				lcd0.gotoxy(0,4); //for troubleshooting
				lcd0.string_size("Weight Scale", 12); //for troubleshooting
				//lcd0.string_size(function.i32toa(tmp), 8); lcd0.string_size("raw", 3); // RAW_READING //for troubleshooting
				
				value_64=hx.raw_average(&hx, 25); // 25 50, smaller means faster or more readings
				lcd0.gotoxy(1,0);
				lcd0.string_size(function.ftoa(value_64, result, 0), 12); lcd0.string_size("raw_av", 6);
				
				if(F.hl(&F) & ONE){ // calibrate offset by pressing button 1
					PORTC^=(ONE<<5); // troubleshooting
					HX711_data.offset_64=value_64;
					//HX711_data.divfactor_64=46; //for troubleshooting
					HX711_data.status=1;
					eprom.update_block(HX711_ptr, (void*) ZERO, sizeblock);
					//lcd0.gotoxy(2,0); //for troubleshooting
					//lcd0.string_size(function.i32toa(hx.get_offset_64(&hx)), 8); //for troubleshooting
					//lcd0.string_size(function.i16toa(hx.get_divfactor_64(&hx)), 8); //for troubleshooting
					memcpy ( hx.ptrcal(&hx), HX711_ptr, sizeblock ); // Update new values
				}
				
				//value_128=(value_64-hx.cal.offset_128)/hx.cal.divfactor_128;
				value_64=(value_64-hx.cal.offset_64)/hx.cal.divfactor_64; //value to be published to LCD
				
				//Display
				if (value_64 > 1000 || value_64 < -1000){
					value_64 = value_64/1000;
					value_128 = value_128/1000;
					lcd0.gotoxy(3,0);
					lcd0.string_size(function.ftoa(value_64,result,3), 12); //lcd0.string_size("Kg", 4);
					//lcd0.gotoxy(3,0);
					//lcd0.string_size(function.ftoa(value_128,result,3), 12); lcd0.string_size("Kg", 4);	
				}else{
					lcd0.gotoxy(3,0);
					lcd0.string_size(function.ftoa(value_64,result,0), 12); //lcd0.string_size("gram", 4);
					//lcd0.gotoxy(3,0);
					//lcd0.string_size(function.ftoa(value_128,result,0), 12); lcd0.string_size("gram", 4);
				}		
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
	//Control buttons
	PORTF|=0x3F;
	//troubleshooting output
	DDRC=0xFF;
	PORTC=0xFF;
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect) // 20 us intervals
{
	/***Block other interrupts during this procedure***/
	uint8_t Sreg;
	Sreg=SREG;
	SREG&=~(ONE<<7);
	/***Block other interrupts during this procedure***/	
	tmp=hx.read_raw(&hx);
	/***enable interrupts again***/
	SREG=Sreg;
}
ISR(TIMER1_COMPA_vect) // 1 second intervals
{
	
	if(F.ll(&F) & ONE)
		counter_1++;
		
	if(counter_1 > _5sec){
		counter_1=_5sec+ONE; //lock in place
		
		PORTC^=(ONE<<5); // troubleshooting
		
		if(F.ll(&F) & 2){
			// Delete eerpom memory ZERO
			HX711_data.status=0;
			eprom.update_block(HX711_ptr, (void*) ZERO, sizeblock);
			PORTC^=(ONE<<5); // troubleshooting
			counter_1=ZERO;
		}
	}
}
/***EOF***/
/**** Comment:
The past only exists if the present comes to be. There is no future only possibilities.
because 24 bit will have to create a vector pointer of the size of 32 bit, then at the 
end do a cast to *((int32_t*)ptr).

Have to make function for average, then offset. DONE.

After that create a calibration section. In progress.

Then it is all extras.

changed rate in HX711 to 80Hz, 10Hz is to slow. Though there is more noise. DONE.

Temos aparelhos de medida para não ter necessidade de recorrer a calculos complexos, cut out all the red tape.

There is a bug with this program, sometimes does not accept changed code, only by checking using print to 
LCD makes it accept it. Stupid bugs, makes you think something is wrong when there is non, the damn compiler fails.
****/
