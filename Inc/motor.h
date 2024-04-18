/*
 * motor.h
 *
 *  Created on: Dec 4, 2023
 *      Author: ovqxp
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#define TIREDIAMETER 51.50//51.25
#define TREDWIDTH 90
//#define M_PI 3.14159

#include <stdint.h>

extern float Gacc;
extern float Gspeed ;
extern float Gspeed_L;
extern float Gspeed_R;

extern float Gx;
extern float angacc;
extern float angvel;
extern float angle;
extern float Gspeed_x;
extern float Gspeed_y;
extern float PID_flag;
extern float PID_flag_senkai;
extern float PID_flag_slalom;
extern uint16_t M_PWMcount_L;
extern uint16_t M_PWMcount_R;
extern int adati_count;

void pl_motor_init(void);
void motor_start_excitation();
//motor回転スタート
void motor_start();
//motor回転ストップ ※励磁は切ってない
void motor_stop();
void Motor_run_L(float);
void Motor_run_R(float);
float cal_turnV(float);
uint16_t calPWMCOunt(float);
void motor_daikei(float ,float,float ,float ,float );
void motor_daikei_gensoku(float ,float,float ,float ,float );
void motor_daikei_L(float ,float,float ,float ,float);
void motor_daikei_R(float ,float,float ,float ,float);
void motor_daikei_back(float ,float,float ,float ,float);
void motor_daikei_slarom_L(float ,float,float ,float ,float);
void motor_daikei_slarom_R(float ,float,float ,float ,float);
void motor_daikei_turn_left(float v_st,float v_max,float v_end,float acc,float X);
void motor_daikei_turn_right(float v_st,float v_max,float v_end,float acc,float X);
void motor_daikei_no_PID(float v_st,float v_max,float v_end,float acc,float X);
void motor_daikei_back_no_PID(float v_st,float v_max,float v_end,float acc,float X);
void motor_daikei_slarom_R_180(float ,float,float ,float ,float);
void motor_daikei_slarom_L_180(float ,float,float ,float ,float);
void motor_daikei_renzkoku(float ,float,float ,float ,float);
void motor_daikei_acc_Z(float ,float,float ,float ,float);

void saitan_motor_daikei_L(float ,float,float ,float ,float);
void saitan_motor_daikei_R(float ,float,float ,float ,float);
void saitan_motor_daikei_acc_Z(float ,float,float ,float ,float);
void saitan_motor_daikei_gensoku(float ,float,float ,float ,float );
void saitan_motor_daikei(float ,float,float ,float ,float );
void saitan_motor_daikei_no_PID(float ,float,float ,float ,float );

void saitan_slarom_R(float ,float,float ,float ,float );
void saitan_slarom_L(float ,float,float ,float ,float );
void saitan_O_R(float ,float,float ,float ,float );
void saitan_O_L(float ,float,float ,float ,float );
void saitan_gensoku(float ,float,float ,float ,float );
void saitan_daikei_no_PID(float ,float,float ,float ,float );
void saitan_daikei(float ,float,float ,float ,float );
void saitan_acc(float ,float,float ,float ,float );

void drv_mtr(int rightDirection, int leftDirection);
#endif /* INC_MOTOR_H_ */
