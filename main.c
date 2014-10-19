/*
This is to just check the Sanguino board without actual Aurdino IDE and Processing language!
Trying with simple C code.

Requirement:
            1. AVR_GCC Compiler for windows!
            2. for programming the board, should have the programmer
            3. should program using command line / using tool called XLoader (select appropriate device and baud rate)
            4. Should know about the PINS/PORT and how to use it before writing with the C program! :)
 */

#include "main.h"


//unsigned char c = '0';
uint16_t ch;
uint8_t byte;
uint8_t i2c_error;
uint8_t i2c_state = 0x00;
uint32_t address;       //Always use variables with _t in embedded word!
int main(void)
{
    /******************************************************************
                    GPIO TEST
    *******************************************************************/
#if (INCLUDE_GPIO > 0)
    while(1)
    {
    #if (USE_GPIO_BAREMETAL > 0)
        PORTB = PORTB | 0x01;
        _delay_ms(500);
        PORTB = PORTB & 0x00;
        _delay_ms(500);
    #endif //USE_GPIO_BAREMETAL

    #if (USE_GPIO_DRIVER > 0)
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_SET);
        _delay_ms(500);
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_RESET);
        _delay_ms(500);
    #endif //USE_GPIO_DRIVER
    }
#endif //INCLUDE_GPIO

    /******************************************************************
                    USART TEST
    *******************************************************************/

#if(INCLUDE_USART > 0)
    #if (USE_USART_BAREMETAL > 0)
	SREG = SREG | 0x80; // enable Global interrupt!
    UCSR0A = 0x00;
    UCSR0B = 0x00;
    UCSR0C = 0x00;

    //UCSR0A = 0x20; //UCSR0A & 0x20; // To indicate that Tx buffer is ready to recieve data
    UCSR0B = 0x98; //0x18;  //0x98; //UCSR0B & 0x08; // RX and TX enable bit
    UCSR0C = 0x06; //UCSR0C & 0x06; // 8-bit data in a frame

    uint16_t BaudRegisterValue;
    //BaudRegisterValue = (F_CPU / 16 - 115200)/ 115200 ;
    //BaudRegisterValue = ((F_CPU / 16) / 115200)  - 1;
    BaudRegisterValue = ((F_CPU / 16 + 19200 / 2) / 19200) - 1;
    UBRR0H = BaudRegisterValue >> 8;
    UBRR0L = BaudRegisterValue;

    while(1)
    {
        while(!(UCSR0A & 0x80))
            ;//No data to read the RX buffer
        c = UDR0;

        while(!(UCSR0A & 0x20))
            ;//Tx Buffer is not empty, wait for buffer to become empty to transmitt another character
        UDR0 = c;
    }
    #endif //USE_USART_BAREMETAL

    #if (USE_USART_DRIVER > 0)
	//Driver part
	USART_StructureType USART_Config;

    USART_Config.USART_BaudRate = 19200;
    USART_Config.USART_Communication = BOTH;
    USART_Config.USART_DataBits = EIGHT;
    USART_Config.USART_Modes = ASYNCHRONOUS;
    USART_Config.USART_Parity = NOPARITY;
    USART_Config.USART_StopBits = ONESTOPBIT;

    USARTInit(USART1, USART_Config);
    //USART_EnableInterrupt(RECEIVE);
    print("\n\rUSART is Configured at Baud Rate 19200\n\r");

    /*while(1)
    {
        char c;
        char *str;
        uint32_t i;
        uint32_t hex;

        str = (char *) malloc(200 * sizeof(char));

        // "\r" is used to make sure that CARRIAGE RETURN (start from the beggining of the line) is used
        // "\n" is the NEW LINE (next line) is used!
        print("Enter the values for integer i and character c:\n\r");
        scan("%d %c", &i, &c);
        //i = 45;
        //x = 'i';
        print("\n\rint i = %d char x = %c\n\r",i,c);

        print("Enter an Hex number:\n\r");
        scan("%x", &hex);
        print("\n\rhex = %x", hex);
        print("\n\rEnter String:\n\r");
        scan("%s", str);
        print("\n\rEntered String is:\n\r%s\n\r", str);

        //ch = USART_GetChar();
        //USART_PutChar(ch);
        //if(gReceive_Buffer_Full == 1)
        //{
        //    USART_ClearReceiveBuffer();
        //}

        free(str);
    }*/
    #endif //USE_USART_DRIVER
#endif  //INCLUDE_USART
	/******************************************************************
                    I2C or TWI test
    *******************************************************************/
#if (INCLUDE_I2C > 0)
	#if (USE_I2C_BAREMETAL > 0)
	SREG = SREG | 0x80; // enable Global interrupt!

	TWAR = 0xE2;	// address will be 71 and No global call recognition
	TWSR = 0xF8;	// Prescalar is 0
	TWBR = ((F_CPU / 100000) - 16) / 2;	//I2C transmit at 100KHz.
	//TWCR &= 0xA5;	// Interrupt Flag bit, Start bit, I2C enable and Interrupt enable bits are set

    //print("\n\rI2C is configured for Master Transmitter\n\r");
    //print("I2C Device address is 71\n\r");
    //print("This Device is looking for slave with address 72\n\r");

    i2c_state = 0xFF;

    //To Find the connected devices!
    for(address = 0 ; address<128; address++)
    {
        i2c_error = 0xFF;

        while(i2c_state != 0xFF)
            ;
        i2c_state = 0x00;

        TWCR = 0xE5;	// Interrupt Flag bit, Start bit, I2C enable and Interrupt enable bits are set

        while(i2c_state != 0xFF)
            ;

        if(i2c_error == 0xFF)// || i2c_error == 2)
            print("\n\rconnected slave address is: %d", address);
    }
    #endif //USE_I2C_BAREMETAL

    #if (USE_I2C_DRIVER > 0)
    print("\n\rDiscovering the connected I2C device!\n\r");
    if(!I2C_DiscoverConnectedDevices())
    {
        I2C_PrintDescoveredDevices();
    }
    else
        print("\n\rFailed to discover any devices!");

    I2C_StructureType i2c;
    i2c.I2C_Activate = ENABLE;
    i2c.I2C_Interrupt = ENABLE;
    i2c.I2C_Acknowledgement = ENABLE; //DISABLE;
    i2c.I2C_GeneralCall = DISABLE;
    i2c.I2C_SelfAddress = 0x70;	// -> 0x70 << 1 == 0xE0
    i2c.I2C_CommunicationSpeed = 100000;
    i2c.I2C_Mode = eMASTER_TRANMIT_MODE; //eMASTER_RECEIVE_MODE; //eSLAVE_MODE;
    i2c.I2C_Prescalar = eNO_PRESCALAR;

    //Master Transmit
    if(!I2CInit(&i2c))
    {
        print("\n\n\rConfigured the I2C as Master Transmitter");
        if(!(I2C_UpdateSlaveAddress(0x39))) // Slave address is 0x72 == (0x39 << 1)
        {
            if(!(I2C_TransmitBufferFill("BUG")))
            {
                I2C_StartCommunication();
                print("\n\rCheck for the LED glow in slave!");
            }
        }
    }
    //Master Receive
    //char *i2c_DataReceived = NULL;
    /*if((!I2CInit(&i2c)) && (!(I2C_UpdateSlaveAddress(0x39))))
    {
		print("\n\n\rConfigure the I2C as Master Receiver");
        I2C_SetReceiveBufferSize(10);
        I2C_StartCommunication();
        while((i2c_DataReceived = (char *)I2C_GetReceivedData()) == NULL)
            ;
        print("\n\rReceived data from Slave is : %s", i2c_DataReceived);
        I2C_FlushReceiveBuffer();
        print("\n\rReceive Buffer has been flushed!");
    }*/
    //Slave mode!
    /*if(!I2CInit(&i2c))
    {
		print("\n\rConfigured the I2C as Slave\n");
		I2C_TransmitBufferFill("BUG");		// Incase if slave is requested to transmit!

		while(I2C_GetSlaveDirection() == 0xFF)
			; //Wait until any master address this as slave

		if(I2C_GetSlaveDirection() != 0x01)	//receive mode
		{
			print("\n\rDevice in Receive Mode");
			while((i2c_DataReceived = (char *)I2C_GetReceivedData()) == NULL)
				; // Wait for the reception to complete!
			print("\n\rReceived data from Master is : %s", i2c_DataReceived);
			I2C_FlushReceiveBuffer();
			print("\n\rReceive Buffer has been flushed!");
		}
		else
		{
		    print("\n\rDevice in Transmit Mode");
			print("\n\rCheck for the LED glow in Master!");
			I2C_FlushTransmitBuffer();
		}
    }*/

    while(1)
    {

    }
    #endif //USE_I2C_DRIVER
    return 0;
#endif  //INCLUDE_I2C
}

#if (USE_USART_BAREMETAL > 0)
ISR(USART0_RX_vect)
{
    while(!(UCSR0A & 0x80))
        ;//No data to read the RX buffer
    c = UDR0 & 0xFF;
    UDR0 = c;
    //UDR0 = ((UBRR0L / 10) + '0');
    //UDR0 = ((UBRR0L % 10) + '0');
}
#endif //USE_USART_BAREMETAL

#if (USE_I2C_BAREMETAL > 0)
ISR(TWI_vect)
{
    //print("\n\rInside I2C ISR\n\r");
	switch(TWSR & 0xF8)
	{
		case 0x08:
		case 0x10:
			//transmit the address of slave with wrtie for Master transmit
			//TWDR = 0xE4;	//72 is slave address and master is transmitting
			TWDR = ((address & 0xFF) << 1);	//72 is slave address and master is transmitting
			TWCR = 0x85;	// Interrupt flag, I2C enable and interrupt enable bits are set
			//print("\n\rNow Transmitted the Slave address %d\n\r", (address&0xFF));
			break;

		case 0x18:	//address ACK
			TWDR = 0x41;	// ASCII value of 'A'
			TWCR = 0x85;
			print("\n\rPossitive Acknoledgement form slave for address %d\n\r", address);
			break;

		case 0x20:	//Address NACK
			//TWDR = 0xE4;	//72 is slave address and master is transmitting
			TWCR = 0x85;	// Interrupt flag, I2C enable and interrupt enable bits are set
			i2c_error = 1;
			//print("\n\rNegetive Acknoledgement form slave for address %d\n\r", (address&0xFF));
			break;

		case 0x28:	//Data ACK
			TWCR = 0x95;	//Interrupt flag, I2C enable and interrupt enable bits are set
			i2c_state = 0xFF;
			print("\n\rPossitive Acknoledgement form slave for Data\n\r");
			break;

		case 0x30:	//Data NACK
			TWCR = 0x95;	//Interrupt flag, I2C enable and interrupt enable bits are set
			i2c_error = 2;
			i2c_state = 0xFF;
			//print("\n\rNegetive Acknoledgement form slave for address %d\n\r", (address&0xFF));
			break;
		default:
		    print("\n\rNot a valid status for Master Transmitter\n\r");
			break;
	}
}
#endif //USE_I2C_BAREMETAL

