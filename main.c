/*
This is to just check the Sanguino board without actual Aurdino IDE and Processing language!
Trying with simple C code.

Requirement:
            1. AVR_GCC Compiler for windows!
            2. for programming the board, should have the programmer
            3. should program using command line / using tool called XLoader (select appropriate device and baud rate)
            4. Should know about the PINS/PORT and how to use it before writing with the C program! :)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "atmega644p_gpio.h"
#include "atmega644p_usart.h"


unsigned char c = '0';
uint16_t ch;
uint8_t byte;
int main(void)
{

    // Insert code
    /*
                    GPIO TEST
    */
    /*while(1)
    {
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_SET);
        //PORTB = PORTB | 0x01;
        _delay_ms(500);
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_RESET);
        //PORTB = PORTB & 0x00;
        _delay_ms(500);
    }*/

    /*
                    USART TEST
    */

    /*SREG = SREG | 0x80; // enable Global interrupt!
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
    */
    USART_StructureType USART_Config;

    USART_Config.USART_BaudRate = 19200;
    USART_Config.USART_Communication = BOTH;
    USART_Config.USART_DataBits = EIGHT;
    USART_Config.USART_Modes = ASYNCHRONOUS;
    USART_Config.USART_Parity = NOPARITY;
    USART_Config.USART_StopBits = ONESTOPBIT;

    USARTInit(USART1, USART_Config);
    //USART_EnableInterrupt(RECEIVE);

    while(1)
    {
        ch = USART_GetChar();
        USART_PutChar(ch);
        //if(gReceive_Buffer_Full == 1)
        //{
        //    USART_ClearReceiveBuffer();
        //}
    }

    return 0;
}


/*ISR(USART0_RX_vect)
{
    while(!(UCSR0A & 0x80))
        ;//No data to read the RX buffer
    c = UDR0 & 0xFF;
    UDR0 = c;
    //UDR0 = ((UBRR0L / 10) + '0');
    //UDR0 = ((UBRR0L % 10) + '0');
}*/

