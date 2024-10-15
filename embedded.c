/* USER CODE BEGIN Header */

/**

  **************************

  * @file           : main.c

  * @brief          : Main program body

  **************************

  * @attention

  *

  * Copyright (c) 2024 STMicroelectronics.

  * All rights reserved.

  *

  * This software is licensed under terms that can be found in the LICENSE file

  * in the root directory of this software component.

  * If no LICENSE file comes with this software, it is provided AS-IS.

  *

  **************************

  */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"



/* Private includes ----------------------------------------------------------*/

/* USER CODE BEGIN Includes */

uint16_t gpio_Segment[]={63,3,118,115,75,121,125,35,127,123};

GPIO_TypeDef* gpio_Port=GPIOA;

uint16_t gpio_Pins[]={GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_0};



GPIO_TypeDef* digit_Port=GPIOD;

uint16_t digit_Pins[]={GPIO_PIN_1,GPIO_PIN_3,GPIO_PIN_4,GPIO_PIN_5};

uint16_t digit_Value[4];



volatile unsigned long  timer_first=0;

volatile unsigned long  timer_last=0;







uint8_t btn_cnt1=0;

uint8_t btn_cnt2=0;

uint8_t number_1=0;

uint8_t number_2=0;

uint16_t sum=0;





uint8_t number_1_mod;

uint8_t number_1_remainder;

uint8_t number_2_mod;

uint8_t number_2_remainder;

uint8_t sum_mod;

uint8_t sum_remainder;







uint8_t systick_10_ms=0;

uint8_t systick_3_ms=0;



uint16_t index_value=0;



uint16_t button_up_one=0;

uint16_t button_up_two=0;





typedef enum {

    IDLE,

    CHECK_PRESSED,

    PRESSED_OK_WAIT_PROCESSING

} ButtonStateTypeDef;



uint8_t Button1State = IDLE;

uint8_t Button2State = IDLE;



/* USER CODE END Includes */



/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */



/* USER CODE END PTD */



/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN PD */



/* USER CODE END PD */



/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

#define button_pin1 GPIO_PIN_1

#define button_pin2 GPIO_PIN_0

#define button_port GPIOB



/* USER CODE END PM */



/* Private variables ---------------------------------------------------------*/



/* USER CODE BEGIN PV */



/* USER CODE END PV */



/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);

void Clear(uint8_t digit);

static void MX_GPIO_Init(void);

void Toplam_Display(void);

void DisplayNumber(uint8_t digit, uint16_t number);

void DebounceButton(uint8_t number, GPIO_TypeDef port, uint16_t pin, uint32_t *lastTime, uint8_t *buttonFlag);

void ReadButton1(void);

void ReadButton2(void);

void Calculate_Display(void);

void Control (void);

void SysTickCustom(void);

void Button_Up_Down(void);



/* USER CODE BEGIN PFP */



/* USER CODE END PFP */



/* Private user code ---------------------------------------------------------*/

/* USER CODE BEGIN 0 */



void DisplayNumber(uint8_t digit, uint16_t number)

{

     	Clear(digit);



		HAL_GPIO_WritePin(digit_Port, digit, GPIO_PIN_SET);



	    for(int i=0;i<7;i++)

	    {

	    	if(number&(1<<i))

	    	{

				HAL_GPIO_WritePin(gpio_Port, gpio_Pins[i],GPIO_PIN_RESET);

			}



			else

			{

				HAL_GPIO_WritePin(gpio_Port, gpio_Pins[i],GPIO_PIN_SET);

			}

	    }

}



void Clear(uint8_t digit)

{

		for(int i=0;i<7;i++)

		{

			HAL_GPIO_WritePin(gpio_Port, gpio_Pins[i],GPIO_PIN_SET);

		}



		for(int i=0;i<4;i++)

		{

			 HAL_GPIO_WritePin(digit_Port,digit_Pins[i], GPIO_PIN_RESET);

		}

}



void ReadButton1(void)

{

	switch(Button1State)

	{



	  case IDLE://BOŞTA OKUMA

	  {

		  if(HAL_GPIO_ReadPin(button_port, button_pin1)==GPIO_PIN_SET)

	      Button1State=CHECK_PRESSED;

	  }



	  break;



	  case CHECK_PRESSED:



		  if(HAL_GPIO_ReadPin(button_port,button_pin1)==GPIO_PIN_SET)

		  {

			  btn_cnt1++;

			  if(btn_cnt1>=10)

			  {

				  Button1State=PRESSED_OK_WAIT_PROCESSING;

				  btn_cnt1=0;

			  }

		  }

		  else

		  {

			  Button1State=IDLE;

			  btn_cnt1=0;

		  }

	  break;



	  case PRESSED_OK_WAIT_PROCESSING:

		  if(HAL_GPIO_ReadPin(button_port,button_pin1)==GPIO_PIN_RESET)

		  {

			  Button1State=IDLE;

			  button_up_one=1;

		  }

	  break;



	  default:

		  Button1State=IDLE;

		  break;

	}

}



void ReadButton2(void)

{

	switch(Button2State)

	{



	  case IDLE:



		  if(HAL_GPIO_ReadPin(button_port, button_pin2)==GPIO_PIN_SET)

		  {

			Button2State=CHECK_PRESSED;

		  }

	 break;



	  case CHECK_PRESSED:



		  if(HAL_GPIO_ReadPin(button_port,button_pin2)==GPIO_PIN_SET)

		  {

			  btn_cnt2++;

			  if(btn_cnt2>=10)

			  {

				  Button2State=PRESSED_OK_WAIT_PROCESSING;

				  btn_cnt2=0;

			  }

		  }

		  else

		  {

			  Button2State=IDLE;

			  btn_cnt2=0;

		  }

	  break;



	  case PRESSED_OK_WAIT_PROCESSING:



		  if(HAL_GPIO_ReadPin(button_port,button_pin2)==GPIO_PIN_RESET)

		  {

			  Button2State=IDLE;

			  button_up_two=1;//bastık çektik set reset 1 olddu

		  }

	  break;





	  default:

		  Button2State=IDLE;

	  break;

	}

}



void Calculate_Display(void)

{

	 number_1_mod=number_1%10;

	 number_1_remainder=number_1/10;



	 number_2_mod=number_2%10;

	 number_2_remainder=number_2/10;



	 sum=number_1_mod +number_2_mod;



	 sum_mod= sum %10;

	 sum_remainder=sum/10;



	 if(number_1_remainder==1)

	 {

		 number_1_remainder=0;

	 }



	 if(number_2_remainder==1)

	 {

		 number_2_remainder=0;

	 }



	     digit_Value[0] = number_1_mod;

	     digit_Value[1] = number_2_mod;

	     digit_Value[2] = sum_remainder;

	     digit_Value[3] = sum_mod;

}



void Control (void)

{

	        for(int i=0;i<7;i++)

			{

				HAL_GPIO_WritePin(gpio_Port, gpio_Pins[i],GPIO_PIN_RESET);

			}



			for(int i=0;i<4;i++)

			{

				HAL_GPIO_WritePin(digit_Port, digit_Pins[i],GPIO_PIN_SET);

			}



			HAL_Delay(3000);

}



void SysTickCustom(void)

{



	timer_first = HAL_GetTick();



	if(timer_last != timer_first)

	{



		if((timer_first % 10)==0)

		{

			systick_10_ms=1;

		}



		if((timer_first % 3)==0)

			{

				systick_3_ms=1;

			}

	}



	timer_last = timer_first;

}



void Button_Up_Down(void)

{

	if(button_up_one==1)

		{

			number_1++;

			button_up_one=0;

		}

	if(button_up_two==1)

		{

			number_2++;

			button_up_two=0;

		}

}





/* USER CODE END 0

 *   */



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

  /* USER CODE BEGIN 2 */

  Control();

//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);//1 / 1000 Hz = 1 ms



  /* USER CODE END 2 */



  /* Infinite loop */

  /* USER CODE BEGIN WHILE */

  while (1)

  {

    /* USER CODE END WHILE */



    /* USER CODE BEGIN 3 */

	SysTickCustom();



	Calculate_Display();



	Button_Up_Down();



	if(systick_10_ms)

	{

		systick_10_ms=0;



		ReadButton1();

		ReadButton2();

	}



	if(systick_3_ms)

	{

		systick_3_ms=0;



		DisplayNumber(digit_Pins[index_value],gpio_Segment[digit_Value[index_value]]);



		index_value++;



		if (digit_Value[2] == 0)

		    {

		        HAL_GPIO_WritePin(digit_Port, digit_Pins[2], GPIO_PIN_RESET);

		    }

		    if (index_value >= 4)

		    {

		        index_value = 0;

		    }

     }



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



  /** Configure the main internal regulator output voltage

  */

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);



  /** Initializes the RCC Oscillators according to the specified parameters

  * in the RCC_OscInitTypeDef structure.

  */

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;

  RCC_OscInitStruct.HSEState = RCC_HSE_ON;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;

  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 8;

  RCC_OscInitStruct.PLL.PLLN = 336;

  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;

  RCC_OscInitStruct.PLL.PLLQ = 7;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)

  {

    Error_Handler();

  }



  /** Initializes the CPU, AHB and APB buses clocks

  */

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK

                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;



  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)

  {

    Error_Handler();

  }

}



/**

  * @brief GPIO Initialization Function

  * @param None

  * @retval None

  */

static void MX_GPIO_Init(void)

{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

/* USER CODE BEGIN MX_GPIO_Init_1 */

/* USER CODE END MX_GPIO_Init_1 */



  /* GPIO Ports Clock Enable */

  __HAL_RCC_GPIOH_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_GPIOD_CLK_ENABLE();



  /*Configure GPIO pin Output Level */

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);



  /*Configure GPIO pin Output Level */

  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

                          |GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);



  /*Configure GPIO pins : PA0 PA1 PA2 PA3

                           PA4 PA5 PA6 */

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



  /*Configure GPIO pins : PB0 PB1 */

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

  GPIO_InitStruct.Pull = GPIO_PULLDOWN;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



  /*Configure GPIO pins : PD0 PD1 PD2 PD3

                           PD4 PD5 */

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

                          |GPIO_PIN_4|GPIO_PIN_5;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);



/* USER CODE BEGIN MX_GPIO_Init_2 */

/* USER CODE END MX_GPIO_Init_2 */

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

  __disable_irq();

  while (1)

  {

  }

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

void assert_failed(uint8_t *file, uint32_t line)

{

  /* USER CODE BEGIN 6 */

  /* User can add his own implementation to report the file name and line number,

     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* USER CODE END 6 */

}

#endif /* USE_FULL_ASSERT */