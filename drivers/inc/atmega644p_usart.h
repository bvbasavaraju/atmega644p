/**
  ******************************************************************************
  * @file    atmega644p_usart.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    21-Oct-2013
  * @brief   This file contains the detailed information about the configuration of USART. Structure and other info
  * @note	 Developer check the data sheet for selecting the appropriate configuration for their requirement!
  ******************************************************************************
  *
  * @Reference	Do check the datasheet for more information on USART and how the clock and Baudrate are derived!
  *
  ******************************************************************************
  */

#ifndef __ATMEGA644P_USART_H				// to avoid the multiple definition!
#define __ATMEGA644P_USART_H

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include "avr/iomxx4.h"
#include "avr/interrupt.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

/* Defines -------------------------------------------------------------------*/
#define USART0	0
#define USART1	1



#define RECEIVE_COMPLETE_FLAG		0x80
#define TRANSMIT_COMPLETE_FLAG		0x40
#define DATA_REGISTER_EMPTY_FLAG	0x20

#define	GLOBAL_INTERRUPT_FLAG		0x80

#define USART0RX_IRQHandler()		ISR(USART0_RX_vect)
#define USART1RX_IRQHandler()		ISR(USART1_RX_vect)

volatile uint8_t	gReceive_Buffer_Full;	//Developer has to make sure to read the buffer once the receive buffer is Full! and reset the flag after reading the buffer

/* Typedefs and structure ----------------------------------------------------*/
typedef enum
{
	NOPARITY 	= 0x00,
	EVENPARITY 	= 0x20,
	ODDPARITY	= 0x30,
}USARTParityBits;

typedef enum
{
	ONESTOPBIT = 0x00,
	TWOSTOPBIT = 0x08,
}USARTStopBits;

typedef enum
{
	FIVE  = 0x00,
	SIX   = 0x01,
	SEVEN = 0x02,
	EIGHT = 0x03,
	NINE  = 0x13,
}USARTDataBits;

typedef enum
{
	SYNCRONOUS  		= 0x01,		// Divider value when Synchronous is selected!
	DOUBLESPEEDASYNC 	= 0x03,		// Divider value when Double speed is selected!
	ASYNCHRONOUS 		= 0x04,		// Divider value when Asynchronous is selected!
}USARTModesType;

typedef enum
{
	TRANSMIT	=	0x08,			// Transmit bit set value of UCSRxB
	RECEIVE		=	0x10,			// Receive bit set value of UCSRxB
	BOTH		=	0x18,			// Both Transmit and receive bits are set value of UCSRxB
}USARTCommunicationType;

typedef struct
{
	uint32_t 				USART_BaudRate;			// Buad rate of the USART TX or RX
	USARTParityBits			USART_Parity;			// Parity Bit required or not!
	USARTStopBits  			USART_StopBits;			// Number of Stop bits 1 or 2!?
	USARTDataBits			USART_DataBits;			// Number of Data bits. it can be from 5 to 9 bits!
	USARTModesType			USART_Modes;			// Mode of USART. Async, Sync or Double speed Async!?
	USARTCommunicationType	USART_Communication;
}USART_StructureType;

/* exported functions ------------------------------------------------------------------*/
void USARTInit(uint8_t, USART_StructureType);
void USART_PutChar(uint16_t);
uint16_t USART_GetChar();
void USART_EnableInterrupt(USARTCommunicationType);
void USART_ClearReceiveBuffer();
void USART_FlushReceiveBuffer();

#endif // end of __ATMEGA644P_USART_H
