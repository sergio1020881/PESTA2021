/*************************************************************************
	ATMEGA128SPI
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
Hardware: ATmega128
Date: 25102020
Comment:
   Stable
*************************************************************************/
/***Preamble Inic***/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include "atmega128spi.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
    #define GLOBAL_INTERRUPT_ENABLE 7
#endif
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#define ATMEGA_SPI
	#define SPI_CONTROL_REGISTER SPCR
	#define SPI_STATUS_REGISTER SPSR
	#define SPI_DATA_REGISTER SPDR
	#define SPI_PORT PORTB
	#define SPI_DDR DDRB
	#define SPI_PIN PINB
	#define SPI_PIN_MASK 15
	#define DD_SS 0
	#define DD_SCK 1
	#define DD_MOSI 2
	#define DD_MISO 3
#else
	#error "Not Atmega 128"
#endif
/***Global File Variables***/
/***Header***/
void spi_default(void);
void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len);
void spi_transmit_sync (uint8_t * dataout, uint8_t len);
uint8_t spi_fast_shift (uint8_t data);
/***Procedure & function***/
SPI SPIenable(uint8_t master_slave_select, uint8_t data_order,  uint8_t data_modes, uint8_t prescaler)
{
	SPI spi;
	spi.transfer_sync = spi_transfer_sync;
	spi.transmit_sync = spi_transmit_sync;
	spi.fast_shift = spi_fast_shift;
	/***/
	SPI_DDR &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
	switch(master_slave_select){
		case SPI_MASTER_MODE:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_SS) | (1<<DD_MOSI) | (1<<DD_SCK);
			SPI_PORT |= (1<<DD_SS);
			break;
		case SPI_SLAVE_MODE:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_MISO);
			break;
		default:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_SS) | (1<<DD_MOSI) | (1<<DD_SCK);
			break;
	}
	switch(data_order){
		case SPI_LSB_DATA_ORDER:
			SPI_CONTROL_REGISTER |= (1<<DORD);
			break;
		case SPI_MSB_DATA_ORDER:
			SPI_CONTROL_REGISTER &= ~(1<<DORD);
			break;
		default:
			SPI_CONTROL_REGISTER &= ~(1<<DORD);
			break;
	}
	switch(data_modes){
		case 0:
			SPI_CONTROL_REGISTER &= ~((1<<CPOL) | (1<<CPHA));
			break;
		case 1:
			SPI_CONTROL_REGISTER |= (1<<CPHA);
			break;
		case 2:
			SPI_CONTROL_REGISTER |= (1<<CPOL);
			break;
		case 3:
			SPI_CONTROL_REGISTER |= (1<<CPOL) | (1<<CPHA);
			break;
		default:
			SPI_CONTROL_REGISTER &= ~((1<<CPOL) | (1<<CPHA));
			break;
	}
	switch(prescaler){
		case 2:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER &= ~((1<<SPR1) | (1<<SPR0));
			break;
		case 4:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER &= ~((1<<SPR1) | (1<<SPR0));
			break;
		case 8:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
		case 16:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
		case 32:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1);
			break;
		case 64:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1);
			break;
		case 128:
			SPI_STATUS_REGISTER &= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1) | (1<<SPR0);
			break;
		default:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
	}
	SPI_CONTROL_REGISTER |= (1<<SPE);
	return spi;
}
void spi_default()
// Initialize pins for spi communication
{
    SPI_DDR &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    // Define the following pins as output
    SPI_DDR |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK)); 
    SPI_CONTROL_REGISTER =	((1<<SPE)|              // SPI Enable
							(0<<SPIE)|              // SPI Interrupt Enable
							(0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
							(1<<MSTR)|              // Master/Slave select   
							(0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
							(0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
							(0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)
    SPI_STATUS_REGISTER =	(1<<SPI2X);             // Double Clock Rate  
}
void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPI_DATA_REGISTER = dataout[i];
             while((SPI_STATUS_REGISTER & (1<<SPIF))==0); // polling, serial transfer is complete interrupt.
             datain[i] = SPI_DATA_REGISTER;
       }
}
void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPI_DATA_REGISTER = dataout[i];
             while((SPI_STATUS_REGISTER & (1<<SPIF))==0); // polling, serial transfer is complete interrupt.
       }
}
uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
    SPI_DATA_REGISTER = data;
    while((SPI_STATUS_REGISTER & (1<<SPIF))==0)
		; // polling, serial transfer is complete interrupt.
    return SPI_DATA_REGISTER;
}
/***Interrupt***/
ISR(SPI_STC_vect){ }
/***EOF***/
