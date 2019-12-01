#include "uart.h"

#define UART_TX_PIN	4
#define UART_RX_PIN	5

#define UART_PORT	GPIOC

void UART1_Init(uint32_t baud)
{	
	/* ********* Pins Init ********* */
	
	// enable GPIO clock (on AHB)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	// set UART TX & RX pins mode = alternative function ('10')
	UART_PORT->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5);	// clear
	UART_PORT->MODER |= 0x02 << (UART_TX_PIN * 2);
	UART_PORT->MODER |= 0x02 << (UART_RX_PIN * 2);
	
	// set UART TX & RX pins alternative func = #7
	UART_PORT->AFR[0] &= ~(GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5);	// clear
	UART_PORT->AFR[0] |= (0x07 << (UART_TX_PIN * 4));
	UART_PORT->AFR[0] |= (0x07 << (UART_RX_PIN * 4));
	
	// set UART TX & RX pins frequency = hight ('11')
	UART_PORT->OSPEEDR |= (0x03 << UART_TX_PIN) | (0x03 << UART_RX_PIN);
	
	
	/* ********* UART Init ********* */
	
	// enable USART1 clock (on APB2)
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// set UART baudrate, USART_DIV = FCK / BAUD
	USART1->BRR = (uint16_t)(SystemCoreClock / baud);
	
	// MCU default: set number of data bits = 8 ('00')
//	USART1->CR1 &= ~(USART_CR1_M_0 | USART_CR1_M_1);
	
	// MCU default: set number of stop bits = 1 stop bit ('00')
//	USART1->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
	
	// MCU default: set oversampling = 16 samples ('0')
//	USART2->CR1 &= ~(USART_CR1_OVER8);
	
}

void UART1_Open(void)
{
	// enable USART & TX/RX
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
	
	// wait until idle preamble frame was transmitted
	while ((USART1->ISR & USART_ISR_TXE) == 0) {
		/// \todo add timeout checking
	}
}

uint8_t UART1_Read(void)
{
	// wait untill received data becomes ready to read from USART_RDR
	while ((USART1->ISR & USART_ISR_RXNE) == 0) {
		/// \todo add timeout checking
	}
	
	return (uint8_t)USART1->RDR;
	/// \todo also can return USART ISR register content, for status control
}

void UART1_Write(uint8_t byte)
{
	// wait until TDR become ready to receive new data
	while ((USART1->ISR & USART_ISR_TXE) == 0) {
		/// \todo add timeout checking
	}
	
	USART1->TDR = byte;
	
	// clear TXE flag
	USART1->RQR = USART_RQR_TXFRQ;
}

void UART1_Close(void)
{
	// wait until last data byte was transmitted
	while ((USART1->ISR & USART_ISR_TXE) == 0) {
		/// \todo add timeout checking
	}
	
	// disable USART & TX/RX
	USART1->CR1 &= ~(USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
}

void UART1_Deinit(void)
{	
	/* ********* UART Deinit ********* */
	
	// set UART baudrate to 0
	USART1->BRR = 0;
	
	// set MCU default number of data bits = 8 ('00')
	USART1->CR1 &= ~(USART_CR1_M_0 | USART_CR1_M_1);
	
	// set MCU default number of stop bits = 1 stop bit ('00')
	USART1->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
	
	// set MCU default oversampling = 16 samples ('0')
	USART2->CR1 &= ~(USART_CR1_OVER8);
	
	// disable USART1 clock (on APB2)
	RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
	
	
	/* ********* Pins Deinit ********* */
	
	// set UART TX & RX pins mode = Input
	UART_PORT->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
	
	// clear UART TX & RX pins alternative func
	UART_PORT->AFR[0] &= ~(GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5);
	
	// clear UART TX & RX pins frequency
	UART_PORT->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5);
}
