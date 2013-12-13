/**
  ******************************************************************************
  * @file    scanf_code.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    09-June-2013
  * @brief   This file is having the scanf() equivalent function!
  * 		 This is simplified scanf(). this can be expanded for the different
  *			 data types
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "scanf_code.h"
#include "printf_code.h"

/* defines -------------------------------------------------------------------*/
#define		MAX_BUFFER_LENGTH		200

/******************************************************************************/
/* 						Private function prototypes  						  */
/******************************************************************************/


/* Function Definitions ------------------------------------------------------*/
/**
  * @brief  this function reads the input
  * @param  None
  * @Note	Here Replace the code with USART/UARTs read function call!
  * @retval None
  */
char get()
{
	return USART_GetChar();
	//return getchar();
}

/**
  * @name   copy_Character()
  * @brief  this function will copy the input character to the address passed
  * @param  dest_addr - this is the address of the variable where the
						character to be copied
  * @param  data - address of the input
  * @note	This copies the character only!
  * @retval None
  */
void copy_Character(char *dest_addr, char *data)
{
	*dest_addr = *data;
}

/**
  * @name   copy_String()
  * @brief  this function will copy the input character to the address passed
  * @param  dest_addr - this is the address of the variable where the
						string to be copied
  * @param  data - address of the input
  * @note	This copies the string only!
  * 		Here if ENTER KEY is pressed or Exlaimatory mark is seen then The copying of string will be stopped!
  * @retval None
  */
void copy_String(char *dest_addr, char *data)
{
	int i;
	i = 0;

	//dest_addr= NULL;
	//while((data[i] != ' ') && (data[i] != '\0') && (data[i] != '!'))
    while((data[i] != '\0') && (data[i] != '!'))
	{
		dest_addr[i] = data[i];
		i++;
	}
	dest_addr[i] = '\0';
	//printf("input string is: %s\n", dest_addr);
}

/**
  * @name   copy_Number()
  * @brief  this function will copy the input character to the address passed
  * @param  dest_addr 	- this is the address of the variable where the
						  number to be copied
  * @param  data 		- address of the input
  * @param  lenght 		- lengh of the formats! if it is Decimal then 10 digits is the max lenght and for Hexadecimal 8 is the Max length!
  * @note	This copies the number only range is from -2147483648 to 2147483647!
  * @retval None
  */
void copy_Number(uint32_t *dest_addr, char *data, int length)
{
	int sign;
	int32_t i;

	i = 0;
	sign = 0;

	*dest_addr = 0;

	while((data[i] != ' ') && (data[i] != '\0') && (i < length))	//here max lenth of any integer is considerd to be 4 bytes i.e. 8 nibbles in Hex!
	{
		if((data[0] == '-') && (sign != 1))
		{
			sign = 1;
			data++;
			//printf("sign is : %d\n", sign);
		}
		else
		{
			if(length != 10)
				*dest_addr |= (data[i] >= 'a') ? data[i] - 'a' + 10 : ((data[i] >= 'A') ? data[i] - 'A' + 10 : data[i]- '0');	//incase if you wanna consider the number as the HEX format
			else
            {
                *dest_addr += data[i] - '0';		//if you wanna consider the number as the Decimal format
                //print("\n\ri = %d %d", i, *dest_addr);
            }

			if( i < (length - 1))
			{
				if(length != 10)
					*dest_addr = *dest_addr << 4; // left shift by one nibble size! //input number is considered as HEX number
				else
                {
                    *dest_addr = *dest_addr * 10; // input number is considered as Decimal format
                    //print("\n\ri = %d %d", i, *dest_addr);
                }
			}
			i++;
		}
	}
	if(i < length)
	{
		if(length != 10)
			*dest_addr = *dest_addr >> 4; // remove the nibble shift!
		else
			*dest_addr = *dest_addr / 10; // remove the extra multiplication by 10
	}
	//print("number copied is : %d\n\r", *dest_addr);
	if(sign != 0)
	{
		*dest_addr = *dest_addr * -1;
	}
}

/**
  * @name   scan()
  * @brief  this function will takes the input as string, parse the input string
			and copies the number or string or character to the address given
  * @param  formats - %d or %c or %s passed as a string while calling this function
  * @param  ... - arguments, number of arguments passed are unknown.
  * @note	This function only handles the error corresponds to data type formats
			not the corresponding address passed as a arguement!
			And also make sure that while reading the string, make sure that memory is allocated
			for that string before calling this function
  * @retval 0
  */
int scan(const char* formats, ... )
{
	char *str;
	char character;
	int i = 0;
	va_list arg;

    str = (char *)malloc(MAX_BUFFER_LENGTH * sizeof(char));

    /*
     * For AVR or ARM compiler we need to use the CARRIAGE RETURN (ASCII Value 0x0D) for ENTER KEY in QWERTY keyboard
     * For MinGW (GCC) compiler we need to use NEW LINE (ASCII value 0x0A) for the ENTER KEY in QWERTY keyboard!
     */

    //while((character = get()) != 0x0A)  // 0x0A is the ASCII value of the NEW LINE
	while(((character = get()) != 0x0D) && (i < (MAX_BUFFER_LENGTH - 1)))  // 0x0D is the ASCII value of the CARRIAGE RETURN
	{
		str[i] = character;
		print("%c", str[i]); //to echo the character in a console!
		i++;
	}
	str[i] = '\0';
	//printf("the input from STDIO is : %s\n\r", str);

	va_start(arg, formats);
	//parse the formats arguement and store it in the adrress passed next to it!
	i = 0;
	while(*formats != '\0')
	{
		character = *formats++;
		if(character == '%')
		{
			if(*formats == '%')
			{
				printf("error in the input!");
			}
			else
			{
				switch(*formats)
				{
					case 'c':	//copy the character from the str to address passed over here
								//printf("Character will be copied now!\n");
								copy_Character(va_arg(arg, char *), &str[i]);
						break;

					case 's':	//Copy the string from the str to address passed over here
								//printf("%s to be copied\n\r",&str[i]);
								copy_String(va_arg(arg, char *), &str[i]);
						break;

					case 'd':
					case 'x':
					case 'X':
								//Copy the string of numbers from the str to the address passed over here
								//for Decimal number 10 digits and for Hex it is 8 digits in 32 bit computation!
								copy_Number(va_arg(arg, uint32_t *), &str[i], (*formats == 'd'? 10: 8));
						break;

					default:
						break;
				}

				//remove the part of the string which is already stored in the arguments
				while(str[i] != ' ')
				{
					i++;
				}
				i++;
			}
		}
	}

	va_end(arg);
    free(str);

	return 0;
}


/**
  * @name   main()
  * @brief  to test the print() function!
  * @param  None!
  * @note   -
  * @retval None
  */
/*int main()
{
	int i;

	printf("Enter the value for i:\n");
	scan("%d", &i);

	printf("value of you have entered is i = %d %x\n", i, i);

	return 0;
}
*/
