/*
 * motor.c
 *
 *  Created on: Dec 4, 2023
 *      Author: ovqxp
 */

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "PL_sensor.h"
#include "stm32l4xx_hal.h"
#include "PL_timer.h"
#include "adati.h"
#include "motor.h"
#include "wall.h"
#include "math.h"

uint16_t M_PWMcount_L;
uint16_t M_PWMcount_R;
float Gacc;
float Gspeed ;
float Gspeed_L;
float Gspeed_R;
float Gx;
float angacc;
float angvel;
float ang;
float PID_flag;
float PID_flag_senkai;
float PID_flag_slalom;
/*
int adati_count;*/
//モーター励磁
void motor_start_excitation(void){
HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_SET);
//励磁
HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_SET);
HAL_Delay(3);
HAL_GPIO_WritePin(MD_RESET_GPIO_Port,MD_RESET_Pin,GPIO_PIN_RESET);
}
//motor回転スタート
void motor_start(void){
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//MOTOR回転
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}
//motor回転ストップ ※励磁は切ってない
void motor_stop(void){
HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port,MOTOR_ENABLE_Pin,GPIO_PIN_RESET);
}

uint16_t calPWMCOunt(float speed) {

	uint16_t PWMCount;

	if ((fabs(speed) > 0.0)
			&& (78539.81634 / fabs(speed) * TIREDIAMETER < UINT16_MAX)) {
		PWMCount = (uint16_t) (78539.81634 / fabs(speed) * TIREDIAMETER) - 1;
	} else {
		PWMCount = UINT16_MAX - 1;
	}
	return PWMCount;
}

float cal_turnV(float W) {
	float TurnV = ((float)  TIREDIAMETER * M_PI / 360) * W;
	return TurnV;
}


void Motor_run_L(float velocity_L) {
		M_PWMcount_L = calPWMCOunt(velocity_L );
}

void Motor_run_R(float velocity_R) {
		M_PWMcount_R = calPWMCOunt(velocity_R );
}

void pl_motor_init(void) {
	HAL_TIM_Base_Start_IT(&htim1); //motor
	HAL_TIM_PWM_MspInit(&htim1); //motor
	HAL_TIM_Base_Start_IT(&htim2); //motor
	HAL_TIM_PWM_MspInit(&htim2); //motor
}


void motor_daikei_L(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=1;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
			HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
			HAL_Delay(3);
			float time_st =(v_max -v_st )/acc;
			float time_end = (v_max -v_end)/acc;
			float coX=X-((v_max+v_st)*time_st/2);
			coX=coX-((v_max+v_end)*time_end/2);
			float gensoku_time_end;
			float gensoku_X;//sankaku
			int sankaku_flag =0;
			Gspeed =v_st;
			Gacc = acc;

		Gx=0;
			while(1){
				gensoku_time_end = (Gspeed -v_end)/acc;
				gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
				if(X-Gx<gensoku_X){
					sankaku_flag =1;
					break;
				}
				if(Gspeed>v_max){
					Gspeed=v_max;
					break;
				}
			}

			if(	sankaku_flag !=1){
			Gacc=0;
			Gx=0;
			while(1){
				if(coX <Gx){
					break;
				}
			}
			}
			Gx=0;
			Gacc = -acc;
			while(1){
				if(Gspeed<v_end){
					Gspeed=v_end;
					break;
				}
			}
			Gacc=0;
			PID_wall_flag=0;
			PID_flag_senkai=0;
}

void motor_daikei_R(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=2;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		float gensoku_time_end;
		float gensoku_X;//sankaku
		int sankaku_flag =0;
		Gspeed =v_st;
		Gacc = acc;

	Gx=0;
		while(1){
			gensoku_time_end = (Gspeed -v_end)/acc;
			gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
			if(X-Gx<gensoku_X){
				sankaku_flag =1;
				break;
			}
			if(Gspeed>v_max){
				Gspeed=v_max;
				break;
			}
		}

		if(	sankaku_flag !=1){
		Gacc=0;
		Gx=0;
		while(1){
			if(coX <Gx){
				break;
			}
		}
		}
		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}
		Gacc=0;
		PID_wall_flag=0;
		PID_flag_senkai=0;
}
void motor_daikei_back(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,1);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,0);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void motor_daikei_slarom_L(float v_st,float v_max,float v_end,float acc,float X){
	PID_wall_flag=0;
	PID_flag_senkai=4;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void motor_daikei_slarom_R(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=3;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}

void motor_daikei_turn_right(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,1);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void motor_daikei_turn_left(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,0);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}

void motor_daikei_no_PID(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void motor_daikei_back_no_PID(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,1);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,0);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}



void motor_daikei_slarom_R_180(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=3;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}



void motor_daikei_slarom_L_180(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=4;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}



void motor_daikei_renzkoku(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=1;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		wall_get_adati();
			while(1){
				for (adati_count=0;adati_count<=254;adati_count++) {
					adatiMaze();
/*					if(Gx>=20){
						break;
					}*/
				}
/*				player_move();*/
				if(Gx>=20){
					break;
				}
			}

		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void motor_daikei_gensoku(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=0;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		Gspeed =v_st;
		Gacc = acc;

		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}

		while(1){
			if(X <=Gx){
				break;
			}
		}

		Gacc=0;
		PID_wall_flag=0;
		PID_flag_senkai=0;
}

void motor_daikei(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=1;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void motor_daikei_acc_Z(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	Gspeed =v_st;
	Gacc = acc;

	Gx=0;
	Gacc = acc;
	while(1){
		if(Gspeed>v_st){
			Gspeed=v_st;
			break;
		}
	}

	while(1){
		if(X <=Gx){
			break;
		}
	}

	Gacc=0;
	PID_wall_flag=0;
	PID_flag_senkai=0;
}


void saitan_motor_daikei_acc_Z(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=5;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	Gspeed =v_st;
	Gacc = acc;

	Gx=0;
	Gacc = acc;
	while(1){
		if(Gspeed>v_st){
			Gspeed=v_st;
			break;
		}
	}

	while(1){
		if(X <=Gx){
			break;
		}
	}

	Gacc=0;
	PID_wall_flag=0;
    PID_flag_senkai=0;
	PID_flag_slalom=0;
}


void saitan_motor_daikei(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=5;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
    PID_flag_senkai=0;
	PID_flag_slalom=0;
}
void saitan_motor_daikei_no_PID(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=0;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		float gensoku_time_end;
		float gensoku_X;//sankaku
		int sankaku_flag =0;
		Gspeed =v_st;
		Gacc = acc;

	Gx=0;
		while(1){
			gensoku_time_end = (Gspeed -v_end)/acc;
			gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
			if(X-Gx<gensoku_X){
				sankaku_flag =1;
				break;
			}
			if(Gspeed>v_max){
				Gspeed=v_max;
				break;
			}
		}

		if(	sankaku_flag !=1){
		Gacc=0;
		Gx=0;
		while(1){
			if(coX <Gx){
				break;
			}
		}
		}
		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}
		Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void saitan_motor_daikei_gensoku(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=0;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		Gspeed =v_st;
		Gacc = acc;

		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}

		while(1){
			if(X <=Gx){
				break;
			}
		}

		Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}



void saitan_motor_daikei_slarom_L(float v_st,float v_max,float v_end,float acc,float X){
	PID_wall_flag=0;
	PID_flag_senkai=4;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void saitan_motor_daikei_slarom_R(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=3;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}


void saitan_acc(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=5;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	Gspeed =v_st;
	Gacc = acc;

	Gx=0;
	Gacc = acc;
	while(1){
		if(Gspeed>v_st){
			Gspeed=v_st;
			break;
		}
	}

	while(1){
		if(X <=Gx){
			break;
		}
	}

	Gacc=0;
	PID_wall_flag=0;
    PID_flag_senkai=0;
	PID_flag_slalom=0;
}


void saitan_daikei(float v_st,float v_max,float v_end,float acc,float X){
    PID_flag_senkai=0;
	PID_wall_flag=0;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;//sankaku
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
	PID_wall_flag=0;
    PID_flag_senkai=0;
	PID_flag_slalom=0;
}
void saitan_daikei_no_PID(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=0;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		float gensoku_time_end;
		float gensoku_X;//sankaku
		int sankaku_flag =0;
		Gspeed =v_st;
		Gacc = acc;

	Gx=0;
		while(1){
			gensoku_time_end = (Gspeed -v_end)/acc;
			gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
			if(X-Gx<gensoku_X){
				sankaku_flag =1;
				break;
			}
			if(Gspeed>v_max){
				Gspeed=v_max;
				break;
			}
		}

		if(	sankaku_flag !=1){
		Gacc=0;
		Gx=0;
		while(1){
			if(coX <Gx){
				break;
			}
		}
		}
		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}
		Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void saitan_gensoku(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=0;
		PID_wall_flag=0;
		HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
		HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
		HAL_Delay(3);
		float time_st =(v_max -v_st )/acc;
		float time_end = (v_max -v_end)/acc;
		float coX=X-((v_max+v_st)*time_st/2);
		coX=coX-((v_max+v_end)*time_end/2);
		Gspeed =v_st;
		Gacc = acc;

		Gx=0;
		Gacc = -acc;
		while(1){
			if(Gspeed<v_end){
				Gspeed=v_end;
				break;
			}
		}

		while(1){
			if(X <=Gx){
				break;
			}
		}

		Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}



void saitan_slarom_L(float v_st,float v_max,float v_end,float acc,float X){
	PID_wall_flag=0;
	PID_flag_senkai=4;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void saitan_slarom_R(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=3;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}



void saitan_O_L(float v_st,float v_max,float v_end,float acc,float X){
	PID_wall_flag=0;
	PID_flag_senkai=8;
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
	Gacc=0;
		PID_wall_flag=0;
	    PID_flag_senkai=0;
		PID_flag_slalom=0;
}

void saitan_O_R(float v_st,float v_max,float v_end,float acc,float X){
	PID_flag_senkai=7;
	PID_wall_flag=0;//かべせいぎょ
	HAL_GPIO_WritePin(MOTOR_CW_CCW_R_GPIO_Port,MOTOR_CW_CCW_R_Pin,0);
	HAL_GPIO_WritePin(MOTOR_CW_CCW_L_GPIO_Port,MOTOR_CW_CCW_L_Pin,1);
	HAL_Delay(3);
	float time_st =(v_max -v_st )/acc;
	float time_end = (v_max -v_end)/acc;
	float coX=X-((v_max+v_st)*time_st/2);
	coX=coX-((v_max+v_end)*time_end/2);
	float gensoku_time_end;
	float gensoku_X;
	int sankaku_flag =0;
	Gspeed =v_st;
	Gacc = acc;

Gx=0;
	while(1){
		gensoku_time_end = (Gspeed -v_end)/acc;
		gensoku_X=(Gspeed+v_end)*gensoku_time_end/2;
		if(X-Gx<gensoku_X){
			sankaku_flag =1;
			break;
		}
		if(Gspeed>v_max){
			Gspeed=v_max;
			break;
		}
	}

	if(	sankaku_flag !=1){
	Gacc=0;
	Gx=0;
	while(1){
		if(coX <Gx){
			break;
		}
	}
	}
	Gx=0;
	Gacc = -acc;
	while(1){
		if(Gspeed<v_end){
			Gspeed=v_end;
			break;
		}
	}
		Gacc=0;
			PID_wall_flag=0;
		    PID_flag_senkai=0;
			PID_flag_slalom=0;
}





















