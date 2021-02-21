/***************************************************************************************************
	ATMEGA128I2C
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Hardware: ATmega128
Date: 25102020
Comment:
	Stable
***************************************************************************************************/
#ifndef F_CPU
	//Mandatory to use util/delay
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include "atmega128i2c.h"
/***Contant & Macros***/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	#define ATMEGA_I2C
	#define I2C_DDR DDRD
	#define I2C_PORT PORTD
	#define I2C_IO_MASK 0x03
	#define I2C_SCL_CLOCK 100000UL
	#define TWI_BIT_RATE_REGISTER TWBR
	#define TWI_CONTROL_REGISTER TWCR
	#define TWI_STATUS_REGISTER TWSR
	#define TWI_STATUS_MASK 0B11111000
	#define TWI_DATA_REGISTER TWDR
	#define TWI_ADDRESS_REGISTER TWAR
	#define TWI_ADDRESS_REGISTER_MASK 0B11111110
	#define TWI_PRESCALER_MASK 0B00000011
	#define GLOBAL_INTERRUPT_ENABLE 7
#else
	#error "Not Atmega 128"
#endif
#define Nticks 1023 //anti polling freeze.
/***Global File Variable***/
uint16_t ticks;
/***Header***/
void I2C_Init(uint8_t prescaler);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t var_i2cData_u8);
uint8_t I2C_Read(uint8_t var_ackOption_u8);
uint8_t I2C_Status(void);
/***I2C I2Cenable(uint8_t prescaler)***/
I2C I2Cenable(uint8_t prescaler)
{
  uint8_t tSREG;
  tSREG=SREG;
  SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
  I2C ic;
  I2C_Init(prescaler);
  /***Vtable***/
  ic.Start=I2C_Start;
  ic.Stop=I2C_Stop;
  ic.Write=I2C_Write;
  ic.Read=I2C_Read;
  ic.Status=I2C_Status;
  /******/
  SREG=tSREG;
  return ic;
}
/***void I2C_Init(uint8_t prescaler)***/
void I2C_Init(uint8_t prescaler)
{
	I2C_DDR|=I2C_IO_MASK;
	I2C_PORT|=I2C_IO_MASK;
	switch(prescaler){
		case 1:
		TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
		break;
		case 4:
		TWI_STATUS_REGISTER |= (1<<TWPS0);
		break;
		case 16:
		TWI_STATUS_REGISTER |= (2<<TWPS0);
		break;
		case 64:
		TWI_STATUS_REGISTER |= (3<<TWPS0);
		break;
		default:
		prescaler=1;
		TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
		break;
	}
	TWI_BIT_RATE_REGISTER = ((F_CPU/I2C_SCL_CLOCK)-16)/(2*prescaler);
	/***Standard Config begin***/
	//TWI_STATUS_REGISTER=0x00; //set presca1er bits to zero
	//TWI_BIT_RATE_REGISTER=0x46; //SCL frequency is 50K for 16Mhz
	//TWI_CONTROL_REGISTER=0x04; //enab1e TWI module
	/***Standard Config end***/
	/***Vtable***/
}
/***void I2C_Start(void)***/
void I2C_Start(void)
{
  TWI_CONTROL_REGISTER = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
  for (ticks=Nticks; !(TWI_CONTROL_REGISTER & (1<<TWINT)) && ticks; ticks--);
}
/***void I2C_Stop(void)***/
void I2C_Stop(void)
{
  TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
   _delay_us(100); //wait for a short time
}
/***void I2C_Write(uint8_t var_i2cData_u8)***/
void I2C_Write(uint8_t var_i2cData_u8)
{
  TWI_DATA_REGISTER = var_i2cData_u8 ;
  TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN));
  for (ticks=Nticks; !(TWI_CONTROL_REGISTER & (1 <<TWINT)) && ticks; ticks--);
}
/***uint8_t I2C_Read(uint8_t var_ackOption_u8)***/
uint8_t I2C_Read(uint8_t var_ackOption_u8)
{
 TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN) | (var_ackOption_u8<<TWEA));
   for (ticks=Nticks; !(TWI_CONTROL_REGISTER & (1 <<TWINT)) && ticks; ticks--);
   return TWI_DATA_REGISTER;
}
/***uint8_t I2C_status(void)***/
uint8_t I2C_Status(void)
{
	return TWI_STATUS_REGISTER & TWI_STATUS_MASK;
}
/***EOF***/
