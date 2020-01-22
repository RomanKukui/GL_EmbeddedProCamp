/** \file
 *
 * Tasks:
 * 1) Create program with livelock
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "gpio.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

/// UART1 mutex handler
SemaphoreHandle_t uart1_mutex_h;

/// Virtual resource 1 mutex handler
SemaphoreHandle_t var1_mutex_h;

/// Virtual resource 2 mutex handler
SemaphoreHandle_t var2_mutex_h;

/// Task1 handler
TaskHandle_t task_1_h;

/// Task2 handler
TaskHandle_t task_2_h;


/* Private function prototypes -----------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/** \brief	Send string to UART1.
 *
 * Used for debug pusporces. Use HAL UART polling mode.
 *
 * \param[in]	*str Pointer to string, that will be sended by UART1.
 */
void send_to_uart(char *str);

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



void task_1_main(void *param)
{
	while(1) {
		send_to_uart("task 1\tstart iteration\n\r");
		
		HAL_Delay(1000);		// some polling code
		
		// try lock resource 1, wait for 5 systick
		if (xSemaphoreTake(var1_mutex_h, 5) == pdTRUE) {
			send_to_uart("task 1\tsource 1\tmutex taked\n\r");
			
			vTaskDelay(1000);	// other task suspending code
			
			// try to lock source 2 for compleating operation
			if (xSemaphoreTake(var2_mutex_h, 5) == pdTRUE) {
				send_to_uart("task 1\tsource 2\tmutex taked\n\r");
				send_to_uart("task 1\t!!! SUCCESS !!!\n\r");
				
				// release source 2 mutex
				if (xSemaphoreGive(var2_mutex_h) == pdTRUE) {
					send_to_uart("task 1\tsource 2\tmutex released\n\r");
				} else {
					send_to_uart("task 1\tsource 2\tmutex release error\n\r");
				}
			} else {
				// release source 1 mutex and skip iteration
				send_to_uart("task 1\tsource 2\tmutex take error\n\r");
			}
			
			// release source 1 mutex
			if (xSemaphoreGive(var1_mutex_h) == pdTRUE) {
				send_to_uart("task 1\tsource 1\tmutex released\n\r");
			} else {
				send_to_uart("task 1\tsource 1\tmutex release error\n\r");
			}
				
		} else {
			send_to_uart("task 1\tsource 1\tmutex take error\n\r");
		}

	}
}

void task_2_main(void *param)
{
	while(1) {
		send_to_uart("task 2\tstart iteration\n\r");
		
		HAL_Delay(1000);		// some polling code
		
		// try to lock resource 2
		if (xSemaphoreTake(var2_mutex_h, 5) == pdTRUE) {
			send_to_uart("task 2\tsource 2\tmutex taked\n\r");
			
			vTaskDelay(1000);	// other task suspending code
			
			// try to lock source 1 for compleating operation
			if (xSemaphoreTake(var1_mutex_h, 5) == pdTRUE) {
				send_to_uart("task 2\tsource 1\tmutex taked\n\r");
				send_to_uart("task 2\t!!! SUCCESS !!!\n\r");
			} else {
				// release source 2 mutex and skip iteration
				send_to_uart("task 2\tsource 1\tmutex take error\n\r");
			}
			
			// release resource 2 mutex
			if (xSemaphoreGive(var2_mutex_h) == pdTRUE) {
				send_to_uart("task 2\tsource 2\tmutex released\n\r");
			} else {
				send_to_uart("task 2\tsource 2\tmutex release error\n\r");
			}
		} else {
			send_to_uart("task 2\tsource 2\tmutex take error\n\r");
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
	
	uart1_mutex_h = xSemaphoreCreateMutex();
	if (uart1_mutex_h == NULL) {
		/// \todo Handle semaphore creation error
	}
	
	var1_mutex_h = xSemaphoreCreateMutex();
	if (var1_mutex_h == NULL) {
		/// \todo Handle semaphore creation error
	}
	
	var2_mutex_h = xSemaphoreCreateMutex();
	if (var2_mutex_h == NULL) {
		/// \todo Handle semaphore creation error
	}

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

  /* add queues, ... */

  /* Create the thread(s) */
	BaseType_t task_creation;
	
	task_creation = xTaskCreate(
			task_1_main,
			NULL, 
			configMINIMAL_STACK_SIZE, 
			NULL,
			2, 
			&task_1_h
	);
	if (task_creation != pdPASS) {
		/// \todo Handle task creation error.
	} else {
		// task 1 created successfully
		send_to_uart("task 1\tcreated\n\r");
	}
	
	task_creation = xTaskCreate(
			task_2_main,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			2,
			&task_2_h
	);
	if (task_creation != pdPASS) {
		/// \todo Handle task creation error.
	} else {
		// task 2 created successfully
		send_to_uart("task 2\tcreated\n\r");
	}
}

void send_to_uart(char *str)
{
	if (xSemaphoreTake(uart1_mutex_h, 100) == pdTRUE) {
		size_t str_size = strlen(str);
		HAL_UART_Transmit(&huart1, (uint8_t *)str, str_size, HAL_MAX_DELAY);
		
		xSemaphoreGive(uart1_mutex_h);
	} else {
		// cannot access source
//		while(1);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
