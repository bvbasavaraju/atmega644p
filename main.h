/**
  ******************************************************************************
  * @file    main.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    03-Oct-2014
  * @brief   This file contains the following things:
                + #defines to including features
  ******************************************************************************
  *
  ******************************************************************************
  */

/*******************************************************************************
    Include Header files
*******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "atmega644p_gpio.h"
#include "atmega644p_usart.h"
#include "printf_code.h"
#include "scanf_code.h"
#include "atmega644p_i2c.h"

/*******************************************************************************
    GPIO #defines
*******************************************************************************/
#ifndef INCLUDE_GPIO
#define INCLUDE_GPIO    0
#endif // INCLUDE_GPIO

#ifndef USE_GPIO_BAREMETAL
#define USE_GPIO_BAREMETAL  0
#endif // USE_GPIO_BAREMETAL

#ifndef USE_GPIO_DRIVER
#define USE_GPIO_DRIVER  0
#endif // USE_GPIO_DRIVER

/*******************************************************************************
    USART #defines
*******************************************************************************/
#ifndef INCLUDE_USART
#define INCLUDE_USART   1
#endif // INCLUDE_USART

#ifndef USE_USART_BAREMETAL
#define USE_USART_BAREMETAL 0
#endif // USE_USART_BAREMETAL

#ifndef USE_USART_DRIVER
#define USE_USART_DRIVER 1
#endif // USE_USART_DRIVER

/*******************************************************************************
    I2C #defines
*******************************************************************************/
#ifndef INCLUDE_I2C
#define INCLUDE_I2C   1
#endif // INCLUDE_I2C

#ifndef USE_I2C_BAREMETAL
#define USE_I2C_BAREMETAL 0
#endif // USE_I2C_BAREMETAL

#ifndef USE_I2C_DRIVER
#define USE_I2C_DRIVER 1
#endif // USE_I2C_DRIVER
