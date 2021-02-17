/************************************************************************
	KEYPAD
Author: Sergio Santos 
   <sergio.salazar.santos@gmail.com>
Hardware: all
Date: 29092020
Comment:
   Stable
************************************************************************/
/***Preamble Inic***/
#ifndef _KEYPAD_H_
	#define _KEYPAD_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
#define KEYPADLINE_1 7
#define KEYPADLINE_2 0
#define KEYPADLINE_3 1
#define KEYPADLINE_4 3
#define KEYPADDATA_1 2
#define KEYPADDATA_2 4
#define KEYPADDATA_3 5
#define KEYPADDATA_4 6
#define KEYPADSTRINGSIZE 20
#define KEYPADENTERKEY 'D'
/***Gloabl Variable***/
struct keypadata{
	char character;
	char* printstring;
	char* string;
};
/******/
struct keypad{
	//Local Variables
	//Function Pointers
	char (*getkey)(void);
	struct keypadata (*read)(void);
	struct keypadata (*get)(void);
	void (*flush)(void);
};
typedef struct keypad KEYPAD;
/***Header***/
KEYPAD KEYPADenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
#endif
/************************************************************************
The matrix buttons should have a diode in series so each button would only let current flow in one direction not allowing
feedbacks. Little defect of keypads !
Simply Magic.
************************************************************************/
/***EOF***/
