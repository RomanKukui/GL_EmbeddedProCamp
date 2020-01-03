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
#include "string.h"
#include "stdio.h"
#include "usart.h"

/// B1 button pressed input value.
#define PRESSED_B1_VAL		1

/// Number of button presses before task creating. Range: 1 ~ 128.
#define TAP_TO_DEL		2

/// Number of button presses before task deleting. Range: 1 ~ 127.
#define TAP_TO_CREATE		3

/// Number of blinks for dummy tasks prio changes
#define BLINK_TO_CHANGE_PRIO	5


/// Blink task handler.
TaskHandle_t blink_task_h;

/// Blinks counter (increments every blink by blink task).
uint8_t blink_cnt;


/// Button task handler.
TaskHandle_t button_task_h;

/// Button task control structure.
StaticTask_t button_tcs;

/// Button task stack array.
StackType_t button_stack[configMINIMAL_STACK_SIZE];


/// Dummy LD8-on task handler.
TaskHandle_t dummy_ld8_on_h;

/// Dummy LD8-off task handler.
TaskHandle_t dummy_ld8_off_h;


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
	
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
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
}

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/** \brief	Print to UART1 connected console terminal free heap size in bytes.
 *
 * \param[in]	*in_msg	Suffix string pointer. 
 *		This string will be added before free heap info.
 *
 * \warning	This function use HAL UART \b polling API/transmit method.
 */
void print_free_heap(char *in_msg)
{
	uint8_t msg[50];
	uint8_t msg_size = 0;
	size_t free_heap = 0;
	
	free_heap = xPortGetFreeHeapSize();
	
	size_t in_msg_size;
	in_msg_size = strlen(in_msg);
	
	strcpy((char*)msg, in_msg);
	
	sprintf((char*)(msg + in_msg_size), " free heap: \t%d bytes\n\r", free_heap);
	msg_size = strlen((char*)msg);
	
	HAL_UART_Transmit(&huart1, msg, msg_size, HAL_MAX_DELAY);
}

/** \brief	LD10 blinking task.
 *
 * \param[in,out]	*param Default FreeRTOS task parameter.
 */
void blink_task(void *param)
{	
	while(1) {
		HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		vTaskDelay(250);
		
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
	print_free_heap("after blink task creation");
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
	print_free_heap("after blink task deletion");
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
	
	uint8_t msg[30];
	size_t msg_size = 0;
	
	// Total heap info
	sprintf((char*)msg, "Total heap: %d\n\r\n\r", configTOTAL_HEAP_SIZE);
	msg_size = strlen((char*)msg);
	HAL_UART_Transmit(&huart1, msg, msg_size, HAL_MAX_DELAY);
	
	print_free_heap("before tasks creation");
	
	start_blinking();
	
	button_task_h = xTaskCreateStatic(
			button_task,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			3,
			button_stack,
			&button_tcs
	);
	/// \todo task creation error processing skipped
	print_free_heap("after button task creation");

	xTaskCreate(
			dummy_task_ld8_on,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			0,
			&dummy_ld8_on_h
	);
	/// \todo task creation error processing skipped
	print_free_heap("after dummy ON task creation");
	
	xTaskCreate(	
			dummy_task_ld8_off,
			NULL,
			configMINIMAL_STACK_SIZE,
			NULL,
			0,
			&dummy_ld8_off_h
	);
	/// \todo task creation error processing skipped
	print_free_heap("after dummy OFF task creation");
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
