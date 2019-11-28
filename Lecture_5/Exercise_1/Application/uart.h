/** \file
 *
 * STM32F30x polling UART driver
 */
 
#include "stm32f30x.h"
 
uint8_t UART_Init(USART_TypeDef *uart);

uint8_t UART_Open(void);

uint8_t UART_Read(void);

uint8_t UART_Write(uint8_t byte);

uint8_t UART_Close(void);

uint8_t UART_Deinit(void);
