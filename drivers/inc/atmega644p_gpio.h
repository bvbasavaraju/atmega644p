/**
  ******************************************************************************
  * @file    atmega644p_gpio.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    09-July-2013
  * @brief   This file contails the PIN layout for atmega664P controller. It
  *			 This file handles all PINs but only used for GPIO.
  * @note	 Developer should take care that, if the pin can handle multiple functions
  *			 like UART or USART or SPI etc.. then those pins cannot be used as the
  *			 GPIOs
  ******************************************************************************
  *
  * @Reference	Do check the I/O ports in the datasheet to know more about GPIO
  * 			for this controller.
  *				Do remember that, AVR has only PULL-UP resister no PULL-DOWN resiter.
  *				So when pin is selected as input pin, make sure that PULL-UP resister
  *				is enabled with that. else Pin will be floating, and the result form
  *				that Pin will be unpridicatable! (you may read that pin is grounded!)
  *
  ******************************************************************************
  */

#ifndef __ATMEGA644P_GPIO_H				// to avoid the multiple definition!
#define __ATMEGA644P_GPIO_H

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include "avr/iomxx4.h"

/* defines	------------------------------------------------------------------*/
#define		GPIO_PIN_RESET		0x00
#define		GPIO_PIN_SET		0x01

/* enums	------------------------------------------------------------------*/
typedef enum
{
	GPIOA = 0,
	GPIOB = 3,
	GPIOC = 6,
	GPIOD = 9
}ports;

typedef enum
{
	READ	= 0,
	CONFIG	= 1,
	WRITE	= 2
}actions;

typedef enum
{
	INPUT 	= 0,
	OUTPUT 	= 1
}modes;

typedef enum
{
	PIN_ZERO 	= 0x01,	//0b 0000 0001
	PIN_ONE 	= 0x02, //0b 0000 0010
	PIN_TWO 	= 0x04,	//0b 0000 0100
	PIN_THREE	= 0x08, //0b 0000 1000
	PIN_FOUR	= 0x10, //0b 0001 0000
	PIN_FIVE	= 0x20,	//0b 0010 0000
	PIN_SIX	    = 0x40,	//0b 0100 0000
	PIN_SEVEN	= 0x80,	//0b 1000 0000
	ALLPINS	    = 0xFF	//0b 1111 1111
}pins;

//typedef struct
//{
//	ports		port;			// the port in the ATMEGA644P
//	modes		mode;			// whether port/pin should be in INPUT or OUTPUT mode
//	actions		action;			// need to read/config/write the port/pin
//	pins		pin_num;		// pin number where value will be written
//}GPIO_Structure;

/* exported functions ------------------------------------------------------------------*/
void GPIO_Write(ports, pins, uint8_t);
uint8_t GPIO_Read(ports, pins);
void GPIO_Config(ports, pins, modes);

#endif // end of __ATMEGA644P_GPIO_H
