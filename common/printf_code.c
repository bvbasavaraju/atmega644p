/**
  ******************************************************************************
  * @file    printf_code.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    06-Dec-2013
  * @brief   This file is having the printf() equivalent function!
  * 		 This is simplified printf(). this can be expanded for the different
  *			 data types!
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "printf_code.h"

/* Function Definations ------------------------------------------------------*/

/**
  * @name   display_Character()
  * @brief  this function will print the char
  * @param  ch - character needs to be printed!
  * @note	Here instead of putchar() use the USART/UART put function written by urself!
  * @retval None
  */
void display_Character(char ch)
{
	USART_PutChar(ch);
	//putchar(ch);
}

/**
  * @name   print_String()
  * @brief  this function will print the string
  * @param  *str - the pointer to the string needs to be printed!
  * @note	-
  * @retval None
  */
void print_String(const char *str)
{
	while(*str != '\0')
	{
		display_Character(*str);
		str++;
	}
}

/**
  * @name   print_Character()
  * @brief  this function will print the character only!
  * @param  data - charater needs to be printed!
  * @note	-
  * @retval None
  */
void print_Character(const char data)
{
	display_Character(data);
}

/**
  * @name   print_Integer()
  * @brief  this function will print the numbers only!
  * @param  data - number needs to be printed!
  * @param  length - lenght of number needs to be printed! bacisally Decimal meaning max lenght is 10 decimanls (assuming 32 bit computer)
  * @note	right now it will be printing decimal numbers from -2147483648 to 2147483647
  * 		needs some modification for hex numbers and unsigned values!
  * @retval None
  */
void print_Integer(const int32_t data, int length)
{
	char *s;
	int i;
	uint32_t val = 0;

	//Initialize the variables!
	i = 0;
	s = (char *)(calloc((length+1), sizeof(char)));
    if(data != 0)
    {
        if(length == 10)
        {
            val = (data<0)? (data * -1): data;
            while(val != 0 && i<length)
            {
                s[i] = (val % 10) + '0';
                val/=10;
                i++;
            }
        }
        else
        {
            val = data;

            while(val != 0 && i<length)
            {
                //When nibble is >= 0x0A, you have to subtract 10 (or 0x0A) becuase:
                //assume nibble is 0x0A, when oyu add char 'a'(decimal 97) its ascii value will be 107(in decimal)
                //But you want yo display char 'a' not char 'k'(decimal 107). By subtracting 10 (0x0A) you will get 'a'
                s[i] = (val & 0x0F) >=0x0A? ((val & 0x0F) + 'a') - 10 : (val & 0x0F) + '0';
                val = val >> 4;
                i++;

            }
        }
    }
    else
    {
        s[i] = data + '0';
        i++;
    }

	s[i]='\0';

	if(data<0 && length > 8)
	{
		display_Character('-');
	}

	i= i - 1; //as i represents the number of digits
	while(i >= 0)
	{
		display_Character(s[i]);
		i--;
	}

	free(s);
}


/**
  * @name   print()
  * @brief  this function will behave similar to printf but the fully functional printf, a partial printf function!
  * @param  *str - pointer to the string which needs to be analysed and printed!
  * @param  ... - unknown number of arguments!
  * @note	for printing unsinged numbers, hex values and long values needs modification!
  *         For integers typecast the number by (uint32_t) or (int32_t) to print the proper value
  * @retval None
  */
void print(const char *str, ...)
{
	va_list arg_list;

	va_start(arg_list, str);
	while(*str != '\0')
	{
		switch(*str)
		{
			case '%':
						str++;
						if(*str != '%')
						{
							switch(*str)
							{
								case 'd':	//print the Integers!
								case 'x':
								case 'X':
											//assuming 10 digits is max for Decimal format(2^32) and 8 in hexadecimal - 32 bit machine!
											//Note: These int or uint variable must have postfixed with _t, like int8_t or uint8_t
											//      Else the print values may be different from what has been passed!
											//      if _t is used then size is always fixed to those many bits!
											print_Integer(va_arg(arg_list, const int32_t), (*str=='d'? 10: 8));
										break;
								case 'c':
											print_Character(va_arg(arg_list, const int));
										break;
								case 's':
											print_String(va_arg(arg_list, const char *));
										break;

								default:
										display_Character(*str);
										break;
							}
						}
						else
						{
							//Here 2 times % symbol is invalid (you cannot use %%)
							print("Error Error Error! cannot print becasue format specifier is entered twice!");
						}

					break;

			default:
						display_Character(*str);
					break;
		}
		str++;
	}
	va_end(arg_list);
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
	i = 567638789;
	printf("I am trying to duplicate the printf code\n");
	print("I am inside a print code where printf is duplicated! - Bug\n");
	print("This is for Integer %d\n", i);
	print("This is for Character %c\n", 'C');
	print("%s", "This is for String test!\n");
	print("Trying to find the Hex number: %x", 0xFFFFFFFF);
	print("\n%d", -2147483647);

	return 0;
}
*/
