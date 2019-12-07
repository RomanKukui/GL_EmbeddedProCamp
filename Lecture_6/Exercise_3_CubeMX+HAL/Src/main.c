/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	#include <string.h>
	#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

	/// store current ADC 8-bit values
	uint8_t adc_curr[5];
	uint8_t adc_prev[5];
	
	/// store ADC conversion compleate flag
	uint8_t adc_cplt_f = 0;
	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/** \brief	Send/print arr* content to USART1 as integer values + "\n\r".
 *
 * \param[in] *arr	Pointer to array, that will be printed.
 * \param[in] size	Number of elements, that will be printed.
 *
 * \note	Used UART polling mode with HAL_MAX_DELAY timeout.
 */
void arr_print(uint8_t *arr, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		uint8_t msg[15];
		sprintf((char*)msg, "%i\t", arr[i]);
		HAL_UART_Transmit(&huart1, msg, strlen((char*)msg), HAL_MAX_DELAY);
	}
	uint8_t ret_msg[] = {"\n\r"};
	HAL_UART_Transmit(&huart1, ret_msg, 2, HAL_MAX_DELAY);
}

/** \brief	Compare elements of two arrays.
 *
 * \param[in] *arr1	Pointer to array #1.
 * \param[in] *arr2	Pointer to array #2.
 * \param[in] size	Number of elements for comparing (size of array).
 *
 * \note 	Will be compared only \p size number of elements.
 *
 * \retval -1		Arrays elements identical.
 * \retval 		Index of first different element.
 */
int8_t arr_cmp(uint8_t *arr1, uint8_t *arr2, size_t size)
{
	int8_t ret_val = -1;		// default value, arrays identical
	
	for (size_t i = 0; i < size; i++) {
		if (arr1[i] != arr2[i]) {
			ret_val = i;
			break;
		}
	}
	return ret_val;
}

/** \brief	Copy \p size elements from \p *src to \p *dst array.
 *
 * \param[out] *dst	Pointer to destination array.
 * \param[in] *src	Pointer to source array.
 * \param[in] size	Number of bytes, that will be copied.
 */
void arr_cpy(uint8_t *dst, uint8_t *src, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		dst[i] = src[i];
	}
}

/** \brief	Set conversion compleat flag (redefined callback).
 *
 * \param[in] hadc	Pointer to ADC control struct.
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	adc_cplt_f = 1;		// set conversion completed flag
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
	uint8_t welcome_msg[] = {"\n\rStartup initialisation completed\n\r"};
	HAL_UART_Transmit(&huart1, welcome_msg, strlen((char*)welcome_msg), 1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	// uint8_t* pointer converted to uint32_t* because used ADC 8-bit mode
	// and 1 byte DMA packet size
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_curr, 5);
	
  while (1)
  {
	if (adc_cplt_f == 1) {
		// test for changing ADC measurements relative to adc_prev[]
		int8_t res = -1;
		res = arr_cmp(adc_prev, adc_curr, sizeof(adc_curr));
		
		if ( res >= 0) {
			// display measured(changed) values
			arr_print(adc_curr, sizeof(adc_curr));
			
			// 'shift' to previous value
			arr_cpy(adc_prev, adc_curr, sizeof(adc_prev));
		}
		
		adc_cplt_f = 0;		// clear conversion compleated flag
		
		// uint8_t* pointer converted to uint32_t* because used ADC 8-bit mode
		// and 1 byte DMA packet size
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_curr, 5);
		
		HAL_Delay(50);		// user friendly speed down
	}
	  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
