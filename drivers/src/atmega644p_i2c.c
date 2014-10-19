/**
  ******************************************************************************
  * @file    atmega644p_i2c.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    09-Oct-2013
  * @brief   This file has the I2C configuration and Transmitting and receiving the data via I2C
  * @Note	 For Calculating the BitRateRegister Value Below formula has been used
			 BitRateRegister = (((F_CPU / I2C_Speed) - 16 ) / 2 ) / (4 ^ PrescalarValue)
			 BitRateRegister Value should be greater than or equal to 10 for Master mode
  ******************************************************************************
  *
  *					HOW TO USE
  * 1. Call The initialization function for I2C/TWI using I2CInit()
  * -> Master Mode:
  * 2. For Master mode, Update the slave address by using function I2C_UpdateSlaveAddress()
  * 3. If data needs to be transmitted, first fill the transmit buffer using I2C_TransmitBufferFill()
  * 4. if data needs to be recevied, then set receive buffer size using I2C_SetReceiveBufferSize()
  * 5. Start the communication by calling I2C_StartCommunication()
  * 6. for master receive, wait for data to be received in the buffer using I2C_GetReceivedData()
  * 7. Once Data is received after performing actions please make sure that buffer is flusshed using I2C_FlushReceiveBuffer()
  * -> Slave Mode:
  * 2. fill the data using I2C_TransmitBufferFill(), Incase if master requests data then this data will be used!
  * 3. Wait until device is address using I2C_GetSlaveDirection()
  * 4. Once device is addressed, check the direction using I2C_GetSlaveDirection()
  * 5. If it is receive, then wait for data to be filled in buffer using i2c_DataReceived()
  * 6. Once Data is received and after performing actions, flush the receive buffer using ()
  * 7. If it is slave transmit or receive, once the usage is completed make sure that transmit buffer is flushed
  *    using I2C_FlushTransmitBuffer(), is transmit buffer was filled!
  ******************************************************************************
  */

/*----------------------------------- Includes -------------------------------*/
#include "atmega644p_i2c.h"

/*---------------------------------- Global Variables ----------------------------------*/
static uint8_t gI2C_Slave_Address;
static uint8_t gI2C_Self_Address;
static uint8_t gI2C_Address_Positive_ACK = 0x00;	// == 0x01 when Positive ACK is received! == 0x02 for negative ack!
static uint8_t gI2C_TransmitFlag = 0xFF;	// 0xFF = Direction Unknown, 0x00 -> Receive, 0x01 -> Transmit
static uint8_t gI2C_CommunicationError =0x00;
static I2CModesOfOperation gMode;

static uint8_t* gI2C_Connected_Devices = NULL;
static uint8_t* gReceive_Buffer_I2C = NULL;		// Once Received data is printed the buffer needs to be freed
static uint8_t* gTrasnmit_Buffer_I2C = NULL;		// Once the data is tranmitted the buffer needs to be freed
static uint8_t gReceive_Buffer_Index = 0;
static uint8_t gTransmit_Buffer_Index = 0;
static uint8_t gReceiveBufferSize = 0;


/*---------------------------------- Function and Hooks ----------------------------------*/

/*
 * @name	I2CInit
 * @brief	This function initializes the I2C
 * @param	I2CStruct - a pointer of type I2C_StructureType
 * @retval	0x00 - Successful in initializing the I2C
			0x01 - Device's selve address is > 0x7F
			0x02 - BitRateRegister cannot have < 10 value for Master Mode
			0x03 - Communication speed is above the supported range (upto 400KHz)!
 * @note
 */
uint8_t I2CInit(I2C_StructureType *I2CStruct)
{
	uint32_t	bitRateRegister = 0;
	uint8_t 	retVal = 0x00;	//No Error

	gMode = I2CStruct->I2C_Mode;
	if(gMode != eSLAVE_MODE)
		gI2C_TransmitFlag = (gMode == eMASTER_TRANMIT_MODE) ? 0x01 : 0x00;
	gI2C_Self_Address = I2CStruct->I2C_SelfAddress;

	if((gI2C_Self_Address) && (gI2C_Self_Address <= 0x7F))
	{
		TWAR = (gI2C_Self_Address << 1) | (I2CStruct->I2C_GeneralCall != DISABLE ? 0x01 : 0x00);
	}
	else
	{
		retVal = 0x01;	// Self Address Not in Range!
		return retVal;
	}

	TWCR = I2CStruct->I2C_Activate != DISABLE ?  0x04 : 0x00;			// -> 0000 0100
	TWCR |= I2CStruct->I2C_Acknowledgement != DISABLE ? 0x40 : 0x00;	// -> 0100 0000

	if(I2CStruct->I2C_Interrupt != DISABLE)
	{
		TWCR |= 0x01;
		SREG |= GLOBAL_INTERRUPT_FLAG_ENABLE;
	}

	if(I2CStruct->I2C_CommunicationSpeed <= 400000)
	{
		bitRateRegister = ((F_CPU / I2CStruct->I2C_CommunicationSpeed) - 16) / 2 / (4 ^ I2CStruct->I2C_Prescalar);
		if((gMode != eSLAVE_MODE) && (bitRateRegister < 10))
		{
			retVal = 0x02;	// BitRateRegister cannot have < 10 value for Master Mode
			return retVal;
		}
		else
		{
			TWBR = bitRateRegister;
		}
	}
	else
	{
		retVal = 0x03;	// Communication Speed is above the supported Range
		return retVal;
	}

	return retVal;
}

/*
 * @name	I2C_UpdateSlaveAddress
 * @brief	This function will set the address of the slave!
 * @param   slaveAddress - of type uint8_t
 * @retval	0x00 - Succeed!
			0x04 - Device cannot address itself as slave address
			0x05 - Slave address is greater than 0x7F
			0x06 - I2C in slave mode and trying to adrress assign the slave address!

 * @note	To have the successfull update of the slave address make sure that I2C is in Master mode and
			Slave address is <=7F
 */
uint8_t  I2C_UpdateSlaveAddress(uint8_t slaveAddress)
{
	uint8_t retVal = 0x00;	//No Error

	if(gMode != eSLAVE_MODE)
	{
		if(slaveAddress <= 0x7F)
		{
			if(slaveAddress != gI2C_Self_Address)
			{
				gI2C_Slave_Address = slaveAddress;
			}
			else
			{
				retVal = 0x04;	//Device cannot addressed to itself
				return retVal;
			}
		}
		else
		{
			retVal = 0x05;		//Slave Address is not in range!
			return retVal;
		}
	}
	else
	{
		retVal = 0x06;	// I2C in Slave mode and trying to assign Slave address!
	}

	return retVal;
}

/*
 * @name	I2C_InitStructureDefault
 * @brief	This Function will initializes the structure with default values!
 * @param	I2CStruct which is a pointer of type I2CI2C_StructureType
 * @retval  0x00 - Succeed!
 * @note	By defualt Atmega644P is configured as the Master transmitter! with the speed of 100KHz and with self address 0x70
 *			General call and Acknoledgement has been disabled!
 */

uint8_t I2C_InitStructureDefault(I2C_StructureType *I2CStruct)
{
	I2CStruct->I2C_Activate = ENABLE;			// I2C is activated!
	I2CStruct->I2C_Interrupt = ENABLE;			// Interrupt is enabled!
	I2CStruct->I2C_Acknowledgement = DISABLE;	// Acknoledgement is disabled!
	I2CStruct->I2C_GeneralCall = DISABLE;		// General Call disabled!
	I2CStruct->I2C_SelfAddress = 0x70;			// Default value of self address is 70
	I2CStruct->I2C_CommunicationSpeed = 100000;	// 100KHz
	I2CStruct->I2C_Mode = eMASTER_TRANMIT_MODE;	// Master Transmit!
	I2CStruct->I2C_Prescalar = eNO_PRESCALAR;	// No Prescalar!

	return 0x00;
}

/*
 * @name	I2C_StartCommunication
 * @brief	This Function will set the start I2C communication bit.
 * @param	-
 * @retval  0x00 - Succeed!
			0x07 - I2C is configured in Slave mode and requested to start the I2C communication
			0x08 - I2C is not enabled to start the communication
 * @note
 */
uint8_t I2C_StartCommunication(void)
{
	uint8_t retVal = 0x00;
	if(TWCR & 0x04)		// Check for I@C enable condition!
	{
		if(gMode != eSLAVE_MODE)
		{
			TWCR |= 0xA0;	// -> 1010 0000
		}
		else
		{
			retVal = 0x07;	// Device is configured in Slave mode and requested for start I2C communication
		}
	}
	else
	{
		retVal = 0x08;		// I2C is not enabled to start the communication
	}
	return retVal;
}

/*
 * @name	I2C_SetStopBit
 * @brief	This Function will set the stop I2C communication bit.
 * @param	-
 * @retval  0x00 - Succeed!
			0x09 - I2C is configured in Slave mode and requested to stop the I2C communication
 * @note	Set the stop bit meaning either tranmission or receive operation is complete.
 *			Only Transmit Buffer is emptied! Receive buffer has to be emptied once the data is read!
 */
static uint8_t I2C_SetStopBit(void)
{
	uint8_t retVal = 0x00;
	if(gMode != eSLAVE_MODE)
	{
		TWCR |= 0x10; 	// -> 0001 0000

		I2C_FlushTransmitBuffer();
	}
	else
	{
		retVal = 0x09;	// Device is configured in Slave mode and requested for stop I2C communication
	}
	return retVal;
}

/*
 * @name	I2C_StopCommunication
 * @brief	This Function will stop the I2C communication.
 * @param	-
 * @retval  0x00 - Succeed!
 * @note	All ongoing communication will be stopped if this function is called!
 */
uint8_t I2C_StopCommunication(void)
{
	uint8_t retVal = 0x00;

	TWCR &= 0xFB;	// -> 1111 1011
	I2C_FlushReceiveBuffer();
	I2C_FlushTransmitBuffer();

	return retVal;
}

/*
 * @name	I2C_TransmitBufferFill
 * @brief	This Function will copy the data needs to be transmitted!
 * @param	data - pointer of type uint8_t -> data needs to be tranmitted to other device!
 * @retval  0x00 - Succeed!
			0x0A - I2C is configured in Slave mode and asked to fill the tranmit buffer
 * @note
 */
uint8_t I2C_TransmitBufferFill(const char* data)
{
	uint8_t retVal = 0x00;
	uint8_t length = 0;
	uint8_t *temp = (uint8_t *)data;

	if(gMode == eSLAVE_MODE || gI2C_TransmitFlag == 0x01)		//Either in slva mode or for Master Transmit mode!
	{
		while(*temp != '\0')
		{
			length++;
			temp++;
		}
		if(length != 0)
		{
			gTrasnmit_Buffer_I2C = (uint8_t*)(malloc((sizeof(uint8_t) * length) + 1));	// Allocate only the necessary size of buffer
		}
		for(gTransmit_Buffer_Index = 0; *data != '\0'; data++, gTransmit_Buffer_Index++)
			gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index] = *data;

		gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index] = '\0'; // End of String
		gTransmit_Buffer_Index = 0;	//Setting Index back to 0
	}
	else
	{
		retVal = 0x0A;		// I2C is configured as receive and not for transmit!
	}

	return retVal;
}

/*
 * @name	I2C_SetReceiveBufferSize
 * @brief	This Function will set the receive buffer size!
 * @param	length - Number of bytes that Device has to receive!
 * @retval  -
			0x0B - Receive Buffer is full! Cannot receive any more data!
 * @note	It will not allocate memory yet. it will allocate when first byte has been received!
 */
void I2C_SetReceiveBufferSize(uint8_t length)
{
	gReceiveBufferSize = length;
}

/*
 * @name	I2C_ReceivedData
 * @brief	This Function will copy the data the has been received!
 * @param	data - of type uint8_t -> data that has been recieved!
 * @retval  0x00 - Succeed!
			0x0B - Receive Buffer is full! Cannot receive any more data!
 * @note
 */
static uint8_t I2C_ReceivedData(uint8_t data)
{
	uint8_t retVal = 0x00;

	if((gI2C_TransmitFlag == 0x00) && (gReceive_Buffer_Index == 0))
		gReceive_Buffer_I2C = (uint8_t *)(calloc(gReceiveBufferSize + 1, sizeof(uint8_t)));

	if(gReceive_Buffer_Index < gReceiveBufferSize)
	{
		gReceive_Buffer_I2C[gReceive_Buffer_Index] = data;
		gReceive_Buffer_Index++;
	}
	else
	{
		retVal = 0x0B;		// Receive Buffer is full!
	}

	return retVal;
}

/*
 * @name	I2C_PrintReceivedData
 * @brief	This function returns the pointer to the received data!
 * @param
 * @retval  NULL - Receive Buffer is empty
			or Recieved data
 * @note	Developer has to make sure to Flush the received buffer after it is assumed that all data is read!
 */
uint8_t* I2C_GetReceivedData(void)
{
	uint8_t* retVal = NULL;	//Received Buffer is Empty!

	if(gReceive_Buffer_Index != 0 && (gReceive_Buffer_I2C[gReceive_Buffer_Index] == '\0'))
	{
		retVal = gReceive_Buffer_I2C;
	}

	return retVal;
}

/*
 * @name	I2C_FlushReceiveBuffer
 * @brief	This function flush the receive buffer and resets the index to 0
 * @param	-
 * @retval  0x00 - Succeed!
			0x0C - Receive Buffer is Empty!
 * @note
 */
uint8_t I2C_FlushReceiveBuffer(void)
{
	uint8_t retVal = 0x00;

	if(gReceive_Buffer_Index != 0)
	{
		free(gReceive_Buffer_I2C);
		gReceive_Buffer_Index = 0;
	}
	else
		retVal = 0x0C;   // Receive Buffer is Empty!

	return retVal;
}

/*
 * @name	I2C_FlushTransmitBuffer
 * @brief	This function flush the Transmit buffer and resets the index to 0
 * @param	-
 * @retval  0x00 - Succeed!
			0x0C - Receive Buffer is Empty!
 * @note	In Slave Mode this function has to be called if transmit buffer was filled but only slave receive has been requested!
 */
uint8_t I2C_FlushTransmitBuffer(void)
{
	uint8_t retVal = 0x00;

	if(gTrasnmit_Buffer_I2C != NULL)
	{
		free(gReceive_Buffer_I2C);
		gTransmit_Buffer_Index = 0;
	}
	else
		retVal = 0x0C;   // Receive Buffer is Empty!

	return retVal;
}

/*
 * @name	I2C_DiscoverConnectedDevices
 * @brief	This function will find all the devices connected to I2C
 * @param	-
 * @retval  0x00 - Succeed!
 *			0x0D - There are no connected devices
 * @note	This function will only check if there is a positive ACK for the address! it will not send any data after that!
 *			gI2C_Connected_Devices[index] == 0xFF if no device is connected for that address!
 */
uint8_t I2C_DiscoverConnectedDevices(void)
{
	uint8_t retVal = 0x00;
	uint8_t address;
	uint8_t noDevicesConnected = 0x01;
	I2C_StructureType i2c;


	if((!(I2C_InitStructureDefault(&i2c))) && (!(I2CInit(&i2c))))	// Configure as Master Trasmitter! and Initialize the I2C registers!
	{
		gI2C_Connected_Devices = (uint8_t *)(malloc(sizeof(uint8_t) * TOTAL_POSSIBLE_DEVICES));
		for(address = 0; address<=0x7F; address++)
		{
			gI2C_Address_Positive_ACK = 0x00;
			if(!(I2C_UpdateSlaveAddress(address)))	// Update the address!
			{
				I2C_StartCommunication();			// Start the communication and wait for the flag to change!
				while(!gI2C_Address_Positive_ACK)
				;

				if(gI2C_Address_Positive_ACK == 0x01)
				{
					gI2C_Connected_Devices[address] = address & 0xFF;
					noDevicesConnected = 0x00;		// at least one device is connected!
				}
				else
					gI2C_Connected_Devices[address] = 0xFF;			// When device's address doesn't match!
			}
			else
			{
				gI2C_Connected_Devices[address] = 0xFF;		// For Self address as well it is assumed to be negative ack as it cannot address itself!
				continue;
			}
		}
		gI2C_TransmitFlag = 0xFF;	//Resett the Transmit Flag to Unknown direction
	}

	if(noDevicesConnected)
		retVal = 0x0D;		//There are no devices connected!

	return retVal;
}

/*
 * @name	I2C_PrintDescoveredDevices
 * @brief	This function will find print the devices connected!
 * @param	-
 * @retval  0x00 - Succeed!
 *			0x0E - Devices are not been discovered yet!
 * @note	This function will has to be called only after calling I2C_DiscoverConnectedDevices() else return an error code 0x0E
 *          Here, print() is used to print the devices connected! User can replace this function!
 */
uint8_t I2C_PrintDescoveredDevices(void)
{
	uint8_t retVal = 0x00;
	uint8_t index;

	if(gI2C_Connected_Devices != NULL)	// Make sure that I2C_DiscoverConnectedDevices() is called before calling this function!
	{
		print("\n\r\t0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F");
		for(index = 0; index < TOTAL_POSSIBLE_DEVICES; index++)
		{
			if(index%16 == 0)
				print("\n\r%x\t", (uint32_t)(index));

			if(gI2C_Connected_Devices[index] != 0xFF)
			{
				print("0x%x  ", (uint32_t)(gI2C_Connected_Devices[index]));
			}
			else
                print("-     ");
		}
	}
	else
		retVal = 0x0E;	//Devices are not been discovered yet!

	return retVal;
}

/*
 * @name	I2C_SetAcknowledgementBit
 * @brief	This function will set or rest the Acknowledgement bit of control register!
 * @param	value - 1 - Set the Acknoledgement bit
 *					0 - Rest the Acknoledgement bit
 * @retval  -
 * @note	This function can be called during slave mode of the device. It can also be called to re-establish the I2C with this device in slave mode!
 */
void I2C_SetAcknowledgementBit(uint8_t value)
{
	if(value)
		TWCR |= 0x40;
	else
		TWCR &= 0xBF;
}

/*
 * @name	I2C_GetCommunicationError
 * @brief	This function will display on the console, the error message!
 * @param	-
 * @retval  0x00 - No Communication error
 *			0x10 - Negative Acknowledgement for the Address
 *			0x20 - Negative Acknoledgement for the Data
 *			0x30 - Master lost the Arbitration
 *			0x40 - Illegal Start or Stop error
 * @note
 */
uint8_t I2C_GetCommunicationError(void)
{
	return gI2C_CommunicationError;
}

/*
 * @name	I2C_ResetInterruptFalg
 * @brief	This function will reset the interrupt flag!
 * @param	-
 * @retval  -
 * @note	Once the ISR has been served, TWINT flag has to be reset by setting it to one!
 */
static void I2C_ResetInterruptFalg(void)
{
	TWCR |= 0x80;	// -> 1000 0000
}

/*
 * @name	I2C_GetSlaveDirection
 * @brief	This function will Gives the direction of the Transmit flag
 * @param	-
 * @retval  0x00 -> in receive mode
 *			0x01 -> in transmit mode
 *			0xFF -> Direction Unknown
 * @note	It can be called in the main application side, to check the direction of slave. and then take the action.
 */
uint8_t I2C_GetSlaveDirection(void)
{
	return gI2C_TransmitFlag;
}

/*
 * @name	I2C_IRQHandler
 * @brief	This is a ISR for I2C or TWI of Atmega644P
 * @param	-
 * @retval  -
 * @note	The user can modify this according to his requirements!
 */
I2C_IRQHandler()
{
	switch(TWSR & 0xFF)
	{
	//Master Common
		case MASTER_START_SENT:
		case MASTER_REPEATED_START_SENT:

			TWDR = (((gI2C_Slave_Address & 0xFF) << 1) | (gMode & 0x01));
			TWCR &= 0xDF; // -> 1101 1111 Reset the TWSTA bit in the control regester! to avoid repeated start!
			I2C_ResetInterruptFalg();
			break;

	//Master Trasmit
		case MASTER_TRANSMIT_ADDRESS_POSITIVE_ACK:
			gI2C_Address_Positive_ACK = 0x01;		// for Discovering devices!
		case MASTER_TRANSMIT_DATA_POSITIVE_ACK:
			if((gTrasnmit_Buffer_I2C != NULL) && (gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index] != '\0'))
			{
				TWDR = gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index] & 0xFF;
				gTransmit_Buffer_Index++;
				I2C_ResetInterruptFalg();
			}
			else
			{
				I2C_SetStopBit();
				I2C_ResetInterruptFalg();
			}
			break;

		case MASTER_TRANSMIT_ADDRESS_NEGATIVE_ACK:

			gI2C_Address_Positive_ACK = 0x02;		// for Discovering devices!
			gI2C_CommunicationError = 0x10;			//Address Negative Acknoledgement
			I2C_SetStopBit();
			I2C_ResetInterruptFalg();
			break;

		case MASTER_TRANSMIT_DATA_NEGATIVE_ACK:

			gI2C_CommunicationError = 0x20;		//Data Negative error
			I2C_SetStopBit();
			I2C_ResetInterruptFalg();
			break;

		case MASTER_ARBITRATION_LOST:

			gI2C_CommunicationError = 0x30;		//Master Lost the Arbitration!
			I2C_StartCommunication();		// Start again once the I2C bus is free!
			break;

	//Master Receive
		case MASTER_RECEIVE_DATA_POSITIVE_ACK:
			I2C_ReceivedData(TWDR & 0xFF);
		case MASTER_RECEIVE_ADDRESS_POSITIVE_ACK:

			if(gReceive_Buffer_Index >= gReceiveBufferSize)
			{
				if(gReceive_Buffer_Index != 0)
					gReceive_Buffer_I2C[gReceive_Buffer_Index] = '\0';	//End Of String
				I2C_SetAcknowledgementBit(0); //Negative acknowledgement to inform slave to stop sending more data!
			}
			else
			{
				I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			}

			I2C_ResetInterruptFalg();
			break;

		case MASTER_RECEIVE_DATA_NEGATIVE_ACK:
			I2C_ReceivedData(TWDR & 0xFF);
		case MASTER_RECEIVE_ADDRESS_NEGATIVE_ACK:

			I2C_SetStopBit();
			I2C_ResetInterruptFalg();
			break;

	//Slave Receive
		case SLAVE_RECEIVE_ADDRESS_POSITIVE_ACK:
		case SLAVE_RECEIVE_GENERAL_CALL_POSITIVE_ACK:
		case ARBITRATION_LOST_ADDRESSED_SLAVE_RECEIVE:
		case ARBITRATION_LOST_GENERAL_CALL_SLAVE_RECEIVE:

			gI2C_TransmitFlag = 0x00;	//Receive Mode
			I2C_SetReceiveBufferSize(BUFFER_SIZE); //Maximum Read size is set to 128 Bytes! In Slave mode Device cannot recieve more than 128 Bytes!
			I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			I2C_ResetInterruptFalg();	//Clear Interrupt Flag

			break;

		case SLAVE_RECEIVE_DATA_POSITIVE_ACK:
		case SLAVE_RECEIVE_GENERAL_CALL_DATA_POSITIVE_ACK:

			I2C_ReceivedData(TWDR & 0xFF);
			if(gReceive_Buffer_Index >= gReceiveBufferSize)
			{
				gReceive_Buffer_I2C[gReceive_Buffer_Index] = '\0';
				I2C_SetAcknowledgementBit(0); //Negative Acknoledgement
			}
			else
			{
				I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			}
			I2C_ResetInterruptFalg();
			break;

		case SLAVE_RECEIVE_DATA_NEGATIVE_ACK:
		case SLAVE_RECEIVE_GENERAL_CALL_DATA_NEGATIVE_ACK:

			I2C_SetAcknowledgementBit(0); //Negative Acknoledgement
			I2C_ResetInterruptFalg();
			break;

		case SLAVE_STOP_OR_REPEATED_START:
			if(gReceive_Buffer_Index < gReceiveBufferSize)
				gReceive_Buffer_I2C[gReceive_Buffer_Index] = '\0';
			I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			I2C_ResetInterruptFalg();
			break;

	//Slave Transmit
		case SLAVE_TRANSMIT_ADDRESS_POSITIVE_ACK:
		case ARBITRATION_LOST_ADDRESSED_SLAVE_TRANSMIT:
			gI2C_TransmitFlag = 0x01;		//Set the tranmit flag to 1
			gTransmit_Buffer_Index = 0;		// TO make sure that index is pointing to 0
		case SLAVE_TRANSMIT_DATA_POSITIVE_ACK:

			if((gTrasnmit_Buffer_I2C != NULL) && (gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index] != '\0'))
			{
				TWDR = gTrasnmit_Buffer_I2C[gTransmit_Buffer_Index];
				gTransmit_Buffer_Index++;
				I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			}
			else
			{
				I2C_SetAcknowledgementBit(0); //Negative Acknoledgement
			}
			I2C_ResetInterruptFalg();
			break;

		case SLAVE_TRANSMIT_DATA_NEGATIVE_ACK:
		case SLAVE_TRANSMIT_LAST_DATA_BYTE_POSITIVE_ACK:
			I2C_SetAcknowledgementBit(1); //Positive Acknoledgement
			I2C_FlushTransmitBuffer();
			I2C_ResetInterruptFalg();
			break;

	//ALL TIME!
		case ILLEGAL_START_OR_STOP_CONDITION:

			gI2C_CommunicationError = 0x40;		// Illegal Start or Stop condition
			I2C_SetStopBit();
			I2C_ResetInterruptFalg();
			break;

		case NO_INFORMATION_AVAILABLE:

			//Nothing needs be done here!
			break;

		default:
			print("\n\rImplementation of state %x has been missed! Inform the developer of this driver\n\r", (uint32_t)(TWSR&0xFF));
			break;
	}
}
