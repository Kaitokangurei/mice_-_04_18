/*
 * tansaku.c
 *
 *  Created on: Jan 25, 2024
 *      Author: ovqxp
 */

#include "PL_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "motor.h"
#include "wall.h"
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "stm32l4xx_hal.h"
#include "PL_timer.h"

int detectWall_tansaku[4] = { 50,65,65,60};//センサーのウムのハンダンチ
int detectWall_tansaku_boundary[4] = { 50,35,35,60};//センサーのウムのハンダンチ

/*uint32_t  wall_save_F1[array_MAX];
uint32_t  wall_save_F2[array_MAX];
uint32_t  wall_save_R[array_MAX];
uint32_t  wall_save_L[array_MAX];
float Gacc;
float Gspeed ;
uint32_t wall_step;*/


void func_move_forward(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=1;
	uint8_t move_forward_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);


	uint8_t acc_delta=10;

	Gspeed =v_st;
	Gacc=acc;
	while(1){
		if(	(wall_save_F1[wall_step]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step]>=detectWall_tansaku_boundary[3])
				&&	(wall_save_F1[wall_step-1]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step-1]>=detectWall_tansaku_boundary[3])&&
				(wall_save_F1[wall_step-2]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step-2]>=detectWall_tansaku_boundary[3])){
			move_forward_flag=1;
			break;
		}
		else if(Gspeed>=v_max){
			move_forward_flag=2;
			Gspeed=v_max;
			break;
		}
		Gacc +=acc_delta;
		Gspeed += Gacc;
	}
	if(move_forward_flag==1){
	}
	else if(move_forward_flag==2){
		while(1){
			if(	(wall_save_F1[wall_step]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step]>=detectWall_tansaku_boundary[3])
					&&	(wall_save_F1[wall_step-1]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step-1]>=detectWall_tansaku_boundary[3])&&
					(wall_save_F1[wall_step-2]>=detectWall_tansaku_boundary[0]||wall_save_F2[wall_step-2]>=detectWall_tansaku_boundary[3])){
				move_forward_flag=1;
				break;
			}
		}
	}
		//gensoku
	while(1){
		Gacc=acc;
		Gacc +=acc_delta;
		Gspeed -= Gacc;
		if(Gspeed<=v_end){
			break;
		}
		else if(wall_save_F1[wall_step]>=detectWall_tansaku[0]||wall_save_F2[wall_step]>=detectWall_tansaku[3]){
					move_forward_flag=1;
			break;
		}
	}
	Gacc=0;
	Gspeed=0;
	move_forward_flag=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}




