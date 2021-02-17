/***************************************************************************************************
	PCF8563RTC
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Hardware: PCF8563
Date: 25102020
Comment:
	Stable
 **************************************************************************************************/
#ifndef _PCF8563RTC_H_
	#define _PCF8563RTC_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
#define PCF8563ReadMode_U8   0xA3  // PCF8563 ID
#define PCF8563WriteMode_U8  0xA2  // PCF8563 ID
#define PCF8563SecondRegAddress_U8   0x02    // Address to access PC8563 SEC register
#define PCF8563MinuteRegAddress_U8   0x03    // Address to access PC8563 MINUTE register
#define PCF8563HourRegAddress_U8     0x04    // Address to access PC8563 HOUR register
#define PCF8563DayRegAddress_U8      0x05    // Address to access PC8563 DAY register
#define PCF8563WeekdayRegAddress_U8  0x06    // Address to access PC8563 WEEKDAY register
#define PCF8563MonthRegAddress_U8    0x07    // Address to access PC8563 MONTH register
#define PCF8563YearRegAddress_U8     0x08    // Address to access PC8563 YEAR register
#define PCF8563ControlRegAddress_U8  0x00    // Address to access PC8563 CONTROL register
#define PCF8563CLKOUT_control_U8     0x0D	 // External oscillating pin
/***Global Variable***/
struct date{
	uint8_t years;
	uint8_t century_months;
	uint8_t weekdays;
	uint8_t days;	
};
struct time{
	uint8_t hours;
	uint8_t minutes;
	uint8_t VL_seconds;
};
struct alarm{
	uint8_t minute_alarm;
	uint8_t	hour_alarm;
	uint8_t day_alarm;
	uint8_t weekday_alarm;
	
};
struct pcfrtc{
	void (*SetTime)(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8);
	void (*SetHour)(uint8_t var_hour_u8);
	void (*SetMinute)(uint8_t var_min_u8);
	void (*SetSecond)(uint8_t var_sec_u8);
	void (*SetClkOut)(uint8_t onoff, uint8_t freq);
	void (*SetDate)(uint8_t var_day_u8, uint8_t var_weekday_u8, uint8_t var_month_u8, uint8_t var_year_u8);
	void (*SetDay)(uint8_t var_day_u8);
	void (*SetWeekday)(uint8_t var_weekday_u8);
	void (*SetMonth)(uint8_t var_month_u8);
	void (*SetYear)(uint8_t var_year_u8);
	struct time (*GetTime)(void);
	struct date (*GetDate)(void);
	uint8_t (*bcd2dec)(uint8_t num);
	uint8_t (*bintobcd)(uint8_t bin);
};
typedef struct pcfrtc PCF8563RTC;
/***Header***/
PCF8563RTC PCF8563RTCenable(uint8_t prescaler);
#endif
/***EOF***/
