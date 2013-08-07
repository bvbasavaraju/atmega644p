/**
#include "atmega644p_gpio.h"
/*
 *  This Function returns the pointer to the SFR_IO8 page! So it should be volatile
 *  Check iomxx4.h file for more detials on PORTx/PINx/DDRx
 *  Check the file sfr_defs.h for more details on SFR Pages and address!
 *  As the atmega644p is a 8 bit controller the return address is of type uint8_t
 *  Volatile is used for ret value and the function because to avoid warnings and compiler should not optimise the code!
 */
    volatile uint8_t *ret = 0;
	return ret;
