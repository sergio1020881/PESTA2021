/***************************************************************************************************
	PCF8563RTC
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Hardware: PCF8563
Date: 25102020
Comment:
	Stable
 **************************************************************************************************/
/***Library***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "pcf8563rtc.h"
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#include "atmega128i2c.h"
#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	#include "atmega328i2c.h"
#else
	#include "i2c.h"
#endif
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/***Global File Variables***/
I2C i2c;
/***Header***/
void PCF8563RTC_Init(void);
void PCF8563RTC_SetTime(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8);
void PCF8563RTC_SetHour(uint8_t var_hour_u8);
void PCF8563RTC_SetMinute(uint8_t var_min_u8);
void PCF8563RTC_SetSecond(uint8_t var_sec_u8);
void PCF8563RTC_SetClkOut(uint8_t onoff, uint8_t freq);
void PCF8563RTC_SetDate(uint8_t var_day_u8, uint8_t var_weekday_u8, uint8_t var_month_u8, uint8_t var_year_u8);
void PCF8563RTC_SetDay(uint8_t var_day_u8);
void PCF8563RTC_SetWeekday(uint8_t var_weekday_u8);
void PCF8563RTC_SetMonth(uint8_t var_month_u8);
void PCF8563RTC_SetYear(uint8_t var_year_u8);
struct time PCF8563RTC_GetTime(void);
struct date PCF8563RTC_GetDate(void);
uint8_t PCF8563RTC_bcd2dec(uint8_t num);
uint8_t PCF8563RTC_bintobcd(uint8_t bin);
/***PCF8563RTC PCF8563RTCenable(uint8_t prescaler)***/
PCF8563RTC PCF8563RTCenable(uint8_t prescaler)
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	PCF8563RTC pcf;
	i2c = I2Cenable(prescaler);  			// Initialize the I2c module.
	PCF8563RTC_Init();                      //Initialize RTC
	/***Vtable***/
	pcf.SetTime=PCF8563RTC_SetTime;
	pcf.SetHour=PCF8563RTC_SetHour;
	pcf.SetMinute=PCF8563RTC_SetMinute;
	pcf.SetSecond=PCF8563RTC_SetSecond;
	pcf.SetClkOut=PCF8563RTC_SetClkOut;
	pcf.SetDate=PCF8563RTC_SetDate;
	pcf.SetDay=PCF8563RTC_SetDay;
	pcf.SetWeekday=PCF8563RTC_SetWeekday;
	pcf.SetMonth=PCF8563RTC_SetMonth;
	pcf.SetYear=PCF8563RTC_SetYear;
	pcf.GetTime=PCF8563RTC_GetTime;
	pcf.GetDate=PCF8563RTC_GetDate;
	pcf.bcd2dec=PCF8563RTC_bcd2dec;
	pcf.bintobcd=PCF8563RTC_bintobcd;
	/******/
	SREG=tSREG;
	return pcf;
}
/***void PCF8563RTC_Init(uint8_t prescaler)***/
void PCF8563RTC_Init(void)
{
	i2c.Start();							// Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);			// Connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563ControlRegAddress_U8);	// Select the PCF8563 ControlRegister to configure PCF8563
	i2c.Write(0x00);						// Write 0x00 to Control register to disable SQW-Out
	i2c.Stop();								// Stop I2C communication after initializing PCF8563
}
/***void PCF8563RTC_SetTime(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8)***/
void PCF8563RTC_SetTime(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8)
{
	i2c.Start();                            // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);         // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563SecondRegAddress_U8);  // Select the SEC RAM address
	i2c.Write(var_sec_u8);			        // Write sec from RAM address 02H
	i2c.Write(var_min_u8);			        // Write min from RAM address 03H
	i2c.Write(var_hour_u8);			        // Write hour from RAM address 04H
	i2c.Stop();           	                // Stop I2C communication after Setting the Time
}
/***void PCF8563RTC_SetHour(uint8_t var_hour_u8)***/
void PCF8563RTC_SetHour(uint8_t var_hour_u8)
{
	i2c.Start();                            // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);         // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563HourRegAddress_U8);    // Select the SEC RAM address
	i2c.Write(var_hour_u8);			        // Write hour from RAM address 04H
	i2c.Stop();           	                // Stop I2C communication after Setting the Time
}
/***void PCF8563RTC_SetMinute(uint8_t var_min_u8)***/
void PCF8563RTC_SetMinute(uint8_t var_min_u8)
{
	i2c.Start();                            // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);         // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563MinuteRegAddress_U8);  // Select the SEC RAM address
	i2c.Write(var_min_u8);			        // Write min from RAM address 03H
	i2c.Stop();           	                // Stop I2C communication after Setting the Time
}
/***void PCF8563RTC_SetSecond(uint8_t var_sec_u8)***/
void PCF8563RTC_SetSecond(uint8_t var_sec_u8)
{
	i2c.Start();                            // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);         // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563SecondRegAddress_U8);  // Select the SEC RAM address
	i2c.Write(var_sec_u8);			        // Write sec from RAM address 02H
	i2c.Stop();           	                // Stop I2C communication after Setting the Time
}
/***void PCF8563RTC_SetClkOut(uint8_t onoff, uint8_t freq)***/
void PCF8563RTC_SetClkOut(uint8_t onoff, uint8_t freq)
{
	uint8_t word=0;
	if(onoff)
		word=0x80 | (freq & 0x03);
	i2c.Start();                            // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);			// connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563CLKOUT_control_U8);	// Select the CLKOUT RAM address
	i2c.Write(word);						// Write Clock from RAM address 0DH						
	i2c.Stop();           	                // Stop I2C communication after Setting the Time
}
/***void PCF8563RTC_SetDate(uint8_t var_day_u8, uint8_t var_weekday_u8, uint8_t var_month_u8, uint8_t var_year_u8)***/
void PCF8563RTC_SetDate(uint8_t var_day_u8, uint8_t var_weekday_u8, uint8_t var_month_u8, uint8_t var_year_u8)
{
	i2c.Start();                          // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);	      // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563DayRegAddress_U8);  // Request DAY RAM address at 05H
	i2c.Write(var_day_u8);			      // Write date on RAM address 05H
	i2c.Write(var_weekday_u8);            // Write date on RAM address 06H
	i2c.Write(var_month_u8);			  // Write month on RAM address 07H
	i2c.Write(var_year_u8);			      // Write year on RAM address 08h
	i2c.Stop();				              // Stop I2C communication after Setting the Date
}
/***void PCF8563RTC_SetYear(uint8_t var_year_u8)***/
void PCF8563RTC_SetYear(uint8_t var_year_u8)
{
	i2c.Start();                          // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);	      // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563YearRegAddress_U8);  // Request DAY RAM address at 08H
	i2c.Write(var_year_u8);			      // Write year on RAM address 08h
	i2c.Stop();				              // Stop I2C communication after Setting the Date
}
/***void PCF8563RTC_SetMonth(uint8_t var_month_u8)***/
void PCF8563RTC_SetMonth(uint8_t var_month_u8)
{
	i2c.Start();                           // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);	       // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563MonthRegAddress_U8);  // Request DAY RAM address at 07H
	i2c.Write(var_month_u8);			   // Write month on RAM address 07H
	i2c.Stop();				               // Stop I2C communication after Setting the Date
}
/***void PCF8563RTC_SetWeekday(uint8_t var_weekday_u8)***/
void PCF8563RTC_SetWeekday(uint8_t var_weekday_u8)
{
	i2c.Start();                             // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);	         // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563WeekdayRegAddress_U8);  // Request DAY RAM address at 06H
	i2c.Write(var_weekday_u8);               // Write date on RAM address 06H
	i2c.Stop();				                 // Stop I2C communication after Setting the Date
}
/***void PCF8563RTC_SetDay(uint8_t var_day_u8)***/
void PCF8563RTC_SetDay(uint8_t var_day_u8)
{
	i2c.Start();                          // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);	      // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563DayRegAddress_U8);  // Request DAY RAM address at 05H
	i2c.Write(var_day_u8);			      // Write date on RAM address 05H
	i2c.Stop();				              // Stop I2C communication after Setting the Date
}
/***struct time PCF8563RTC_GetTime(void)***/
struct time PCF8563RTC_GetTime(void)
{
	struct time result;
	i2c.Start();							       // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);			       // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563SecondRegAddress_U8);	       // Request Sec RAM address at 00H
	i2c.Stop();								       // Stop I2C communication after selecting Sec Register
	i2c.Start();							       // Start I2C communication
	i2c.Write(PCF8563ReadMode_U8);			       // connect to PCF8563(Read mode) by sending its ID
	result.VL_seconds = i2c.Read(1) & ~0x80;	   // read second and return Positive ACK
	result.minutes = i2c.Read(1) & ~0x80;		   // read minute and return Positive ACK
	result.hours = i2c.Read(0) & ~0xC0;			   // read hour and return Negative/No ACK
	i2c.Stop();								       // Stop I2C communication after reading the Time
	return result;
}
/***struct date PCF8563RTC_GetDate(void)***/
struct date PCF8563RTC_GetDate(void)
{
	struct date result;
	i2c.Start();							        // Start I2C communication
	i2c.Write(PCF8563WriteMode_U8);			        // connect to PCF8563 by sending its ID on I2c Bus
	i2c.Write(PCF8563DayRegAddress_U8);	            // Request DAY RAM address at 04H
	i2c.Stop();								        // Stop I2C communication after selecting DAY Register
	i2c.Start();							        // Start I2C communication
	i2c.Write(PCF8563ReadMode_U8);			        // connect to PCF8563 (Read mode) by sending its ID
	result.days = i2c.Read(1) & ~0xC0;				// read Day and return Positive ACK
	result.weekdays = i2c.Read(1) & ~0xF8;			// read Month and return Positive ACK
	result.century_months = i2c.Read(1) & ~0xE0;
	result.years = i2c.Read(0);				        // read Year and return Negative/No ACK
	i2c.Stop();								        // Stop I2C communication after reading the Date
	return result;
}
/***uint8_t PCF8563RTC_bcd2dec(uint8_t num)***/
uint8_t PCF8563RTC_bcd2dec(uint8_t num)
{
	return ((num/16 * 10) + (num % 16));
}
/***uint8_t PCF8563RTC_bintobcd(uint8_t bin)***/
uint8_t PCF8563RTC_bintobcd(uint8_t bin)
{
	return (((bin) / 10) << 4) + ((bin) % 10);
}
/***EOF***/
