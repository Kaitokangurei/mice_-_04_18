/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "tansaku.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PL_sensor.h"
#include "stm32l4xx_it.h"
#include "stm32l4xx_hal_conf.h"
#include "PL_timer.h"
#include "adc.h"
#include "motor.h"
#include "wall.h"
#include "adati.h"
#include "stdio.h"
#include "saitan.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_ADDRESS 0x7c
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int adati_flag =0;
int LIPO_flag=0;
float CENTER_L;
float CENTER_R;
int SWICH_COUNT=0;

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 10);
	return len;
}

void lcd_cmd(uint8_t x) {
	uint8_t aTxBuffer[2] = { 0x00, x };
	while (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) LCD_ADDRESS,
			(uint8_t*) aTxBuffer, 2, 1000) != HAL_OK) {
		if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			Error_Handler();
		}
	}
}

void lcd_data(uint8_t x) {
	uint8_t aTxBuffer[2] = { 0x40, x };
	while (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) LCD_ADDRESS,
			(uint8_t*) aTxBuffer, 2, 1000) != HAL_OK) {
		if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF) {
			Error_Handler();
		}
	}
}

void pl_lcd_puts(const char *s) {
	while (*s)
		lcd_data(*s++);
}
char strBuffer[17] = { 0 };


uint16_t bat;

float g_V_batt;

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
	setbuf(stdout, NULL);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_TIM15_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	pl_timer_init();

/*
	void pl_lcd_move(uint8_t pos) {
		lcd_cmd(0x80 | pos);
	}
*/

	void pl_lcd_pos(uint8_t raw, uint8_t col) {
		lcd_cmd(0x80 | ((raw & 0x01) << 6) | col);
	}

	void pl_lcd_clear() {
		lcd_cmd(0x01);
		HAL_Delay(2);
	}

	void pl_lcd_init(void) {
		//lcd initialize
		lcd_cmd(0x38);
		lcd_cmd(0x39);
		lcd_cmd(0x14);
		lcd_cmd(0x70);
		lcd_cmd(0x56);
		lcd_cmd(0x6c);
		HAL_Delay(300);
		lcd_cmd(0x38);
		lcd_cmd(0x0c);
		lcd_cmd(0x01);
		HAL_Delay(2);
		lcd_cmd(0x38);
	}
	HAL_TIM_Base_Start_IT(&htim15); //speaker
	HAL_TIM_PWM_MspInit(&htim15); //speaker
	HAL_TIM_Base_Start_IT(&htim1);//motor
	HAL_TIM_PWM_MspInit(&htim1);//motor
	HAL_TIM_Base_Start_IT(&htim2);//motor
	HAL_TIM_PWM_MspInit(&htim2);//motor
	HAL_TIM_Base_Start_IT(&htim6);

	HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	pl_lcd_init();

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	bat = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);


	//かべせいぎょのせんさ�??????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��ちしゅと?????��?��??��?��???��?��??��?��????��?��??��?��???��?��??��?��?
	CENTER_L =g_sensor[1];
	CENTER_R =g_sensor[2];

	while (1) {
	    if (HAL_GPIO_ReadPin(SWITCH_1_GPIO_Port,SWITCH_1_Pin)==0){
	    	HAL_GPIO_WritePin(INTERFACELED_GPIO_Port,INTERFACELED_Pin,GPIO_PIN_SET);
	    	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
	    	while(HAL_GPIO_ReadPin(SWITCH_1_GPIO_Port,SWITCH_1_Pin)==1){
	    	}
    	HAL_Delay(100);
	    	HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2);
	    	if(SWICH_COUNT==0){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("0!!");
				HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
		    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
		    	HAL_Delay(3);
		    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
		    	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		    	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

/*		    	saitan_move_one();*/
		    	saitan_move_sakaime();
		    	saitan_move_acc();
		    	saitan_move_gen();
/*		    	saitan_daikei(500, 500, 500, 600, 180);
		    	saitan_slalom_gen();
		    	saitan_move_slalom_L();
		    	saitan_move_acc();
		    	saitan_move_one_700();
		    	saitan_move_gen();*/
/*		    	saitan_move_slalom_L();*/
/*		    	saitan_move_O_L_90();
		    	saitan_move_acc();*/

		    	saitan();
		    	kabeate_start();
		    	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		    	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		    	HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);
	/*	    	motor_start_excitation();
		    	 motor_start();
		    	 motor_stop();*/
				pl_lcd_clear();
	    	}
	    	else if(SWICH_COUNT==1){pl_lcd_clear();
			pl_lcd_pos(0, 0);
			pl_lcd_puts("1");
			HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
	    	HAL_Delay(3);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
	    	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	    	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

			HAL_Delay(1500);
	    	motor_daikei(100, 400, 400, 600, 120);
	    	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	    	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	    	HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);
/*	    	motor_start_excitation();
	    	 motor_start();
	    	 motor_stop();*/
			pl_lcd_clear();
	    	}
	    	else if(SWICH_COUNT==3){
	    		    		adati_flag = 1;
	    		    		pl_lcd_clear();
	    		    		pl_lcd_pos(0, 0);
	    					pl_lcd_puts("adati");
	    					HAL_Delay(1500);
	    				    HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
	    			    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
	    			    	wait_ms(3);
	    			    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
	    			    	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	    			    	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	    			    	CENTER_L =g_sensor[1];
	    			    	CENTER_R =g_sensor[2];
	    		    		adatiMath(); // adatiMath()
	    				    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	    				    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	    				    HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);
	    					pl_lcd_clear();
	    	}
	    	else if(SWICH_COUNT==4){	pl_lcd_clear();
			pl_lcd_pos(0, 0);
			pl_lcd_puts("print");
			pl_lcd_pos(1, 0);
			pl_lcd_puts("MAP");
/*			printMaze();*/
/*			    HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
	    	wait_ms(3);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
	    	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	    	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		  	motor_daikei(100,500,100,500,180);
		    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		    HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);*/
			pl_lcd_clear();
	    	}
	    	else if(SWICH_COUNT==5){pl_lcd_clear();
			pl_lcd_pos(0, 0);
			pl_lcd_puts("MOTOR!!");
			HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
	    	HAL_Delay(3);
	    	HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
	    	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	    	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
/*		    	for(int lll=0;lll<=10;lll++){
				slalom_right();
	    	}*/
/*		    	 turn_back();*/
/*		    	func_move_forward(100,500,100,500,360);*/
/*		    	kabeate();*/
/*		    	 move_one();*/
/*		    	slalom_right();*/
	    	motor_daikei_slarom_R(100,500,100,500,800);
/*				slalom_left();
			move_one();
			move_one();*/
/*		    	 turn_back();*/
/*
	    	PID_wall_flag=0;
	    	Gspeed =1000;
	    	//Gspeed_R=500;
	    	//Gspeed_L=500;
			//M_PWMcount_R = calPWMCOunt(500);
			//M_PWMcount_L = calPWMCOunt(500);
	    	HAL_Delay(3000);//500m
*/
	    	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	    	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	    	HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);
/*	    	motor_start_excitation();
	    	 motor_start();
	    	 motor_stop();*/
			pl_lcd_clear();
	    	}
		}

	    printf("CENTER_L,R%f,%f\n\r",CENTER_L,CENTER_R);
		printf("SEN1=%u, SEN2=%u, SEN3=%u, SEN4=%u\n\r", g_sensor[0], g_sensor[1], g_sensor[2], g_sensor[3]);
		for (int i = 0; i < 4; i++) {
		        printf("%d ", wall_flag[i]);
		}
		for (int i = 0; i < 4; i++) {
        printf("%.1u ", average_sensor[i] );
		}
/*		HAL_ADC_Start_DMA(&hadc1, g_ADCBuffer,sizeof(g_ADCBuffer) /sizeof(uint16_t));
		printf("BATT=%f\n",g_V_batt);*/

		if (g_V_batt < LIPO_LIMIT){
			pl_lcd_clear();
			pl_lcd_pos(0, 0);
			pl_lcd_puts("LIPO");
			pl_lcd_pos(1, 0);
			pl_lcd_puts("error");
			wait_ms(500);
 			LIPO_flag=1;
		}

	    if (HAL_GPIO_ReadPin(SWITCH_2_GPIO_Port,SWITCH_2_Pin)==0){
	    	HAL_GPIO_WritePin(INTERFACELED_GPIO_Port,INTERFACELED_Pin,GPIO_PIN_SET);
	    	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
	    	while(HAL_GPIO_ReadPin(SWITCH_2_GPIO_Port,SWITCH_2_Pin)==1){
	    	}
	    	HAL_Delay(500);
	    	HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2);
	    	SWICH_COUNT++;
	    	if(SWICH_COUNT==6){
	    		SWICH_COUNT=0;
	    	}
	    	if(SWICH_COUNT==0){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH_2");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("0");
	    	}
	    	else if(SWICH_COUNT==1){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH_2");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("1");
	    	}
	    	else if(SWICH_COUNT==2){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH_2");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("2");
	    	}
	    	else if(SWICH_COUNT==3){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("3");
	    	}
	    	else if(SWICH_COUNT==4){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("4");
	    	}
	    	else if(SWICH_COUNT==5){
				pl_lcd_clear();
				pl_lcd_pos(0, 0);
				pl_lcd_puts("SWICH");
				pl_lcd_pos(1, 0);
				pl_lcd_puts("5");
	    	}
		 }




		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
	__disable_irq();
	while (1) {
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
