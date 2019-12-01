/** \file
 *
 * STM32F30x polling UART driver
 */
 
#include "stm32f30x.h"
 
void UART1_Init(uint32_t baud);

void UART1_Open(void);

uint8_t UART1_Read(void);

void UART1_Write(uint8_t byte);

void UART1_Close(void);

void UART1_Deinit(void);
