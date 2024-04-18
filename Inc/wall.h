/*
 * wall.h
 *
 *  Created on: Dec 7, 2023
 *      Author: ovqxp
 */

#ifndef INC_WALL_H_
#define INC_WALL_H_

extern int wall_flag[4];
extern int wall_flag2[4];
#define array_MAX 3800
extern int  wall_save_F1[array_MAX];
extern int  wall_save_F2[array_MAX];
extern int  wall_save_R[array_MAX];
extern int  wall_save_L[array_MAX];
extern int wall_step;
void wall_g_WallControlStatus();
void sensor_update();
void Sensor_Average();
void wall_chack() ;
void detectSensorChange();
void calWalControll();
void wall_handan();
void func_wall_save();
#endif /* INC_WALL_H_ */
