/*
 * PL_sensor.h
 *
 *  Created on: Nov 30, 2023
 *      Author: ovqxp
 */

#ifndef INC_PL_SENSOR_H_
#define INC_PL_SENSOR_H_
#define LIPO_LIMIT 11.7
float  pl_getbatt();

#include "stm32l4xx_hal.h"

extern uint16_t g_ADCBuffer[5];
extern float g_V_batt;

extern uint16_t g_sensor[4];
extern uint16_t g_sensor_on[4];
extern uint16_t g_sensor_off[4];

extern float  CENTER_L;// 右壁のセンサ中心値
extern float  CENTER_R;// 右壁のセンサ中心値
#define SENCOR_GAIN 0.5 // 仮のセンサーゲイン

extern int wall_flag[4];//カベのハンダン
extern uint8_t difference_sensor_flag[4];//センサーのへんかりょうのflag
extern uint16_t average_sensor[4];
extern uint16_t sensor_stack[4][4] ;//センサーの情報を保存(更新じゅん,センサーのいち)
extern uint8_t PID_wall_flag;
extern float PID_wall;
extern float  THRESHOLD_change_R;
extern float  THRESHOLD_change_L;
extern uint16_t count;

void pl_callback_getSensor();
void pl_interupt_getSensor();
void sensor_update();
void Sensor_Average();



#endif /* INC_PL_SENSOR_H_ */
