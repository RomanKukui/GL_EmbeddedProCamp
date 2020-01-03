/** \file
 *
 * Tasks:
 * 1) Create tasks and delete
 * 2) Assign task priority change priority
 * 3) Create idle task hook 
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "gpio.h"

/// B1 button pressed input value.
#define PRESSED_B1_VAL		1

/// Number of button presses before task creating. Range: 1 ~ 128.
#define TAP_TO_DEL		2

/// Number of button presses before task deleting. Range: 1 ~ 127.
#define TAP_TO_CREATE		3

/// Number of blinks for dummy tasks prio changes
#define BLINK_TO_CHANGE_PRIO	5

/// Blinking task handler.
TaskHandle_t blink_task_h;

/// Dummy LD8-on task handler.
TaskHandle_t dummy_ld8_on_h;

/// Dummy LD8-off task handler.
TaskHandle_t dummy_ld8_off_h;

/// Blinks counter (increments every blink by blink task).
uint8_t blink_cnt;

/* Private function prototypes -----------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
	
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
}

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;

}

/** \brief	LD10 blinking task.
 *
 * \param[in,out]	*param Default FreeRTOS task parameter.
 */
void blink_task(void *param)
{	
//	TickType_t last_wake_time;
//	last_wake_time = xTaskGetTickCount();
	
	while(1) {
		HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		vTaskDelay(250);
//		vTaskDelayUntil(&last_wake_time, 250);
		
		blink_cnt++;		
	}
}

/** \brief	Create blinking task and check creation result.
 */
void start_blinking(void)
{
	blink_cnt = 0;
	
	BaseType_t create_res;
	create_res = xTaskCreate(	
		blink_task, 
		NULL, 
		configMINIMAL_STACK_SIZE, 
		NULL, 
		2, 
		&blink_task_h
	);
	
	if (create_res != pdPASS) {
		/// \todo Error handling
	}
}

/** \brief	Delete blinking task and turn off LD10.
 *
 * Before deletion task handler will be checked.
 */
void stop_blinking(void)
{
	if (blink_task_h != NULL) {
		vTaskDelete(blink_task_h);
		blink_task_h = NULL;
		
		HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
	}
}

/** \brief	Button press event handler.
 */
void btn_pressed(void) 
{
//	vTaskSuspend(blink_task_handler);
	
	static int8_t tsk_create_cnt;
	
	if (blink_task_h != NULL) {
		tsk_create_cnt--;
		
		if (tsk_create_cnt == -TAP_TO_DEL) {
			stop_blinking();		// delete blink task
			tsk_create_cnt = 0;
		}
	} else {
		tsk_create_cnt++;
		
		if (tsk_create_cnt == TAP_TO_CREATE) {
			start_blinking();		// create blink task
			tsk_create_cnt = 0;
		}
	}
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}

/** \brief	Button B1 release event handler.
 */
void btn_released(void)
{
//	vTaskResume(blink_task_handler);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}

/** \brief	B1 key-press seeking task.
 *
 * \param[in,out]	*param Default FreeRTOS task parameter.
 */
void button_task(void *param)
{		
	uint8_t btn_pressed_f = 0;
	
	while(1) {		
		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == PRESSED_B1_VAL) {
			if (btn_pressed_f == 0) {
				vTaskDelay(60);		// debouncing delay
				
				if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == PRESSED_B1_VAL) {
					btn_pressed_f = 1;
					btn_pressed();
				}
			}
		} else {
			if (btn_pressed_f == 1) {
				btn_released();
				
				btn_pressed_f = 0;
			}
		}
		vTaskDelay(200);	// period between button status reading
	}
}

/** \brief	Dummy task, that ON LD8 in infinite loop.
 *
 * \param[in,out]	*param Default FreeRTOS task parameter.
 */
void dummy_task_ld8_on(void *param)
{
	while (1) {
		HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_SET);
		
		if (blink_cnt == BLINK_TO_CHANGE_PRIO) {
			blink_cnt = 0;
			
			vTaskPrioritySet(NULL, 0);
			vTaskPrioritySet(dummy_ld8_off_h, 1);		
		}
	}
}

/** \brief	Dummy task, that OFF LD8 in infinite loop.
 *
 * \param[in,out]	*param Default FreeRTOS task parameter.
 */
void dummy_task_ld8_off(void *param)
{
	while (1) {
		HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
		
		if (blink_cnt == BLINK_TO_CHANGE_PRIO) {
			blink_cnt = 0;
			
			vTaskPrioritySet(NULL, 0);
			vTaskPrioritySet(dummy_ld8_on_h, 1);
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

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

  /* add queues, ... */

  /* Create the thread(s) */
	start_blinking();
	
	xTaskCreate(	button_task,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			3,
			NULL
	);
	/// \todo task creation error processing skipped
	
	xTaskCreate(
			dummy_task_ld8_on,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			0,
			&dummy_ld8_on_h
	);
	/// \todo task creation error processing skipped
	
	xTaskCreate(	
			dummy_task_ld8_off,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			0,
			&dummy_ld8_off_h
	);
	/// \todo task creation error processing skipped
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
