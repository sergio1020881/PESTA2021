/************************************************************************
Title: BALANCA COMERCIAL
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
File: $Id: MAIN,v 1.8.2.1 21/02/2021 Exp $
License: GNU General Public License
Software: Atmel Studio 7 (ver 7.0.129)
Hardware: Atmega128 by ETT ET-BASE
	-PORTA LCD
	-PORTF pin 6,7 HX711, pin 0 to 5 Buttons, 
		PIN 0 -> OFFSET, 
		PIN 3 -> DEFAULT 5sec press, and up count for div factor, 
		PIN 4 -> ENTER GAIN FACTOR MENU 5 sec press, and down count for div factor, 
		PIN 5 -> ENTER KEY to validate entered value and put in effect.
		PIN 6 and 7 -> dedicated to comunicate with Signal Amplifier
	-PORTC status indicator leds
		PIN 5 -> Indicate using stored values
		PIN 6 -> Reset to default indicator (blinks four times)
		PIN 7 -> In Calibratio Menu (blinking)
Comment:
	nice
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
//#include "atmega128interrupt.h"
#include "atmega128timer.h"
#include "function.h"
#include "lcd.h"
#include "hx711.h"
#include "eeprom.h"
/*
** Constant and Macro
*/
#ifndef STATUS_REGISTER
	#define STATUS_REGISTER SREG
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define ONE 1
#define TRUE 1
#define average_n 24 //64 -> 24
#define blink 8
#define IMASK 0x3F
#define _5sec 5
#define _10sec 10
#define minDIV 1
#define maxDIV 255
/*
** Global File variable
*/
EXPLODE F;
LCD0 lcd0;
TIMER_COUNTER0 timer0;
//INTERRUPT intx;
HX711_calibration HX711_data;
HX711_calibration* HX711_ptr;
const uint8_t sizeblock = sizeof(HX711_calibration);
HX711 hx;
float tmp;
EEPROM eprom;

char result[32];
char Menu = '1'; // Main menu selector
uint8_t counter_1 = ZERO;
uint8_t counter_2 = ZERO;
uint8_t signal = ZERO;
uint8_t count=blink;
uint16_t divfactor;
/*
** Header
*/
void PORTINIT();
/****MAIN****/
int main(void)
{
	PORTINIT();
	HX711_ptr = &HX711_data; // CALIBRATION DATA BUS
	/***INICIALIZE OBJECTS***/
	F = EXPLODEenable();
	FUNC function = FUNCenable();
	lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	timer0 = TIMER_COUNTER0enable(2,2); //2,2
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(4,2); //4,2
	hx = HX711enable(&DDRF, &PINF, &PORTF, 6, 7); //6,7
	eprom = EEPROMenable();
	//intx = INTERRUPTenable();
	/******/
	float value = 0;
	float publish = 0;
	uint8_t choice;
	// Get default values to buss memory
	HX711_data.offset_32 = hx.get_cal(&hx)->offset_32;
	HX711_data.offset_64 = hx.get_cal(&hx)->offset_64;
	HX711_data.offset_128 = hx.get_cal(&hx)->offset_128;
	HX711_data.divfactor_32 = hx.get_cal(&hx)->divfactor_32;
	HX711_data.divfactor_64 = hx.get_cal(&hx)->divfactor_64;
	HX711_data.divfactor_128 = hx.get_cal(&hx)->divfactor_128;
	HX711_data.status = hx.get_cal(&hx)->status;
	/***Parameters timers***/
	timer0.compoutmode(1); // troubleshooting blinking PORTB 5
	/***79 and 8  -> 80 us***/
	timer0.compare(60); // 8 -> 79 -> 80 us, fine tunned = 8 -> 60 -> 30.4us
	timer0.start(8); // 1 -> 32 us , 8 -> 256 us , 32 64 128 256 1024
	// to be used to jump menu for calibration in progress
	timer1.compoutmodeA(1); // troubleshooting blinking PORTB 6
	timer1.compareA(62800); // Freq = 256 -> 62800 -> 2 s
	timer1.start(256);
	//intx.set(1,0); // Not necessary, if used move IDC from PORTF to PORTD with new config pinage.
	// HX711 Gain
	hx.set_amplify(&hx, 64); // 32 64 128
	choice = hx.get_amplify(&hx);
	if(choice == 1)
		divfactor = (uint16_t) HX711_data.divfactor_128;
	if(choice == 2)
		divfactor = (uint16_t) HX711_data.divfactor_32;
	if(choice == 3)
		divfactor = (uint16_t) HX711_data.divfactor_64;
	//Get stored calibration values and put them to effect
	eprom.read_block(HX711_ptr, (const void*) ZERO, sizeblock);
	if(HX711_ptr->status == 1){
		//Load stored value 
		hx.get_cal(&hx)->offset_32 = HX711_ptr->offset_32;
		hx.get_cal(&hx)->offset_64 = HX711_ptr->offset_64;
		hx.get_cal(&hx)->offset_128 = HX711_ptr->offset_128;
		hx.get_cal(&hx)->divfactor_32 = HX711_ptr->divfactor_32;
		hx.get_cal(&hx)->divfactor_64 = HX711_ptr->divfactor_64;
		hx.get_cal(&hx)->divfactor_128 = HX711_ptr->divfactor_128;
		hx.get_cal(&hx)->status=ZERO;
		PORTC &= ~(ONE << 5); // troubleshooting
	}
	/*********************************************************/
	//lcd0.gotoxy(1,0); // for troubleshooting
	//lcd0.string_size(function.ftoa(HX711_data.status, result, ZERO), 13);
	//lcd0.string_size(function.ftoa(hx.get_cal(&hx)->offset_64, result, ZERO), 13);
	/*********************************************************/
	while(TRUE){
		/******PREAMBLE******/
		lcd0.reboot(); //Reboot LCD
		F.boot(&F,PINF); //PORTF INPUT READING
		while(hx.query(&hx)); //Catches falling Edge instance, begins bit shifting.
		/***geting data interval***/
		/************INPUT***********/
		// Jump Menu signal
		if(signal == ONE){ //INPUT FROM INTERRUPT SINALS
			Menu = '2';
			signal = ZERO; // ONE SHOT
			lcd0.clear();
		}
		tmp = hx.raw_average(&hx, average_n); // average_n  25 or 50, smaller means faster or more readings
		/****************************/
		switch(Menu){
			/***MENU 1***/
			case '1': // Main Program Menu
				lcd0.gotoxy(0,4); //TITLE
				lcd0.string_size("Weight Scale", 12); //TITLE
				/*********************************************/
				//lcd0.gotoxy(1,0); // for troubleshooting
				//lcd0.string_size(function.ftoa(hx.read_raw(&hx), result, ZERO), 13);
				/*********************************************/
				if((F.hl(&F) & IMASK) & ONE){ // calibrate offset by pressing button 1
					HX711_data.offset_32 = tmp;
					HX711_data.offset_64 = tmp;
					HX711_data.offset_128 = tmp;
					HX711_data.status = ONE;
					eprom.update_block(HX711_ptr, (void*) ZERO, sizeblock);
					hx.get_cal(&hx)->offset_32 = HX711_ptr->offset_32;
					hx.get_cal(&hx)->offset_64 = HX711_ptr->offset_64;
					hx.get_cal(&hx)->offset_128 = HX711_ptr->offset_128;
					hx.get_cal(&hx)->status=ZERO;
					PORTC &= ~(ONE << 5);
				}
				if(choice == 1 || choice == 11)
					value = (tmp - hx.get_cal(&hx)->offset_128) / hx.get_cal(&hx)->divfactor_128; //value to be published to LCD
				if(choice == 2 || choice == 21)
					value = (tmp - hx.get_cal(&hx)->offset_32) / hx.get_cal(&hx)->divfactor_32; //value to be published to LCD
				if(choice == 3 || choice == 31)
					value = (tmp - hx.get_cal(&hx)->offset_64) / hx.get_cal(&hx)->divfactor_64; //value to be published to LCD
				/*********************************************/
				//lcd0.gotoxy(3,0); // for troubleshooting
				//lcd0.string_size(function.ftoa(tmp, result, ZERO), 13);
				//lcd0.string_size(function.ftoa(hx.get_cal(&hx)->divfactor_128, result, ZERO), 13);
				//lcd0.string_size(function.ftoa(hx.get_cal(&hx)->offset_128, result, ZERO), 13);
				/*********************************************/
				if (value > 1000 || value < -1000){
					publish = value / 1000;
					lcd0.gotoxy(2,1);
					lcd0.string_size(function.ftoa(publish, result, 3), 13); lcd0.string_size("Kg", 4);
				}else{
					publish = value;
					lcd0.gotoxy(2,1);
					lcd0.string_size(function.ftoa(publish, result, ZERO), 13); lcd0.string_size("gram", 4);
				}
				break;
			/***MENU 2***/
			case '2': // MANUAL CALIBRATE DIVFACTOR MENU
				/**/
				lcd0.gotoxy(0,1);
				lcd0.string_size("SETUP GAIN FACTOR",17);
				switch(choice){
					case 1: // case 128
						divfactor=hx.get_cal(&hx)->divfactor_128;
						choice=11;
						break;
					case 11: // case 128
						lcd0.gotoxy(2,9);
						if(F.hl(&F) == (ONE << 3)){
							divfactor++;
							if(divfactor > maxDIV)
								divfactor = maxDIV;
						}
						if(F.hl(&F) == (ONE << 4)){
							divfactor--;
							if(divfactor < minDIV)
								divfactor = minDIV;
						}
						HX711_data.divfactor_128 = divfactor;
						lcd0.string_size(function.ui16toa(divfactor),6);
						break;
					case 2: // case 32
						divfactor=hx.get_cal(&hx)->divfactor_32;
						choice=21;
						break;
					case 21: // case 32
						lcd0.gotoxy(2,9);
						if(F.hl(&F) == (ONE << 3)){
							divfactor++;
							if(divfactor > maxDIV)
								divfactor = maxDIV;
						}
						if(F.hl(&F) == (ONE << 4)){
							divfactor--;
							if(divfactor < minDIV)
								divfactor=minDIV;
						}
						HX711_data.divfactor_32 = divfactor;
						lcd0.string_size(function.ui16toa(divfactor),6);
						break;
					case 3: // case 64
						divfactor=hx.get_cal(&hx)->divfactor_64;
						choice=31;
						break;
					case 31: // case 64
						lcd0.gotoxy(2,9);
						if(F.hl(&F) == (ONE << 3)){
							divfactor++;
							if(divfactor > maxDIV)
								divfactor = maxDIV;
						}
						if(F.hl(&F) == (ONE << 4)){
							divfactor--;
							if(divfactor < minDIV)
								divfactor = minDIV;
						}
						HX711_data.divfactor_64 = divfactor;
						lcd0.string_size(function.ui16toa(divfactor),6);
						break;
					default:
						choice = 3;
						break;
				};
				// Exit and store value
				if((F.ll(&F) & IMASK) == (ONE << 5)){ // Button 6
					HX711_data.status = ONE;
					eprom.update_block(HX711_ptr, (void*) ZERO, sizeblock);
					hx.get_cal(&hx)->divfactor_32=divfactor;
					hx.get_cal(&hx)->divfactor_64=divfactor;
					hx.get_cal(&hx)->divfactor_128=divfactor;
					hx.get_cal(&hx)->status=ZERO;
					PORTC &= ~(ONE << 5); // troubleshooting
					PORTC |= (ONE << 7); // troubleshooting
					counter_2 = ZERO;
					Menu = '1';
					lcd0.clear();
				}
				/**/
				break;
				/********************************************************************/
			default:
				Menu = '1';
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
	PORTF |= IMASK;
	//troubleshooting output
	DDRC = 0xFF;
	PORTC = 0xFF;
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect) // 15.4 us intervals
{
	/***Block other interrupts during this procedure***/
	uint8_t Sreg;
	Sreg = STATUS_REGISTER;
	STATUS_REGISTER &= ~(ONE << GLOBAL_INTERRUPT_ENABLE);
	//hx.query(&hx);	
	hx.read_raw(&hx);
	/***enable interrupts again***/
	STATUS_REGISTER = Sreg;
}
ISR(TIMER1_COMPA_vect) // 1 second intervals
{
	/***CLEAR EEPROM OFFSET SEQUENCE START***/
	if((F.ll(&F) & IMASK) == (ONE << 3)) //button 4
		counter_1++;
	else if(counter_1 < _5sec+ONE)
		counter_1=ZERO;
	if(counter_1 > _5sec){
		counter_1 = _5sec+ONE; //lock in place
		PORTC ^= (ONE << 6); // troubleshooting
		count--;
		if(!count){ //led blinks x times
			// Delete eeprom memory ZERO
			eprom.update_block(HX711_Default, (void*) ZERO, sizeblock);
			hx.get_cal(&hx)->offset_32 = HX711_Default->offset_32;
			hx.get_cal(&hx)->offset_64 = HX711_Default->offset_64;
			hx.get_cal(&hx)->offset_128 = HX711_Default->offset_128;
			hx.get_cal(&hx)->divfactor_32 = divfactor = HX711_Default->divfactor_32;
			hx.get_cal(&hx)->divfactor_64 = divfactor = HX711_Default->divfactor_64;
			hx.get_cal(&hx)->divfactor_128 = HX711_Default->divfactor_128;
			hx.get_cal(&hx)->status = HX711_Default->status;
			PORTC |= (ONE << 5); // troubleshooting
			PORTC |= (ONE << 6); // troubleshooting
			counter_1 = ZERO;
			count=blink;
		}
	}
	/***CLEAR EEPROM OFFSET SEQUENCE END***/
	/***CAL DIVFACTOR DEFINE START***/
	if((F.ll(&F) & IMASK) == (ONE << 4)) //button 5
		counter_2++;
	else if(counter_2 < _5sec+ONE)
		counter_2=ZERO; //RESET TIMER
	if(counter_2 > _5sec){
		counter_2 = ZERO; //RESET TIMER
		signal = ONE;
		PORTC &= ~(ONE << 7); // troubleshooting
	}
	/***CAL DIVFACTOR DEFINE END***/
}
/***EOF***/
/**** Comment:
The past only exists if the present comes to be. There is no future only possibilities.
because 24 bit will have to create a vector pointer of the size of 32 bit, then at the 
end do a cast to *((int32_t*)ptr).

forget about sleep function for HX711 display is always on, then finish the calibration menu of div factor DONE, 
also add functionality for batch counting forget about batch counting already enough level of complexity,

FINISHED LEAVING HAS IS. LATER MAYBE PLAY AROUND AND CLEANUP FOR BEAUTIES SAKE.

MEMCP is not appreciated by this MCU, sometimes crashes.

Endurance test over 2 months, noticed drift during days, but never getting over 10 grams. For a scale using a 50Kg cell is Excellent.
Using offset corrects the drift and precision is concise to the gram.
****/
