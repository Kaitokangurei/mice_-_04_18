/*
 * saitan.h
 *
 *  Created on: Jan 29, 2024
 *      Author: ovqxp
 */

#ifndef INC_SAITAN_H_
#define INC_SAITAN_H_

/*
 * saitan.c
 *
 *  Created on: Jan 29, 2024
 *      Author: ovqxp
 */
#include "saitan.h"
#include "motor.h"
#include "adati.h"
#include "PL_sensor.h"

void saitan_move_one_500(){//加速に２マスいる

	saitan_daikei(500, 500, 500, 600, 180);
}
void saitan_move_one_700(){//加速に２マスいる
/*	saitan_daikei(100, 500, 500, 600, 120);
	saitan_daikei(500, 700, 700, 600, 90);*/
	saitan_daikei(700, 700, 700, 600, 180);
/*	saitan_daikei(700, 700, 700, 600, 180);
	saitan_daikei(700, 500, 500, 600, 90);*/
/*	saitan_daikei(500, 500, 500, 600, 180);*/
/*	kabeate_start();*/
}

void saitan_move_one_no_PID() ;
void saitan_move_slalom_R(){//OK
	Gx=0;
	while(1){
		if(Gx>=20){
			break;
		}
	}
	motor_daikei_slarom_R(500,500,500,500,114);
	while(1){
		if(Gx>=20){
			break;
		}
	}
}
void saitan_move_slalom_L(){//つかわない
	Gx=0;
	while(1){
		if(Gx>=20){
			break;
		}
	}
	motor_daikei_slarom_L(500,500,500,500,120);
	while(1){
		if(Gx>=20){
			break;
		}
	}
}
void saitan_move_gen(){
	saitan_daikei(700, 500, 500, 600, 180);
}
void saitan_move_sakaime(){//100-500
	saitan_daikei(100, 500, 500, 600, 120);
}
void saitan_move_acc(){
	saitan_daikei(500, 700, 700, 600, 90);
}
void saitan_move_O_R_90(){

}
void saitan_move_O_L_90(){
	saitan_O_L(500,500,500,500,125);
}
void saitan_move_O_R_180(){
	saitan_O_R(500,500,500,500,230);
}
void saitan_move_O_L_180(){//OK
	saitan_O_L(500,500,500,500,250);
}
/*void saitan_move_gensoku();*/


#endif /* INC_SAITAN_H_ */
