/***************************************************************************************************
	ATMEGA128I2C
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Hardware: ATmega128
Date: 25102020
Comment:
	Stable
 **************************************************************************************************/
#ifndef _ATMEGA128I2C_H
	#define _ATMEGA128I2C_H
/***Library***/
#include <inttypes.h>
/***Global Variable***/
struct twi{
	void (*Start)();
	void (*Stop)(void);
	void (*Write)(uint8_t );
	uint8_t (*Read)(uint8_t);
	uint8_t (*Status)(void);
};
typedef struct twi I2C;
/***Header***/
I2C I2Cenable(uint8_t prescaler);
/******/
#endif
/***EOF***/
