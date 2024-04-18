/*
 * adati.h
 *
 *  Created on: Dec 8, 2023
 *      Author: ovqxp
 */

#ifndef INC_ADATI_H_
#define INC_ADATI_H_


#define HEIGHT 17
#define WIDTH 16
#define MAX_QUEUE_NUM 256

#define sensor_kabegire_0 95
#define sensor_kabegire_3 95//さいたんにも


void wall_get();//げんざいちのかべをしゅとく
void printMaze();
void adatiMaze();
/*void player_move();
void player_north();
void player_south();
void player_east();
void player_west();*/
void move_one();
void move_half();
void turn_right();
void turn_left();

void adatiMath();
void slalom_left();
void slalom_right();
void turn_back();
void move_square3();
void move_square();
void wall_get_adati();

void move_sakaime();
void move_sakaime_k();//さかいめまでうごかす+1
void player_move_K();
void player_north_K();
void player_south_K();
void player_east_K();
void player_west_K();
void move_sakaime_Z();//さかいめまでうごかす+1
void player_move_Z();
void player_move_Z_acc();
void player_move_Z_MAX();
void player_move_Z_MAX_s();
void player_move_Z_MAX_m();
void player_move_Z_gensoku();
void player_north_Z();
void player_south_Z();
void player_east_Z();
void player_west_Z();
void move_k_f();
void kabeate_goal();
void kabeate_R();
void kabeate_R_k();
void kabeate_L_k();
void kabeate_L();
void wall_get_adati();
void move_20();
void move_160();
void move_slalom_R();
void move_slalom_L();
void saitan();
void saitan_start();
void ZURU();
void ZURU2();
void move_one_notret();
void turn_back_notsakime();
void get_copy_direction();
void goal_move();
void move_saitan_1000();
void move_saitan_1000_return();
void kabeate_start();
void end_wall_flag();

void moveArrayElementsBack(int *, int , int );


extern unsigned char map[16][16];
extern unsigned char wall_x[HEIGHT][HEIGHT];//縦
extern unsigned char wall_y[HEIGHT][HEIGHT];//横
extern int player_pos_X, player_pos_Y;


#endif /* INC_ADATI_H_ */
