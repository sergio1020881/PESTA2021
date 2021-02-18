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
#define SMIN -200
#define SMAX +200
/*
** Global File variable
*/
struct time tm; // time struct RTC
struct date dt; // date struct RTC
HC595 shift;
UART1 uart;
FUNC function;
PCF8563RTC rtc;
uint8_t count=0; // 1Hz
uint8_t increment=0; // 1Hz
char* ptr=NULL; // pointing to analog reading string
//char* uartreceive=NULL; // pointing to Rx Buffer
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
	rtc = PCF8563RTCenable(16); // RTC with I2C
	shift = HC595enable(&DDRG,&PORTG,2,0,1);
	//uart = UART1enable(103,8,1,NONE);//UART 103 para 9600, 68 para 14400
	ZNPID pid = ZNPIDenable();
	/******/
	char Menu='1'; // Main menu selector
	uint16_t adcvalue; // analog reading
	char str[6]="0"; // analog vector
	int16_t mvalue=90; // manual position reading
	int16_t m_value; // manual positioning
	char mstr[6]="90"; // manual position vector
	char tstr[6]; // time vector
	char cal='0'; // Sub Menu for setting up date and time
	uint16_t set;
	//char uartmessage[64];
	ptr=str;
	uint16_t positionhour=12;
	/***Parameters timers***/
	timer0.compare(249);
	timer0.start(64);
	timer1.compoutmodeB(2);
	timer1.compareA(20000);
	timer1.start(8);
	rtc.SetClkOut(1, 2); // oscillate pin at 1 sec
	pid.set_SP(&pid, 10);
	pid.set_kd(&pid, 6);
	pid.set_ki(&pid, 12);
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
					/***RTC***/
					tm=rtc.GetTime();
					dt=rtc.GetDate();
					/***Reading analog***/
					adcvalue=analog.read(0);
					/***Set Position***/
					if(positionhour>5 && positionhour<21)
						timer1.compareB(function.trimmer(positionhour,2,23,Min,Max)+function.trimmer(adcvalue,0,1023,SMIN,SMAX));
					else
						timer1.compareB(function.trimmer(6,2,23,Min,Max));
					lcd0.gotoxy(0,0);
					lcd0.string_size("Sense: ",7);
					//lcd0.hspace(1);
					strcpy(str,function.i16toa(adcvalue));
					lcd0.string_size(str,4);
					lcd0.gotoxy(0,12);
					lcd0.string_size(function.ui16toa(rtc.bcd2dec(dt.days)),2);
					lcd0.putch(':');
					lcd0.string_size(function.ui16toa(rtc.bcd2dec(dt.century_months)),2);
					lcd0.putch(':');
					lcd0.string_size(function.ui16toa(rtc.bcd2dec(dt.years)),2);
					lcd0.gotoxy(1,12);
					/***set hour for positioning***/
					positionhour=rtc.bcd2dec(tm.hours);
					lcd0.string_size(function.ui16toa(positionhour),2);
					lcd0.putch(':');
					lcd0.string_size(function.ui16toa(rtc.bcd2dec(tm.minutes)),2);
					lcd0.putch(':');
					lcd0.string_size(function.ui16toa(rtc.bcd2dec(tm.VL_seconds)),2);
					/***Message from uart***/
					lcd0.gotoxy(2,0);
					//strcpy(uartmessage,uartreceive);
					//if(uartreceive[0]!='\0'){lcd0.string_size("u> ",3);lcd0.string_size(uartmessage,17);}
					//if(!strcmp(uartreceive,"position\r")){
					//if(!strcmp(uartreceive,"position")){
						//uart.puts("> ");
						//uart.puts("analog Reading: ");
						//uart.puts(ptr);
						//uart.puts("\r\n");
						//uart.Rxflush();
					//}
					//if(!strcmp(uartreceive,"time\r")){
					//if(!strcmp(uartreceive,"time")){
						//uart.putc('>');uart.puts("analog Reading: ");uart.puts(ptr);uart.puts("\r\n");
						//uart.puts(function.ui16toa(rtc.bcd2dec(tm.hours)));
						//uart.putc(':');
						//uart.puts(function.ui16toa(rtc.bcd2dec(tm.minutes)));
						//uart.putc(':');
						//uart.puts(function.ui16toa(rtc.bcd2dec(tm.VL_seconds)));
						//uart.puts("\r\n");
						//uart.Rxflush();
					//}
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
				if(!strcmp(keypad.get().string,"C")){Menu='1';cal='0';keypad.flush();lcd0.clear();break;}
					/*** Menu to set RTC Time and Date ***/
					lcd0.gotoxy(0,0);
					lcd0.string_size("Date and Time Setup",19);
					/***Calibrate Menu***/
					switch(cal){
						case '0': // choice
							lcd0.gotoxy(1,0);
							lcd0.string_size("1-Year",7);
							lcd0.string_size("2-Month",8);
							lcd0.string_size("3-Day",5);
							lcd0.gotoxy(2,0);
							lcd0.string_size("4-Hour",7);
							lcd0.string_size("5-Min",8);
							lcd0.string_size("6-Sec",5);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							if(!strcmp(keypad.get().string,"1")){cal='1';keypad.flush();lcd0.clear();}
							if(!strcmp(keypad.get().string,"2")){cal='2';keypad.flush();lcd0.clear();}
							if(!strcmp(keypad.get().string,"3")){cal='3';keypad.flush();lcd0.clear();}
							if(!strcmp(keypad.get().string,"4")){cal='4';keypad.flush();lcd0.clear();}
							if(!strcmp(keypad.get().string,"5")){cal='5';keypad.flush();lcd0.clear();}
							if(!strcmp(keypad.get().string,"6")){cal='6';keypad.flush();lcd0.clear();}
							//if(keypad.get().character=='1'){cal='1';keypad.flush();lcd0.clear();}
							break;
						/********************************************************************/
						case '1': // YEAR
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Year:",11);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);	
							/***YEAR***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <100){
									rtc.SetYear(rtc.bintobcd(set));
									cal='0';
									}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
						break;
						/********************************************************************/
						case '2': // MONTH
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Month:",12);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							/***MONTH***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <13){
									rtc.SetMonth(rtc.bintobcd(set));
									cal='0';
								}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
							break;
						/********************************************************************/
						case '3': // DAY
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Day:",10);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							/***DAY***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <32){
									rtc.SetDay(rtc.bintobcd(set));
									cal='0';
								}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
						break;
						/********************************************************************/
						case '4': // HOUR
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Hour:",11);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							/***HOUR***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <24){
									rtc.SetHour(rtc.bintobcd(set));
									cal='0';
								}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
						break;
						/********************************************************************/
						case '5': // MINUTE
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Minutes:",13);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							/***MINUTE***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <60){
									rtc.SetMinute(rtc.bintobcd(set));
									cal='0';
								}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
						break;
						/********************************************************************/
						case '6': // SECOND
							lcd0.gotoxy(1,0);
							lcd0.string_size("Enter Seconds:",13);
							lcd0.gotoxy(3,0);
							lcd0.string_size("C - exit",8);
							/***SECOND***/
							if(keypad.get().character==KEYPADENTERKEY){
								strcpy(tstr,keypad.get().string);
								set=function.strToInt(tstr);
								if(set >=0 && set <60){
									rtc.SetSecond(rtc.bintobcd(set));
									cal='0';
								}else{
									strcpy(tstr,"err");
								}
								keypad.flush();
							}
						break;
						default:
							cal='0';
						break;
					};
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
To not work with the hardware directly but just by code is the primordial objective.
****/
