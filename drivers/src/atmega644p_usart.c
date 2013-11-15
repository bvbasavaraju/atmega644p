/**
  ******************************************************************************
  * @file    atmega644p_usart.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    21-Oct-2013
  * @brief   This file has the USART configuration and Transmitting and receiving the data via USART
  * @Note	 For Baud rate 0.5 is added so that to get the proper value for UBBR register and the value falls within the error boundry
  * 		 UBBRn = ((Freq / (16 * BaudRate)) + 0.5) - 1	// This is for asynchromous mode!
  * 		       => ((Freq / 16 + baud / 2) / baud - 1)  => So that overflow and underflow has been taken care in code
  * 		 The Receive IRQ handler will fill the receive buffer gReceived_Data[] untill it is fully filled or enter is pressed.
  ******************************************************************************
  *
  *					HOW TO USE
  * 1. Call the initialization function USARTInit() with the USART which is used.
  * 2. If it is interrupt based make sure that USART_EnableInterrupt() is called
  * 3. Call the function USART_PutChar() and/or USART_GetChar() to send or receive data
  * 4. If the Receive buffer is filled (gReceive_Buffer_Full == 1) either empty the buffer or flush the buffer by called the function
  * 	USART_ClearReceiveBuffer() or USART_FlushReceiveBuffer()
  ******************************************************************************
  */

/*----------------------------------- Includes -------------------------------*/
#include "atmega644p_usart.h"

volatile static uint8_t *RegA;
volatile static uint8_t *RegB;
volatile static uint8_t *RegC;
volatile static uint8_t *BRRH;
volatile static uint8_t *BRRL;
volatile static uint8_t *DataR;
volatile static uint8_t *DataT;
//volatile static uint8_t *Data;

/*---------------------------------- Global Variables ----------------------------------*/
static uint16_t gReceived_Data[128];
static uint8_t gIndex = 0;

/*---------------------------------- Function and Hooks ----------------------------------*/

/*
 * @name   	USARTRegInit(uint8_t, USART_StructureType)
 * @brief	This function is to update the Golbal variables with proper Registers!
 * @param  	__USARTType__ - can have the values USART0 or USART1
 *          USARTConfig   - Structure to configure the USARTx
 * @retval	None
 */
void USARTRegInit(uint8_t __USARTType__, USART_StructureType USARTConfig)
{
    if(__USARTType__ != 0)
    {
        RegA = &(UCSR1A);
        RegB = &(UCSR1B);
        RegC = &(UCSR1C);
        BRRH = &(UBRR1H);
        BRRL = &(UBRR1L);

        if(USARTConfig.USART_Communication != BOTH)
        {
            if(USARTConfig.USART_Communication != TRANSMIT)
            {
				DataR = &(UDR1);
			}
            else
            {
				DataT = &(UDR1);
			}
        }
        else
        {
			//Data = &(UDR1);
			DataT = &(UDR1);
			DataR = &(UDR1);
		}
    }

    else
    {
        RegA = &(UCSR0A);
        RegB = &(UCSR0B);
        RegC = &(UCSR0C);
        BRRH = &(UBRR0H);
        BRRL = &(UBRR0L);

        if(USARTConfig.USART_Communication != BOTH)
        {
            if(USARTConfig.USART_Communication != TRANSMIT)
            {
				DataR = &(UDR0);
			}
            else
            {
				DataT = &(UDR0);
			}
        }
        else
        {
			//Data = &(UDR0);
			DataT = &(UDR0);
			DataR = &(UDR0);
		}
    }
}

/*
 * @name   	USARTInit(uint8_t, USART_StructureType)
 * @brief	This function is to configure USARTx based on the given inputs
 * @param  	__USARTType__ - can have the values USART0 or USART1
 * 			USARTConfig   - Structure to configure the USARTx
 * @note	For Parity check Parity check related function should be called
 * @retval	None
 */
void USARTInit(uint8_t __USARTType__, USART_StructureType USARTConfig)
{
	uint16_t BaudRateRegValue; // The register vlaue to be given in UBRRx register!

    gReceive_Buffer_Full = 0;
    USARTRegInit(__USARTType__, USARTConfig);

	//Initialize the USART Control and Status Registers to 0
	*RegA = 0x00;
	*RegB = 0x00;
	*RegC = 0x00;

	//gReceived_Data[127] = '\0';		//Null character at the end of the string!

	//Now with the given value of the structure USART_StructureType configure the USART
	//BaudRateRegValue = (((F_CPU / BaudRateDivider + USARTConfig.USART_BaudRate / 2) / USARTConfig.USART_BaudRate) - 1);
	BaudRateRegValue = ((((F_CPU / (1 << USARTConfig.USART_Modes)) + (USARTConfig.USART_BaudRate / 2)) / USARTConfig.USART_BaudRate) - 1);
	*BRRH = (BaudRateRegValue >> 8) & 0xFF;
	*BRRL = BaudRateRegValue & 0xFF;

	/*Needs to be changed in SPI driver for Master SPI mode*/
	if(USARTConfig.USART_Modes != DOUBLESPEEDASYNC)
	{
		*RegC = *RegC | (USARTConfig.USART_Modes << 6);   // To select the USART mode in UCSRxC register!
	}

	// Based on the type of USART communication enable the Tx or RX or Both bits!
	*RegB = *RegB | USARTConfig.USART_Communication;

	// Based on the number of Databits enable the corresponding Control and status register in Reg C and/Or Reg B bit 0
	*RegC = *RegC | ((USARTConfig.USART_DataBits & 0x0F) << 1);
	*RegB = *RegB | ((USARTConfig.USART_DataBits & 0xF0) >> 4);

	// Configuring the stop bits!
	*RegC = *RegC | USARTConfig.USART_StopBits;

	// Configure the Parity Bits!
	*RegC = *RegC | USARTConfig.USART_Parity;
}

/*
 * @name   	USART_PutChar(uint16_t)
 * @brief	This function is to transmit the charater
 * @param  	data - The data to be transmitted!
 * @retval	None
 */
void USART_PutChar(uint16_t data)
{
	while(!(*RegA & DATA_REGISTER_EMPTY_FLAG))
		; //As the Tansmit buffer is not empty wait until the Transmit buffer is empty then copy the data to data register to transmit!

	//If there are 9 bits to transmit
	if(data & 0x0100)
		*RegB = *RegB | 0x01;
	*DataT = data & 0xFF;
}

/*
 * @name   	USARTInit()
 * @brief	This function is to transmit the charater
 * @param  	None
 * @retval	uint16_t
 */
uint16_t USART_GetChar()
{
	while(!(*RegA & RECEIVE_COMPLETE_FLAG))
		; //As the Receive buffer is empty wait until the receive buffer is filled then return the data from data register!

	//Make sure that 9th bit is also copied while returning the received data!
	return (((*RegB & 0x02) << 7) | (*DataR & 0xFF));
}

/*
 * @name   	USART_EnableInterrupt(USARTCommunicationType)
 * @brief	This function is to enable the interrupt
 * @param  	irq_enable - can have value TRANSMIT, RECEIVE, BOTH
 * @retval	NONE
 */
void USART_EnableInterrupt(USARTCommunicationType irq_enable)
{
	SREG = SREG | GLOBAL_INTERRUPT_FLAG;		//Enable the Global interrupt first

	*RegB = *RegB | (irq_enable << 3);
}

/*
 * @name   	USART0RX_IRQHandler()
 * @brief	This function is a interrupt service routine to handle the USART0 interrupt
 * @param  	NONE
 * @note	In this function the received data will be copied to a buffer gReceived_Data. After 127 data being received the buffer will
 * 			start filling again only after buffer is emptied!
 * 			If the Enter Key is pressed, the gReceive_Buffer_Full flag will be set, Buffer needs to be emptied for new data to be received.
 * @retval	NONE
 */
USART0RX_IRQHandler()
{
	uint16_t ch;
	ch = USART_GetChar();

	if((gIndex < 127) && (ch != 0x0D) && (gReceive_Buffer_Full != 1))
	{
		gReceived_Data[gIndex] = ch;
		gIndex++;
	}
	else
	{
		gReceive_Buffer_Full = 1;
	}
}

/*
 * @name   	USART1RX_IRQHandler()
 * @brief	This function is a interrupt service routine to handle the USART1 interrupt
 * @param  	NONE
 * @note	In this function the received data will be copied to a buffer gReceived_Data. After 127 data being received the buffer will
 * 			start filling again only after buffer is emptied!
 * 			If the Enter Key is pressed, the gReceive_Buffer_Full flag will be set, Buffer needs to be emptied for new data to be received.
 * @retval	NONE
 */
USART1RX_IRQHandler()
{
	uint16_t ch;
	ch = USART_GetChar();

	if((gIndex < 127) && (ch != 0x0D) && (gReceive_Buffer_Full == 0))
	{
		gReceived_Data[gIndex] = ch;
		gIndex++;
	}
	else
	{
		gReceive_Buffer_Full = 1;
	}
}

/*
 * @name   	USART_ClearReceiveBuffer()
 * @brief	This function is to empty the receive buffer so that new data can be read.
 * @param  	None
 * @note	This function has to be called by the developer once the gReceive_Buffer_Full is set!
 * @retval	NONE
 */
void USART_ClearReceiveBuffer()
{
	if(gReceive_Buffer_Full != 0)
	{
		uint8_t i;

		for(i = 0; i<gIndex; i++)
		{
			USART_PutChar(gReceived_Data[i]);
		}

		gReceive_Buffer_Full = 0;	//Once the buffer is emptied the flag and the index should be reset!
		gIndex = 0;
	}
}

/*
 * @name   	USART_FlushReceiveBuffer()
 * @brief	This function is to flush the receive buffer.
 * @param  	None
 * @note	This function can be called when receive buffer needs to be flushed.
 * @retval	NONE
 */
void USART_FlushReceiveBuffer()
{
	gReceive_Buffer_Full = 0;		// reset the receive complete flag and the index for receive buffer!
	gIndex = 0;
}
/*
 * Function to Check for the parity, and Parity bit after the data bits!
 */
