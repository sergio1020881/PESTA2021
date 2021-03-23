/************************************************************************
	HX711
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: Atmega 128
Date: 08032021_start
Comment:

************************************************************************/
#ifndef _HX711_H_
	#define _HX711_H_
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/

/***Global Variable***/
//calibration
struct HX711_calibration{
	int32_t offset_32; // ZERO set point A
	int32_t offset_64; // ZERO set point B 64
	int32_t offset_128; // ZERO set point B 128
	uint8_t divfactor_32; // interval A
	uint8_t divfactor_64; // interval B
	uint8_t divfactor_128; // interval B
	uint8_t status;
};
//device
struct hx711{
	uint8_t readflag; // indicate start of bit shifting
	uint8_t trigger; // pickup signal
	uint8_t amplify; // number of end clock cycles
	uint8_t ampcount; // count down final amplify pulses
	uint8_t bitcount; // count down 24 bit
	uint8_t buffer[4]; // reading buffer
	uint8_t bufferindex; // buffer index
	int32_t raw_reading; // reading to be published
	int32_t sum;
	uint8_t av_n;
	float raw_mean;
	struct HX711_calibration cal_data;
	/******/
	void (*set_readflag)(struct hx711* self);
	uint8_t (*check_readflag)(struct hx711* self);
	uint8_t (*read_bit)(void);
	void (*set_amplify)(struct hx711* self, uint8_t amplify);
	int32_t (*read_raw)(struct hx711* self);
	float (*raw_average)(struct hx711* self, uint8_t n);
	struct HX711_calibration* (*get_cal)(struct hx711* self);
	int32_t (*get_offset_32)(struct hx711* self);
	int32_t (*get_offset_64)(struct hx711* self);
	int32_t (*get_offset_128)(struct hx711* self);
	uint8_t (*get_divfactor_32)(struct hx711* self);
	uint8_t (*get_divfactor_64)(struct hx711* self);
	uint8_t (*get_divfactor_128)(struct hx711* self);
};
typedef struct hx711 HX711;
/***Header***/
HX711 HX711enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin);
#endif
/***Comment***
*************/
/***EOF***/
