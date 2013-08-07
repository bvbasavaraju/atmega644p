/*
This is to just check the Sanguino board without actual Aurdino IDE and Processing language!
Trying with simple C code.

Requirement:
            1. AVR_GCC Compiler for windows!
            2. for programming the board, should have the programmer
            3. should program using command line / using tool called XLoader (select appropriate device and baud rate)
            4. Should know about the PINS/PORT and how to use it before writing with the C program! :)
            5. DDRA/DDRB/ (in general DDRX) is used for configuring the pins!
            6. PORTX is used for writing to pin or Activating pin as PULL-UPs or PULL_DOWN
            7. PINX is used for Reading the pin!
 */

#include <avr/io.h>
#include <util/delay.h>
#include "atmega644p_gpio.h"

int main(void)
{

    // Insert code

    while(1)
    {
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_SET);
        //PORTB = PORTB | 0x01;
        _delay_ms(500);
        GPIO_Write(GPIOB, PIN_ZERO, GPIO_PIN_RESET);
        //PORTB = PORTB & 0x00;
        _delay_ms(500);
    }

    return 0;
}
