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
#include "usart.h"

/// UART1 mutex handler
SemaphoreHandle_t uart1_mutex_h;

/// Resource 1 mutex handler
SemaphoreHandle_t LD4_mutex_h;

/// Resource 2 mutex handler
SemaphoreHandle_t LD5_mutex_h;

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

/** \brief	Blink LD3 with 2s blink period.
 */
void blink_LD3(void);

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


/** \brief	Task 1 source code.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void task_1_main(void *param)
{
while(1) {
	send_to_uart("task 1\tstart iteration\n\r");
	
	HAL_Delay(1000);		// some polling code
	
	// try to get LD4 mutex, wait for 5 systick's
	if (xSemaphoreTake(LD4_mutex_h, 5) == pdTRUE) {
		
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		send_to_uart("task 1\tLD 4\tmutex taken\n\r");
		
		vTaskDelay(1000);	// other task suspending code
		
//		// *** artificial timing tune for livelock :)
//		while(xSemaphoreTake(LD5_mutex_h, 0) == pdTRUE) {
//			xSemaphoreGive(LD5_mutex_h);
//		}
		
		// try to get LD5 mutex for compleating operation
		if (xSemaphoreTake(LD5_mutex_h, 5) == pdTRUE) {
			
			// access to both resources
			send_to_uart("task 1\tLD 5\tmutex taken\n\r");
			
			// compleate operation
			blink_LD3();
			send_to_uart("task 1\t!!! SUCCESS !!!\n\r");
			
			// release LD5 mutex
			xSemaphoreGive(LD5_mutex_h);
			send_to_uart("task 1\tLD 5\tmutex released\n\r");

		} else {
			send_to_uart("task 1\tLD 5\tmutex take error\n\r");
			// release LD4 mutex and skip iteration
		}
		vTaskDelay(1000);	// other task suspending code
		
		// release LD4 mutex
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		xSemaphoreGive(LD4_mutex_h);
		send_to_uart("task 1\tLD 4\tmutex released\n\r");			
	} else {
		send_to_uart("task 1\tLD 4\tmutex take error\n\r");
	}
}
}

/** \brief	Task 2 source code.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void task_2_main(void *param)
{
while(1) {
	send_to_uart("task 2\tstart iteration\n\r");
	
	HAL_Delay(1000);		// some polling code
	
	// try to take LD5 mutex
	if (xSemaphoreTake(LD5_mutex_h, 5) == pdTRUE) {
		
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
		send_to_uart("task 2\tLD 5\tmutex taken\n\r");
		
		vTaskDelay(1200);	// other task suspending code
		
//		// *** artificial timing tune for livelock :)
//		while(xSemaphoreTake(LD4_mutex_h, 0) == pdTRUE) {
//			xSemaphoreGive(LD4_mutex_h);
//		}
		
		// try to get LD4 mutex for compleating operation
		if (xSemaphoreTake(LD4_mutex_h, 5) == pdTRUE) {
			
			// have both mutexe's
			send_to_uart("task 2\tLD 4\tmutex taken\n\r");
			
			// compleate operation
			blink_LD3();
			send_to_uart("task 2\t!!! SUCCESS !!!\n\r");
			
			// release LD4 mutex
			xSemaphoreGive(LD4_mutex_h);
		} else {
			send_to_uart("task 2\tLD 4\tmutex take error\n\r");
			// release LD5 mutex and skip iteration
		}
		vTaskDelay(800);	// other task suspending code
		
		// release LD5 mutex
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		xSemaphoreGive(LD5_mutex_h);
		send_to_uart("task 2\tLD 5\tmutex released\n\r");
	} else {
		send_to_uart("task 2\tLD 5\tmutex take error\n\r");
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
	
	LD4_mutex_h = xSemaphoreCreateMutex();
	if (LD4_mutex_h == NULL) {
		/// \todo Handle semaphore creation error
	}
	
	LD5_mutex_h = xSemaphoreCreateMutex();
	if (LD5_mutex_h == NULL) {
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
		// cannot access source catch
		while(1);
	}
}

void blink_LD3(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	vTaskDelay(1000);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
