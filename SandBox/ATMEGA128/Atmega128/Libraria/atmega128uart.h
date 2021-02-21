/************************************************************************
	ATMEGA128UART
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
Hardware: ATmega128
Date: 25102020
Comment:
	Stable
************************************************************************/
/***Preamble Inic***/
#ifndef _ATMEGA128UART_H_
	#define _ATMEGA128UART_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/***Library***/
#include <inttypes.h>
/***Constant & Macro***/
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)
// Size of the circular buffer, must be power of 2
#ifndef UART_RX_BUFFER_SIZE
	#define UART_RX_BUFFER_SIZE 64
#endif
#ifndef UART_TX_BUFFER_SIZE
	#define UART_TX_BUFFER_SIZE 64
#endif
// test if the size of the circular buffers fits into SRAM
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
	#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif
#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */
/***Parity choices***/
#define NONE 0
#define EVEN 2
#define ODD 3
/***Global Variable***/
struct rt{
	/***Parameters***/
	uint8_t ubrr;
	uint8_t FDbits;
	uint8_t Stopbits;
	uint8_t Parity;
	// prototype pointers
	char* (*read)(void);
	uint8_t (*getc)(void);
	void (*putc)(const char data);
	void (*puts)(const char *s );
	int8_t (*available)(void);
	void (*flush)(void);
};
typedef struct rt UART;
struct rt1{
	/***Parameters***/
	uint8_t ubrr;
	uint8_t FDbits;
	uint8_t Stopbits;
	uint8_t Parity;
	//prototype pointers
	char* (*read)(void);
	uint8_t (*getc)(void);
	void (*putc)(const char data);
	void (*puts)(const char *s );
	int8_t (*Rxavailable)(void);
	void (*Rxflush)(void);
	void (*Txflush)(void);
};
typedef struct rt1 UART1;
/***Header***/
UART UARTenable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity );
UART1 UART1enable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity );
#endif
/***EOF***/
