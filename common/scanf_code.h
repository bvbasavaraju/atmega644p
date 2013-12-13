/**
  ******************************************************************************
  * @file    scanf_code.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    10-June-2013
  * @brief   Header for scanf_code.c
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SCANF_CODE_H
#define __SCANF_CODE_H

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
extern int scan(const char* formats, ... );

#endif // __SCANF_CODE_H
