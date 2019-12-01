#include "main.h"

void main(void)
{
	UART1_Init(UART_BAUDRATE);
	UART1_Open();
	
	// write welcome message
	uint8_t str[] = {"\n\rtype '~' for UART closing\n\r"};
	
	for (uint8_t i = 0; str[i] != 0; i++) {
		UART1_Write(str[i]);
	}
	
	// main loop
	while(1) {
		//receive polling
		uint8_t ch = UART1_Read();
		switch(ch) {
			case '~':
				UART1_Close();
				UART1_Deinit();
			break;
			
			case '\r':			// if received Enter
				UART1_Write('\n');
			
			default:
				UART1_Write(ch);
		}
	}
}

