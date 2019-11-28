#include "main.h"

#define UART_BAUDRATE	((uint16_t)9600)

#define UART_TX_PIN	(2)

int main(void)
{
	//SystemInit();
	
	
// === Clock Init ===
	// enable USART2 clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// enable GPIO clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
	
	
// === Pin Init ===
	// enable LED round
	GPIOE->MODER |= 0x55550000;	// to out
	
	// PA2 mode -> clear
	//GPIOA->MODER &= ~GPIO_MODER_MODER2;
	// PA2 mode -> Alternate (10)
	GPIOA->MODER |= (0x02 << (UART_TX_PIN * 2));
	
	// PA2 -> push-pull (0)
	//GPIOA->OTYPER &= ~GPIO_OTYPER_OT_2;
	
	// PA2 pull-up and pull-down -> disable (00)
	// pull-up
	GPIOA->PUPDR |= (1 << (UART_TX_PIN * 2));
	
	// PA2 frequency -> hight (11)
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
	
	// PA2 alternative func number -> clear
	//GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL2;
	// set PA2 alternative func #7 -> USART2_TX
	GPIOA->AFR[0] |= (0x07 << (UART_TX_PIN * 4));
	
	
// === USART Init ===
	// M[1:0] = 00 - 8 data bits
	//USART2->CR1 &= ~(USART_CR1_M_0 | USART_CR1_M_1);
	
	// USART_DIV = FCK / BAUD
	USART2->BRR = SystemCoreClock / UART_BAUDRATE;
	 
	// STOP[1:0] = 00 - 1 stop bit
	//USART2->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
	
	// 16 samples oversampling
	//USART2->CR1 &= ~(USART_CR1_OVER8);
	
	// enable USART & TX/RX
	USART2->CR1 |= USART_CR1_TE;
	USART2->CR1 |= USART_CR1_UE;
	
// === Main Loop ===
	while(1) {
		for (uint32_t i = 0; i < 300000; i++) {
		}
		
		// wait untill transmission complete
		GPIOE->ODR |= (1 << 8);			// on LED8
		
		while ((USART2->ISR & USART_ISR_TXE) == 0) {
		}
		
		while ((USART2->ISR & USART_ISR_TC) == 0) {
		}
		
		GPIOE->ODR &= ~(1 << 8);		// off LED8
		
		// send test data
		USART2->TDR = '8';
		
		// clear TXE flag
		USART2->ISR = 0;
	};
	
	return 0;
}
