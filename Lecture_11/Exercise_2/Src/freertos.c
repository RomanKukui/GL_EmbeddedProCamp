/** \file
 *
 * Tasks:
 * 2) Create program with using queue
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "gpio.h"
#include "string.h"
#include "usart.h"

/// UART1 mutex handler
SemaphoreHandle_t uart1_mh;

/// Uart1 queue handler
QueueHandle_t uart1_qh;

/// Uart1 queue size
#define UART1_QUEUE_SIZE	64


/// button_to_led queue handler
QueueHandle_t button_to_led_qh;

/// button_to_led queue size
#define EVENT_QUEUE_SIZE	10


/// Event identificator
#define EVENT_BLINK	'1'

/// Event code encapsulation typedef
typedef uint8_t EventCode;


/* Private function prototypes -----------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/** \brief	Send string to UART1.
 *
 * Used for debug pusporces. Use HAL UART interrupt mode and mutex.
 *
 * \param[in]	*str Pointer to string, that will be sended by UART1.
 */
void send_to_uart(char *str);

/** \brief	Post event id to queue (to led engine).
 *
 * \param[in]	event EventCode, that will be posted to queue.
 */
void post_event(EventCode event);

/** \brief	Process received events id's. LD5 blink every EVENT_BLINK.
 *
 * \param	event EventCode for processing
 */
void process_events(EventCode event);

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/** \brief	Read B1 keyDown and sent it to LED driver task across Queue.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void button_engine_main(void *param)
{
	uint8_t b1_kdown_f = 0;		// key down event flag
	BaseType_t b1_state;
	
	while(1) {		
		b1_state = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
		if (b1_state == GPIO_PIN_SET) {
			if (b1_kdown_f == 1) {
				// key still pressed, skip iteration
				continue;
			}
			
			vTaskDelay(60);		// debouncing
			
			b1_state = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
			if (b1_state == GPIO_PIN_SET) {
				// button pressed, add event to queue
				post_event(EVENT_BLINK);
				
				b1_kdown_f = 1;
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			}
		} else {
			b1_kdown_f = 0;
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		}
		vTaskDelay(1);	// sleep to next systick
	}
}

/** \brief	LED driving engine task source code.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void led_engine_main(void *param)
{
	BaseType_t q_state;
	EventCode event;
	
	while(1) {
		if (uxQueueMessagesWaiting(button_to_led_qh) > 0) {
			q_state = xQueueReceive(button_to_led_qh, &event, 0);
			
			if (q_state == pdPASS) {
				process_events(event);
			}
		} else {
			vTaskDelay(1);		// sleep to next systick
		}
	}
}

/** \brief	Send data to uart.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void uart_tx_main(void *param)
{
	BaseType_t tmp_retval;
	uint8_t ch;
	
	while(1) {
		
		if (uxQueueMessagesWaiting(uart1_qh) > 0) {
			tmp_retval = xQueueReceive(uart1_qh, &ch, 0);
			if (tmp_retval == pdPASS) {
				HAL_UART_Transmit(&huart1, &ch, 1, HAL_MAX_DELAY);
			} else {
				/// \todo Receive error handler.
			}
		} else {
			vTaskDelay(1);		// sleep to next systick
		}
	}
}

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* add mutexes, ... */
	
	uart1_mh = xSemaphoreCreateMutex();
	if (uart1_mh == NULL) {
		/// \todo Handle semaphore creation error
	}

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

  /* add queues, ... */
	
	button_to_led_qh = xQueueCreate(EVENT_QUEUE_SIZE, sizeof(EventCode));
	if (button_to_led_qh == NULL) {
		/// \todo Handle queue creation error.
	}
	
	uart1_qh = xQueueCreate(UART1_QUEUE_SIZE, sizeof(uint8_t));
	if (uart1_qh == NULL) {
		/// \todo Handle queue creation error.
	}

  /* Create the thread(s) */
	
	BaseType_t task_ret;
	
	task_ret = xTaskCreate(
			button_engine_main,
			NULL, 
			configMINIMAL_STACK_SIZE, 
			NULL,
			2, 
			NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
	}
	
	task_ret = xTaskCreate(
			led_engine_main,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			2,
			NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
	}
	
	task_ret = xTaskCreate(
			uart_tx_main,
			NULL,
			configMINIMAL_STACK_SIZE+128,
			NULL,
			1,
			NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
	}
	
	send_to_uart("Initialisation compleated\n\r");
}

void send_to_uart(char *str)
{
	size_t str_size = strlen(str);
		
	if (str_size <= UART1_QUEUE_SIZE) {
		
		if (xSemaphoreTake(uart1_mh, 200) == pdTRUE) {
			for (uint8_t i = 0; i < str_size; i++) {
				if (xQueueSend(uart1_qh, (str+i), 10) != pdPASS) {
					/// \todo Handle add to queue error.
				}
			}
//			HAL_UART_Transmit(&huart1, (uint8_t *)str, str_size, HAL_MAX_DELAY);
		} else {
			/// \todo Can not access source catch handler.
		}
		xSemaphoreGive(uart1_mh);
	} else {
		/// \todo Message too long error handler.
	}
}

void post_event(EventCode event)
{
	BaseType_t stat;
	stat = xQueueSend(button_to_led_qh, &event, 10);
	
	if(stat != pdPASS) {
		// error adding to queue
		send_to_uart("error adding to queue\n\r");
		
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		vTaskDelay(1000);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	}
}

void process_events(EventCode event)
{
	switch(event)
	{
	case EVENT_BLINK:
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
		vTaskDelay(200);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		vTaskDelay(200);
	break;
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
