/**
  ******************************************************************************
  * @file    atmega644p_i2c.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    08-Oct-2013
  * @brief   This file contains the detailed information about the configuration of I2C or TWI. Structure and other info
  * @note	 Developer check the data sheet for selecting the appropriate configuration for their requirement!
  ******************************************************************************
  *
  * @Reference	Do check the datasheet for more information on I2C and equations related to SCL, Bitrates and Prescalar!
  *
  ******************************************************************************
  */

#ifndef __ATMEGA644P_I2C_H				// to avoid the multiple definition!
#define __ATMEGA644P_I2C_H

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include "avr/iomxx4.h"
#include "avr/interrupt.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "printf_code.h"

/* Defines -------------------------------------------------------------------*/
//Master Common States:
#define MASTER_START_SENT									0x08
#define MASTER_REPEATED_START_SENT							0x10
#define	MASTER_ARBITRATION_LOST								0x38	//Arbitration Lost
//Master Transmit States:
#define MASTER_TRANSMIT_ADDRESS_POSITIVE_ACK				0x18
#define	MASTER_TRANSMIT_ADDRESS_NEGATIVE_ACK				0x20
#define	MASTER_TRANSMIT_DATA_POSITIVE_ACK					0x28	//received positive ack
#define	MASTER_TRANSMIT_DATA_NEGATIVE_ACK					0x30	//received negative ack
//Master Receive States!
#define MASTER_RECEIVE_ADDRESS_POSITIVE_ACK					0x40
#define	MASTER_RECEIVE_ADDRESS_NEGATIVE_ACK					0x48
#define MASTER_RECEIVE_DATA_POSITIVE_ACK					0x50	//sent positive ack
#define MASTER_RECEIVE_DATA_NEGATIVE_ACK					0x58	//sent negetive ack
//Slave Receive State
#define SLAVE_RECEIVE_ADDRESS_POSITIVE_ACK					0x60	//Addressed as Slave Receive adn ACK returned
#define SLAVE_RECEIVE_GENERAL_CALL_POSITIVE_ACK				0x70	//General Call received and ACK returned
#define SLAVE_RECEIVE_DATA_POSITIVE_ACK						0x80
#define SLAVE_RECEIVE_DATA_NEGATIVE_ACK						0x88
#define SLAVE_RECEIVE_GENERAL_CALL_DATA_POSITIVE_ACK		0x90
#define SLAVE_RECEIVE_GENERAL_CALL_DATA_NEGATIVE_ACK		0x98
#define	SLAVE_STOP_OR_REPEATED_START						0xA0
//Slate Transmit State
#define SLAVE_TRANSMIT_ADDRESS_POSITIVE_ACK					0xA8
#define SLAVE_TRANSMIT_DATA_POSITIVE_ACK					0xB8	//Data transmitted and received positive acknowledgement
#define SLAVE_TRANSMIT_DATA_NEGATIVE_ACK					0xC0	//Data transmitted and received negative acknowledgement
#define SLAVE_TRANSMIT_LAST_DATA_BYTE_POSITIVE_ACK			0xC8	//stop received from Master
//states when Master looses its arbitration
#define	ARBITRATION_LOST_ADDRESSED_SLAVE_RECEIVE			0x68	//Arbitration Lost, addressed as slave receive and ACK returned
#define	ARBITRATION_LOST_GENERAL_CALL_SLAVE_RECEIVE			0x78	//Arbitration Lost, addressed by general call and ACK returned
#define	ARBITRATION_LOST_ADDRESSED_SLAVE_TRANSMIT			0xB0	//Arbitration Lost, addressed as slave transmit and ACK returned
//Misc States
#define NO_INFORMATION_AVAILABLE							0xF8
#define ILLEGAL_START_OR_STOP_CONDITION						0x00	//Bus error due to illegal start/stop


//Interrupt related
#define	GLOBAL_INTERRUPT_FLAG_ENABLE	0x80
#define I2C_IRQHandler()				ISR(TWI_vect)	//ISR

#define BUFFER_SIZE					128
#define TOTAL_POSSIBLE_DEVICES		128

#define DISABLE		0x00
#define ENABLE		0x01

/* Typedefs and structure ----------------------------------------------------*/
typedef enum
{
	eSLAVE_MODE				= 0x00,		//No need to trigger the start condition!
	eMASTER_RECEIVE_MODE	= 0x01,		//LSB&0x01 is the R/Wbar bit!
	eMASTER_TRANMIT_MODE 	= 0x02,		//LSB&0x01 is the R/Wbar bit!
}I2CModesOfOperation;

typedef enum
{
	eNO_PRESCALAR		= 0x00,		//Recommended!
	ePRESCALAR1			= 0x01,
	ePRESCALAR2			= 0x02,
	ePRESCALAR3			= 0x03,
}I2CPrescalarValues;

typedef struct
{
	uint8_t					I2C_Activate;				// Enable or Disable I2C operations
	uint8_t					I2C_Interrupt;				// Enable or Disbale Interrupt
	uint8_t					I2C_Acknowledgement;		// Enable or Disbale Acknowledgement
	uint8_t					I2C_GeneralCall;			// Enable or Disable General Call
	uint8_t					I2C_SelfAddress;			// address of the device! Range is 1 to 127
	uint32_t				I2C_CommunicationSpeed;		// upto 400KHz is the support
	I2CModesOfOperation		I2C_Mode;
	I2CPrescalarValues		I2C_Prescalar;
}I2C_StructureType;

/* exported functions ------------------------------------------------------------------*/
uint8_t I2C_DiscoverConnectedDevices(void);
uint8_t I2C_PrintDescoveredDevices(void);

uint8_t I2CInit(I2C_StructureType *);
uint8_t I2C_UpdateSlaveAddress(uint8_t);
uint8_t I2C_InitStructureDefault(I2C_StructureType *);
uint8_t I2C_StartCommunication(void);
uint8_t I2C_StopCommunication(void);
uint8_t I2C_TransmitBufferFill(const char*);
void I2C_SetReceiveBufferSize(uint8_t);
uint8_t* I2C_GetReceivedData(void);
uint8_t I2C_FlushReceiveBuffer(void);
uint8_t I2C_FlushTransmitBuffer(void);
uint8_t I2C_GetSlaveDirection(void);
void I2C_SetAcknowledgementBit(uint8_t);
uint8_t I2C_GetCommunicationError(void);

#endif // end of __ATMEGA644P_I2C_H
