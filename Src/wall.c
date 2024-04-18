/*
 * wall.c
 *
 *  Created on: Dec 7, 2023
 *      Author: ovqxp
 */
#include "PL_sensor.h"
#include "wall.h"
#include "motor.h"


//研究室用
uint16_t detectWall[4] = { 30,45,60,30};//センサーのウムのハンダンチ
uint16_t detectWall2[4] = { 30,30,30,30};//センサーのウムのハンダンチ
uint16_t difference_sensor[4] = { 50,10,10,50 };//センサーのへんか


/*
//テスト用
int detectWall[4] = { 20,30,100,20 };//センサーのウムのハンダンチ
int difference_sensor[4] = { 10,20,50,10 };//センサーのへんか
*/

int wall_flag[4];//カベのハンダン
int wall_flag2[4];//カベのハンダン
uint8_t difference_sensor_flag[4];//センサーのへんかりょうのflag
uint16_t average_sensor[4];
/*float sensor_stack[4][4] ;//センサーの(こうしんじゅん,センサーのいち)*/
uint8_t PID_wall_flag;
float PID_wall;


/*float sensor_stack[4][4] = {0};//センサーの(こうしんじゅん,センサーのいち)
void sensor_update() {
    for (int i = 4-1; i >0; i--) {
        for (int j = 0; j < 4; j++) {
            sensor_stack[i][j] = sensor_stack[i - 1][j];
        }
    }
    for (int j =0; j < 4; j++) {
        sensor_stack[0][j] =g_sensor[j];
    }
}
void Sensor_Average(){
    for (int i = 0; i < 4; i++) {
            average_sensor[i] = (sensor_stack[0][i]+sensor_stack[1][i]+sensor_stack[2][i]+sensor_stack[3][i])/4;
    }
}*/
void wall_chack() {
    for (int i = 0; i <4 ; i++) {
        if (average_sensor[i] > detectWall[i]) {
            wall_flag[i] = 1;
        }
        else {
            wall_flag[i] = 0;
        }
    }
}

void detectSensorChange() {
    for (int i = 0; i <4; i++) {
        if (g_sensor[i] - average_sensor[i]>difference_sensor[i]) {
            difference_sensor_flag[i] = 1;
        }
        if (g_sensor[i] - average_sensor[i] <= difference_sensor[i]) {
            difference_sensor_flag[i] = 0;
        }
    }
}// センサーのへんかりょうをはんだんするかんすう


int  wall_save_F1[array_MAX];
int  wall_save_F2[array_MAX];
int  wall_save_R[array_MAX];
int  wall_save_L[array_MAX];
int wall_step;


void func_wall_save(){
	float Gx_wall_save=Gx;
	float Gx_wall_step=0.5;
	if(Gx_wall_save>=Gx_wall_step){
		wall_save_F1[wall_step] = g_sensor[0];
		wall_save_F2[wall_step]= g_sensor[3];
		wall_save_R[wall_step]= g_sensor[2];
		wall_save_L[wall_step]= g_sensor[1];
		wall_step++;
		Gx_wall_save=0;
	}
	if(wall_step>= array_MAX){
			wall_step=0;
		}
}

/*void calWalControll(){//つかってない
if (wall_flag[1]==0&&wall_flag[2]==0) {
		PID_wall_flag =0;
	    PID_wall=0;
	}
	else if (wall_flag[1]==1&&wall_flag[2]==0 &&difference_sensor_flag[1]==0) {//ひだり
	PID_wall_flag =1;
	}
	else if (wall_flag[1]==0&&wall_flag[2]==1 &&difference_sensor_flag[2]==0) {
		PID_wall_flag =2;
	}
	else if (wall_flag[1]==1&&wall_flag[2]==1 &&difference_sensor_flag[1]==0&&difference_sensor_flag[2]==0) {
		PID_wall_flag =3;
	}
}*/
