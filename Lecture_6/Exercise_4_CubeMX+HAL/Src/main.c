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

#include "..\Lib\arr.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

///< Number of ADC samples (must be synced with HAL ADC settings)
#define ADC_SAMPLES_NUM		5

/// Length of ADC caption (including '\0') max value 255
#define ADC_CAPTION_LENGTH	6

/// ADC control struct.
typedef struct __ADC_ControlTypeDef {
	uint8_t caption[ADC_CAPTION_LENGTH];	///< User friendly name of ADC
	ADC_HandleTypeDef *adc;			///< Pointer to ADC HAL handler struct
	uint32_t buff[ADC_SAMPLES_NUM];		///< Measurements buffer
	uint8_t conv_cplt_f;			///< Conversion completed flag
} ADC_ControlTypeDef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/// Store ADC control struct
ADC_ControlTypeDef adc[3];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/** \brief	Set conversion compleat flag (redefined callback).
 *
 * \param[in] hadc	Pointer to ADC control struct.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == &hadc1) {
		adc[0].conv_cplt_f = 1;		// set conversion completed flag
	}
	if (hadc == &hadc2) {
		adc[1].conv_cplt_f = 1;
	}
	if (hadc == &hadc3) {
		adc[2].conv_cplt_f = 1;
	}
}

/** \brief	Initialise ADC control struct by HAL handle and caption.
 *
 * \param[out] 		*adc	Pointer to ADC control struct.
 * \param[in] 		*hadc	Pointer to HAL handle ADC struct.
 * \param[in,out]	title[]	Caption string.
 */
void adc_init(ADC_ControlTypeDef *adc, ADC_HandleTypeDef *hadc, char title[])
{	
	// copy caption (with preventing overloading)
	for (uint8_t i = 0; i < (ADC_CAPTION_LENGTH - 1); i++) {
		adc->caption[i] = title[i];
		
		if (title[i] == 0) {
			break;
		}
	}
	adc->caption[ADC_CAPTION_LENGTH - 1] = '\0';
	
	adc->conv_cplt_f = 0;
	adc->adc = hadc;
	
	HAL_ADC_Start_DMA(hadc, adc->buff, ADC_SAMPLES_NUM);
}

/** \brief	Print measured values according to received ADC control struct pointer.
 *
 * \param[in]	*adc	Pointer to ADC control struct.
 *
 * \note	Printing format: 
 *		[caption] tab [val1] tab [val2] ... tab [val5] new line + return
 */
void adc_print(ADC_ControlTypeDef *adc)
{
	// display ADC caption
	HAL_UART_Transmit(	&huart1, 
				adc->caption,
				strlen((char*)adc->caption),
				HAL_MAX_DELAY
			);
	uint8_t tab_ch = '\t';		
	HAL_UART_Transmit(&huart1, &tab_ch, 1, HAL_MAX_DELAY);
				
	// display measured(changed) values
	arr_print(adc->buff, ADC_SAMPLES_NUM);
}

/** \brief	Clean ADC values printin area from old values (3 lines height).
 *
 * \pre		Must be used after ADC measurements printing by adc_print().
 */
void terminal_clean(void)
{
	// move cursor up for three lines
	uint8_t up_str[] = {"\33[3A"};
	HAL_UART_Transmit(&huart1, up_str, strlen((char*)up_str), HAL_MAX_DELAY);
	
	// clear printing area (3 lines down)
	uint8_t clean_str[] = {"     \t   \t   \t   \t   \t   \n\r"};
	HAL_UART_Transmit(&huart1, clean_str, strlen((char*)clean_str), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1, clean_str, strlen((char*)clean_str), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1, clean_str, strlen((char*)clean_str), HAL_MAX_DELAY);
	
	// move cursor up for three lines
	HAL_UART_Transmit(&huart1, up_str, strlen((char*)up_str), HAL_MAX_DELAY);
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
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
	adc_init(adc + 0, &hadc1, "ADC 1");
	adc_init(adc + 1, &hadc2, "ADC 2");
	adc_init(adc + 2, &hadc3, "ADC 3");
	
	uint8_t welcome_msg[] = {"\n\r8bit ADC1-ch1 PA0, ADC2-ch1 PA4, ADC3-ch5 PB13 values\n\r"};
	HAL_UART_Transmit(&huart1, welcome_msg, strlen((char*)welcome_msg), 1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
  while (1)
  {		
	for (uint8_t i = 0; i < 3; i++) {
		adc_print(adc + i);
		
		if (adc[i].conv_cplt_f == 1) {
			adc[i].conv_cplt_f = 0;	// clear conversion compleated flag
			
			HAL_ADC_Start_DMA(adc[i].adc, adc[i].buff, ADC_SAMPLES_NUM);
		}
	}	
	HAL_Delay(100);				// user friendly speed down
	
	terminal_clean();

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12
                              |RCC_PERIPHCLK_ADC34;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV2;
  PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV2;
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
