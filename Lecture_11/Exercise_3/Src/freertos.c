/** \file
 *
 * Tasks:
 * 3) Create program “The Dining Philosophers”
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "gpio.h"
#include "string.h"
#include "usart.h"
#include "time.h"
#include "stdlib.h"

/// UART1 mutex handler
SemaphoreHandle_t uart1_mh;

/// Uart1 queue handler
QueueHandle_t uart1_qh;

/// Uart1 queue size
#define UART1_QUEUE_SIZE	64


/// Number of philosophers
#define PH_NUM			4

/// Fork #0-#3 mutexes handler
SemaphoreHandle_t fork_mh[PH_NUM];

/// Time for second fork waiting, in ms
#define WAIT_FOR_FORK_T		5000


/// Struct, that hold LED settings.
struct _led_representation {
	GPIO_TypeDef *port;		///< LED Port
	BaseType_t pin;			///< LED Pin
};

/// Encapsulate LED settings struct.
typedef struct _led_representation led_t;

/// LED settings, for using of forks visualisation.
led_t fork_led[PH_NUM];

/// LED long blink period.
#define LONG_BLINK_T		200


/// Philosopher control struct.
struct _philosopher_control_struct {
	/// Philosopher LED config.
	led_t led;
	
	/// Fork ID, that will be taken first mutex handler.
	uint8_t fork1_id;
	
	/// Fork ID, that will be taken second mutex handler.
	uint8_t fork2_id;
	
	/// NUmber of chews.
	uint8_t chews;
};

/// Encapsulate Philosopher control struct.
typedef struct _philosopher_control_struct philosopher_t;

/// Philosophers config/control struct.
philosopher_t ph_cs[PH_NUM];

/* Private function prototypes -----------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/// Initialise LED's hardware settings for forks and philosophers.
void init_leds(void);

/** \brief	Switch ON LED. Wrapper for HAL.
 *
 * \param[in]	*led Pointer to LED config structure.
 */
void led_on(led_t *led);

/** \brief	Switch OFF LED. Wrapper for HAL.
 *
 * \param[in]	*led Pointer to LED config structure.
 */
void led_off(led_t *led);

/** \brief	Blink LED with \p period period \p blink_num times.
 *
 * \param[in]	*led Pointer to LED config structure.
 * \param	blinks_num Number of light blinks.
 * \param	period Period of blinking in ms.
 */
void led_blink(led_t *led, uint8_t blinks_num, uint32_t period);

/** \brief	Try to eat of philosopher (get forks and eating).
 *
 * \param[in]	id ID of philosopher_t.
 *
 * \retval	1 if eating success.
 * \retval	0 if eating fail.
 */
uint8_t try_to_eat(uint8_t id);

/** \brief	Send string to UART1.
 *
 * Used for debug pusporces. Use HAL UART interrupt mode and mutex.
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

/** \brief	Philosopher #1 task.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void ph0_main(void *param)
{	
	ph_cs[0].fork1_id = 0;
	ph_cs[0].fork2_id = 3;
	ph_cs[0].chews = 5;
	
	while(1) {
		// thinking
		vTaskDelay(500);
		
		// eating
		if (try_to_eat(0) == 0) {
			/// \todo Eating error.
			send_to_uart("ph0 skip eating\n\r");
		}
	}
}

/** \brief	Philosopher #2 task.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void ph1_main(void *param)
{
	ph_cs[1].fork1_id = 0;
	ph_cs[1].fork2_id = 1;
	ph_cs[1].chews = 4;
	
	while(1) {
		// thinking
		vTaskDelay(1000);
		
		// eating
		if (try_to_eat(1) == 0) {
			/// \todo Eating error.
			send_to_uart("ph1 skip eating\n\r");
		}
	}
}

/** \brief	Philosopher #3 task.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void ph2_main(void *param)
{
	ph_cs[2].fork1_id = 2;
	ph_cs[2].fork2_id = 1;
	ph_cs[2].chews = 3;
	
	while(1) {
		// thinking
		vTaskDelay(1500);

		// eating
		if (try_to_eat(2) == 0) {
			/// \todo Eating error.
			send_to_uart("ph2 skip eating\n\r");
		}
	}
}

/** \brief	Philosopher #4 task.
 * \param	*param Generic parameters, received from xTaskCreate().
 */
void ph3_main(void *param)
{
	ph_cs[3].fork1_id = 2;
	ph_cs[3].fork2_id = 3;
	ph_cs[3].chews = 2;
	
	while(1) {
		// thinking
		vTaskDelay(2000);
		
		// eating
		if (try_to_eat(3) == 0) {
			/// \todo Eating error.
			send_to_uart("ph3 skip eating\n\r");
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
	
	for (uint8_t i = 0; i < PH_NUM; i++) {
		fork_mh[i] = xSemaphoreCreateMutex();
		if (fork_mh[i] == NULL) {
			/// \todo Handle semaphore creation error
		}
	}
	

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

  /* add queues, ... */
	uart1_qh = xQueueCreate(UART1_QUEUE_SIZE, sizeof(uint8_t));
	if (uart1_qh == NULL) {
		/// \todo Handle queue creation error.
	}

  /* Create the thread(s) */
	
	BaseType_t task_ret;
	
	task_ret = xTaskCreate(
		uart_tx_main,
		NULL,
		(configMINIMAL_STACK_SIZE + 128),
		NULL,
		1,
		NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
	}
	
	task_ret = xTaskCreate(
		ph0_main,
		NULL,
		configMINIMAL_STACK_SIZE,
		NULL,
		2,
		NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
		while(1);
	}
	
	task_ret = xTaskCreate(
		ph1_main,
		NULL,
		configMINIMAL_STACK_SIZE,
		NULL,
		2,
		NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
		while(1);
	}
	
	task_ret = xTaskCreate(
		ph2_main,
		NULL,
		configMINIMAL_STACK_SIZE,
		NULL,
		2,
		NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
		while(1);
	}
	
	task_ret = xTaskCreate(
		ph3_main,
		NULL,
		configMINIMAL_STACK_SIZE,
		NULL,
		2,
		NULL
	);
	if (task_ret != pdPASS) {
		/// \todo Handle task creation error.
		while(1);
	}
	
	init_leds();
	
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
		} else {
			/// \todo Can not access source catch handler.
		}
		xSemaphoreGive(uart1_mh);
	} else {
		/// \todo Message too long error handler.
	}
}

void init_leds(void)
{
	fork_led[0].port = LD5_GPIO_Port;
	fork_led[0].pin = LD5_Pin;
	
	fork_led[1].port = LD9_GPIO_Port;
	fork_led[1].pin = LD9_Pin;
	
	fork_led[2].port = LD8_GPIO_Port;
	fork_led[2].pin = LD8_Pin;
	
	fork_led[3].port = LD4_GPIO_Port;
	fork_led[3].pin = LD4_Pin;
	
	
	ph_cs[0].led.port = LD3_GPIO_Port;
	ph_cs[0].led.pin = LD3_Pin;
	
	ph_cs[1].led.port = LD7_GPIO_Port;
	ph_cs[1].led.pin = LD7_Pin;
	
	ph_cs[2].led.port = LD10_GPIO_Port;
	ph_cs[2].led.pin = LD10_Pin;
	
	ph_cs[3].led.port = LD6_GPIO_Port;
	ph_cs[3].led.pin = LD6_Pin;
}

inline void led_on(led_t *led)
{
	HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
}

inline void led_off(led_t *led)
{
	HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
}

void led_blink(led_t *led, uint8_t blinks_num, uint32_t period)
{
	for (uint8_t i = 0; i < blinks_num; i++) {
		vTaskDelay(period/4);
		led_on(led);
		vTaskDelay(period/2);
		led_off(led);
		vTaskDelay(period/4);
	}
}

uint8_t try_to_eat(uint8_t id)
{
	philosopher_t *ph = &ph_cs[id];
	
	uint8_t ret_val = 0;
	
	// try to take first fork
	if (xSemaphoreTake(fork_mh[ph->fork1_id], 0) == pdPASS) {
		led_on(&fork_led[ph->fork1_id]);
		
		// try to take second fork
		if (xSemaphoreTake(fork_mh[ph->fork2_id], WAIT_FOR_FORK_T) == pdPASS) {
			led_on(&fork_led[ph->fork2_id]);
			
			// eating
			led_blink(&ph_cs[id].led, ph->chews, LONG_BLINK_T);
			
			ret_val = 1;
			
			// release second fork
			xSemaphoreGive(fork_mh[ph->fork2_id]);
			led_off(&fork_led[ph->fork2_id]);
		}
		// release first fork
		xSemaphoreGive(fork_mh[ph->fork1_id]);
		led_off(&fork_led[ph->fork1_id]);
	}
	return ret_val;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
