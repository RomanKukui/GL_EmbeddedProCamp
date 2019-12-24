/** \file
 * \brief	STM32F30x USART1 CMSIS based polling UART driver .
 */
 
#include "stm32f30x.h"

/** \brief 	Initialise USART1 & RX/TX GPIO in STM32F303.
 *
 * Initialise USART1 in asynchronouos mode 
 * and RX/TX GPIO's alternative functions.
 *
 * \param[in]	baud	UART baudrate (9600, 115200, ...)
 */
void UART1_Init(uint32_t baud);

/** \brief 	Enable UART1 and its receiver & transmitter
 *
 * \pre		Before must be called UART1_Init(), for USART1 proper config.
 */
void UART1_Open(void);

/** \brief	Read USART1 received data in polling mode
 *
 * \return	Received data byte.
 *
 * \warning	Used polling mode (execution freez here 
 *		untill data received or error).
 */
uint8_t UART1_Read(void);

/** \brief	Sent data across USART1 in async mode.
 *
 * \param[in]	byte	Data, that will be sended.
 *
 * \note	Waiting untill prewious data will be transfered 
 *		(check untill TDR becomes ready to receive new data)
 */
void UART1_Write(uint8_t byte);

/** \brief	Disable USART1 with it's receiver and transmitter.
 */
void UART1_Close(void);

/** \brief	Reset USART1 and RX/TX GPIO's settings to its default values.
 */
void UART1_Deinit(void);
