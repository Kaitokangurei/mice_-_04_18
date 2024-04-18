/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PL_sensor.h"
#include "stm32l4xx_hal.h"
#include "PL_timer.h"
#include "adc.h"
#include "motor.h"
#include "wall.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles ADC1 global interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */

  /* USER CODE END ADC1_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_IRQn 1 */

  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */
  __HAL_TIM_SET_AUTORELOAD(&htim1,M_PWMcount_L);
 	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)((float)(M_PWMcount_L)/2));
 	__HAL_TIM_SET_COUNTER(&htim1, 0);
  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
  __HAL_TIM_SET_AUTORELOAD(&htim2, M_PWMcount_R);
 	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (uint16_t)((float)(M_PWMcount_R)/2));
 	__HAL_TIM_SET_COUNTER(&htim2, 0);
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC channel1 and channel2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
/*    uint16_t leftCount;*/
  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */
  pl_timer_count();
  pl_interupt_getSensor();
/*  sensor_update();*/
  wall_chack();
  detectSensorChange();
/*  func_wall_save();*/
  if(PID_wall_flag==1){
    	  if (wall_flag[1]==0&&wall_flag[2]==0) {
    		  	  PID_wall=0;
  			  Motor_run_L(Gspeed_L+PID_wall);
  			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    		else if (wall_flag[1]==1&&wall_flag[2]==0 &&difference_sensor_flag[1]==0) {
    			PID_wall = 0.25   *  (2  *  (float)(g_sensor[1] - CENTER_L));
    			  Motor_run_L(Gspeed_L+PID_wall);
    			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==0&&wall_flag[2]==1 &&difference_sensor_flag[2]==0) {
    				PID_wall = 0.25     *  (-2  *  (float)(g_sensor[2] - CENTER_R));
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==1&&wall_flag[2]==1 &&difference_sensor_flag[1]==0&&difference_sensor_flag[2]==0) {
    			    PID_wall = 0.25   *  ((float)(g_sensor[1] - CENTER_L) - (float)(g_sensor[2] - CENTER_R));
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R - PID_wall);
    			}
      }else  if(PID_flag_senkai==1){
      	  Gspeed_L = 50*Gspeed/90;
      	  Gspeed_R = 90*Gspeed/90;
    		  Motor_run_L(Gspeed_L);
    		  Motor_run_R(Gspeed_R);
      }
    	else  if(PID_flag_senkai==2){
    	  Gspeed_L = 90*Gspeed/90;
    	  Gspeed_R = 50*Gspeed/90;
  		  Motor_run_L(Gspeed_L);
  		  Motor_run_R(Gspeed_R);
    	}
  	else  if(PID_flag_senkai==3){
  		  	 Gspeed_L = 140*Gspeed/90;
  		  	 Gspeed_R = 30*Gspeed/90;
  				  Motor_run_L(Gspeed_L);
  				  Motor_run_R(Gspeed_R);
    }
  	else  if(PID_flag_senkai==4){
  		  	 Gspeed_L = 30*Gspeed/90;
  		  	 Gspeed_R = 140*Gspeed/90;
  				  Motor_run_L(Gspeed_L);
  				  Motor_run_R(Gspeed_R);
  	}
  	else  if(PID_flag_senkai==7){//R
  		 Gspeed_L = 150*Gspeed/90;//140
  	  	 Gspeed_R = 53*Gspeed/90;//30
  		  Motor_run_L(Gspeed_L);
  		  Motor_run_R(Gspeed_R);
  	  }
  	else  if(PID_flag_senkai==8){//L
  		 Gspeed_L = 53*Gspeed/90;
  	 	 Gspeed_R = 150*Gspeed/90;//140
  		  Motor_run_L(Gspeed_L);
  		  Motor_run_R(Gspeed_R);
     }else   if(PID_wall_flag==5){
  			  if (wall_flag[1]==0&&wall_flag[2]==0) {
    		  	  PID_wall=0;
  			  Motor_run_L(Gspeed_L+PID_wall);
  			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    		else if (wall_flag[1]==1&&wall_flag[2]==0 &&difference_sensor_flag[1]==0) {
    			PID_wall = 0.20   *  (2  *  (float)(g_sensor[1] - CENTER_L))*0.005*Gspeed;
    			  Motor_run_L(Gspeed_L+PID_wall);
    			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==0&&wall_flag[2]==1 &&difference_sensor_flag[2]==0) {
    				PID_wall = 0.20     *  (-2  *  (float)(g_sensor[2] - CENTER_R))*0.005*Gspeed;
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==1&&wall_flag[2]==1 &&difference_sensor_flag[1]==0&&difference_sensor_flag[2]==0) {
    			    PID_wall = 0.50   *  ((float)(g_sensor[1] - CENTER_L) - (float)(g_sensor[2] - CENTER_R))*0.005*Gspeed;
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R - PID_wall);
    			}
    }else   if(PID_wall_flag==6){
    	  if (wall_flag[1]==0&&wall_flag[2]==0) {
    		  	  PID_wall=0;
  			  Motor_run_L(Gspeed_L+PID_wall);
  			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    		else if (wall_flag[1]==1&&wall_flag[2]==0 &&difference_sensor_flag[1]==0) {
    			PID_wall = 0.1   *  (1  *  (float)(g_sensor[1] - CENTER_L));
    			  Motor_run_L(Gspeed_L+PID_wall);
    			  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==0&&wall_flag[2]==1 &&difference_sensor_flag[2]==0) {
    				PID_wall = 0.1     *  (-1  *  (float)(g_sensor[2] - CENTER_R));
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R-PID_wall);
    			}
    			else if (wall_flag[1]==1&&wall_flag[2]==1 &&difference_sensor_flag[1]==0&&difference_sensor_flag[2]==0) {
    			    PID_wall = 0.1   *  ((float)(g_sensor[1] - CENTER_L) - (float)(g_sensor[2] - CENTER_R));
    				  Motor_run_L(Gspeed_L+PID_wall);
    				  Motor_run_R(Gspeed_R - PID_wall);
    			}
    }
  	else {
    	  Motor_run_L(Gspeed_L);
    	  Motor_run_R(Gspeed_R);
      }
    Gspeed=Gspeed+0.001*Gacc;
    Gx= Gx+Gspeed*0.001;
    Gspeed_L = Gspeed;
    Gspeed_R = Gspeed;



/*  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_ADCBuffer, sizeof(g_ADCBuffer) / sizeof(uint16_t));*/

  /*  leftCount = calPWMCount( g_targetTrans.vel );*/
/*  leftCWCCW( g_targetTrans.vel );*/
  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
