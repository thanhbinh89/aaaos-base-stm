#include <stdio.h>
#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf
(option LD Linker->Libraries->Small printf set to 'Yes')
calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int _write(int file, char *data, int len)
#endif
/* __GNUC__ */

/**
* @brief Retargets the C library printf function to the USART.
* @param None
* @retval None
*/
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	// arbitrary timeout 1000
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, (uint8_t*)data, len, 0xFFFFFFFFU);
	// return # of bytes written - as best we can tell
	return (status == HAL_OK ? len : 0);
}
