/**
  ******************************************************************************
  * @file    printf_code.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    10-Dec-2013
  * @brief   Header for printf_code.c
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINTF_CODE_H
#define __PRINTF_CODE_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
//#include <strings.h>
#include <stdlib.h>
#include "atmega644p_usart.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
extern void print(const char *str, ...);

#endif // __PRINTF_CODE_H
