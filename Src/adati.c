/*
 * adati.c
 *
 *  Created on: Dec 8, 2023
 *      Author: ovqxp
 */
#include "adati.h"
#include "PL_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "motor.h"
#include "wall.h"
#include "saitan.h"

/*
 * adati.c
 *
 *  Created on: Dec 8, 2023
 *      Author: ovqxp
 */


#define HEIGHT 17
#define WIDTH 16

#define kabeate_count 1500
#define START_POS_X 0
#define START_POS_Y 0
int goal_x_1 = 2;
int goal_x_2 = 3;
int goal_y_1 = 2;
int goal_y_2 =3;
#define START_playerDirection UP;
#define V_500 700

int adati_count;
int move_saitan[512];
int saitan_count=0;
int saitan_X, saitan_Y;
int count_step_gx=0;
int wall_get_called = 0;
int Gx_15_flag=0;
int copy_goal_X,copy_goal_Y;
unsigned char record_path_X,record_path_Y;
unsigned char record_map[16][16];
int MAX_V_count;
int saitan_F_count;
int saitan_F_flag;

int acc_flag,slalom_flag;


enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};
enum Direction playerDirection;

enum CardinalDirection {
	NORTH,
	SOUTH,
	EAST,
	WEST
};
enum CardinalDirection playerCardinal;

typedef struct {
    unsigned short x;
    unsigned short y;
} STACK_T_XY;

typedef struct{
/* データの最前列 */
int head_x;
/* データの最後尾 */
int tail_x;
/* スタックされているデータ */
int data_x[MAX_QUEUE_NUM];
} STACK_T_x;

typedef struct{
/* データの最前列 */
int head_y;
/* データの最後尾 */
int tail_y;
/* スタックされているデータ */
int data_y[MAX_QUEUE_NUM];
} STACK_T_y;

int isAlreadyPushed(STACK_T_x* stack_x, STACK_T_y* stack_y, int x, int y);

#define walk  255
int player_pos_X;
int player_pos_Y;
int walk_count = 0;
int kabe_count = 0;
int copy_map_player_pos;
int push_count;
int wall_X_flag[HEIGHT] [HEIGHT] ={0},wall_Y_flag[HEIGHT][HEIGHT]={0} ;

//めいろ用の定義
unsigned char map[16][16];
unsigned char wall_x[HEIGHT][HEIGHT];//縦
unsigned char wall_y[HEIGHT][HEIGHT];//横

unsigned char copy[WIDTH][WIDTH];
int player_pos[WIDTH][WIDTH];
// wall_get

enum Direction playerDirection;
enum CardinalDirection playerCardinal;

STACK_T_x stack_walk_x;
STACK_T_y stack_walk_y;
STACK_T_XY XY;
void pushStack_walk(STACK_T_x* stack_x, STACK_T_y* stack_y, unsigned short input_x, unsigned short input_y) {
	/* データをデータの最後尾の１つ後ろに格納 */
	stack_x->data_x[stack_x->tail_x] = input_x;
	stack_y->data_y[stack_y->tail_y] = input_y;
	//printf("push %d %d %3d\n", input_x, input_y, push_count);
	//printf("tail_x: %d, tail_y: %d\n", stack_x->tail_x, stack_y->tail_y);
	push_count++;

	/* データの最後尾を１つ後ろに移動 */
	stack_x->tail_x = stack_x->tail_x + 1;
	stack_y->tail_y = stack_y->tail_y + 1;
	if (stack_x->tail_x == MAX_QUEUE_NUM) stack_x->tail_x = 0;
	if (stack_y->tail_y == MAX_QUEUE_NUM) stack_y->tail_y = 0;
	/* スタックが満杯なら何もせず関数終了 */
	if ((stack_x->tail_x == stack_x->head_x) && (stack_y->tail_y == stack_y->head_y)) {
		//printf("stack_full\n");
		return;
	}
}
STACK_T_XY popStack_walk(STACK_T_x* stack_x, STACK_T_y* stack_y) {
	STACK_T_XY ret = { 0, 0 };

	/* スタックが空なら何もせずに関数終了 */
	if (stack_x->tail_x == stack_x->head_x || stack_y->tail_y == stack_y->head_y) {
		//printf("stack_empty\n");
		ret.x = ret.y = 65535;
		return ret;
	}

	/* データの最前列からデータを取得 */
	ret.x = stack_x->data_x[stack_x->head_x];
	ret.y = stack_y->data_y[stack_y->head_y];
	//printf("Ret: x = %hu, y = %hu, tail_y: %d\n", ret.x, ret.y, stack_x->tail_x);

	/* データの最前列を１つ前にずらす */
	stack_x->head_x = (stack_x->head_x + 1) % MAX_QUEUE_NUM;
	stack_y->head_y = (stack_y->head_y + 1) % MAX_QUEUE_NUM;

	/* 巡回シフト */
	if (stack_x->head_x == MAX_QUEUE_NUM && stack_y->head_y == MAX_QUEUE_NUM) {
		stack_x->head_x = 0;
		stack_y->head_y = 0;
	}

	/* 取得したデータを返却 */
	return (STACK_T_XY) { ret.x, ret.y };
}


void initializeMaze() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			map[i][j] = walk;
			wall_x[i][j] = ' ';
			wall_y[i][j] = ' ';
		}
	}
	for (int i = 0; i < HEIGHT; i++) {
		wall_x[0][i] = '-';
	}
	//ゴール
	for (int i = 7; i < 9; i++)wall_x[7][i] = '-';
	for (int i = 7; i < 8; i++)	wall_x[9][i] = '-';
	for (int i = 7; i < 9; i++)	wall_y[i][7] = '/';
	for (int i = 7; i < 9; i++)	wall_y[i][9] = '/';
	for (int i = 7; i < 9; i++)	wall_x[8][i] = ' ';
	for (int i = 7; i < 9; i++)	wall_y[i][8] = ' ';
	map[goal_y_1][goal_x_1] = 0;
	map[goal_y_1][goal_x_2] = 0;
	map[goal_y_2][goal_x_1] = 0;
	map[goal_y_2][goal_x_2] = 0;
	for (int i = 0; i < HEIGHT; i++) {
		wall_x[0][i] = '-';
		wall_x[WIDTH][i] = '-';
		wall_y[i][0] = '/';
		wall_y[i][WIDTH] = '/';
	}
	playerDirection = UP;
	for (int jop = 0; jop <= 256; jop++) {
		adatiMaze();
	}
}
void wall_get() {
	if (playerDirection == UP) {
		if (wall_flag[0] == 1 && wall_flag[3] == 1) {
			wall_x[player_pos_Y + 1][player_pos_X] = '-';
		}
		if (wall_flag[1] == 1) {
				wall_y[player_pos_Y][player_pos_X] = '/';
		}
		if (wall_flag[2] == 1) {
				wall_y[player_pos_Y][player_pos_X + 1] = '/';
		}
		wall_X_flag[player_pos_Y + 1][player_pos_X] =1;
		wall_Y_flag[player_pos_Y][player_pos_X] = 1;
		wall_Y_flag[player_pos_Y][player_pos_X + 1] = 1;
	}
	else if (playerDirection == DOWN) {
		if (wall_flag[0] == 1 && wall_flag[3] == 1) {
			wall_x[player_pos_Y][player_pos_X] = '-';
		}
		if (wall_flag[1] == 1) {
				wall_y[player_pos_Y][player_pos_X + 1] = '/';
		}
		if (wall_flag[2] == 1) {
				wall_y[player_pos_Y][player_pos_X] = '/';
			}
		wall_X_flag[player_pos_Y][player_pos_X] =1;
		wall_Y_flag[player_pos_Y][player_pos_X+1] = 1;
		wall_Y_flag[player_pos_Y][player_pos_X] = 1;
		}
	else if (playerDirection == RIGHT) {
		if (wall_flag[0] == 1 && wall_flag[3] == 1) {
			wall_y[player_pos_Y][player_pos_X + 1] = '/';
		}
		if (wall_flag[1] == 1) {
				wall_x[player_pos_Y + 1][player_pos_X] = '-';
		}
		if (wall_flag[2] == 1) {
				wall_x[player_pos_Y][player_pos_X] = '-';
		}
		wall_Y_flag[player_pos_Y ][player_pos_X+1] =1;
		wall_X_flag[player_pos_Y + 1][player_pos_X] =1;
		wall_X_flag[player_pos_Y][player_pos_X] =1;
	}
	else if (playerDirection == LEFT) {
		if (wall_flag[0] == 1 && wall_flag[3] == 1) {
			wall_y[player_pos_Y][player_pos_X] = '/';
		}
		if (wall_flag[1] == 1) {
			wall_x[player_pos_Y][player_pos_X] = '-';
		}
		if (wall_flag[2] == 1) {
			wall_x[player_pos_Y + 1][player_pos_X] = '-';
		}
		wall_Y_flag[player_pos_Y][player_pos_X] =1;
		wall_X_flag[player_pos_Y][player_pos_X] =1;
		wall_X_flag[player_pos_Y + 1][player_pos_X] =1;
	}
}
void printMaze() {

	for (int i = 0; i <= WIDTH; i++) {
		printf("%2d  ", i);
	}
	printf("\n\r");

	for (int i = HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j <= WIDTH; j++) {
			printf("+");
			if (j != 16) {
				for (int k = 0; k < 3; k++) {
					printf("%c", wall_x[i][j]);
				}
			}
		}
		printf("%d", i);
		printf("\n\r");

		if (i != 0) {
			for (int j = 0; j <= WIDTH; j++) {
				printf("%c", wall_y[i - 1][j]);
				if (j != WIDTH) {
/*					if (player_pos_X == j && player_pos_Y == i-1) {
						printf(" @ ");
					}
					else if (saitan_X == j && saitan_Y == i - 1) {
						printf(" @ ");
					}*/
					//else {
						printf("%3d", map[i - 1][j]);
				//	}
				}
			}
		}

		printf("\n\r");
	}
	printf("player(");
	if (playerDirection == UP) {
		printf("UP");
	}
	else if (playerDirection == DOWN) {
		printf("DOWN");
	}
	else if (playerDirection == LEFT) {
		printf("WEST");
	}
	else if (playerDirection == RIGHT) {
		printf("EAST");
	}
	printf("%d,%d)", saitan_X, saitan_Y);
	printf("%d,%d)\n\n", player_pos_X, player_pos_Y);
}
void adatiMaze() {
	if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) {//左
		if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
			map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
			pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
		}
	}
	if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) {//下
		if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
			map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
			pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
		}
	}
	if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) {//右
		if (copy[XY.y][XY.x + 1] > copy[XY.y][XY.x]) {
			map[XY.y][XY.x + 1] = copy[XY.y][XY.x] + 1;
			pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
		}
	}
	if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) {//上
		if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
			map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
			pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y + 1);
		}
	}
	if (map[player_pos_Y][player_pos_X] != copy_map_player_pos) {
		adati_count = 254;
	}
}
void adatiMath() {
	//initializeMaze();
	walk_count = 0;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			map[i][j] = walk;
			wall_x[i][j] = ' ';
			wall_y[i][j] = ' ';
		}
	}
	map[goal_y_1][goal_x_1] = 0;
	map[goal_y_1][goal_x_2] = 0;
	map[goal_y_2][goal_x_1] = 0;
	map[goal_y_2][goal_x_2] = 0;

	for (int i = 0; i < HEIGHT; i++) {
		wall_x[0][i] = '-';
		wall_x[WIDTH][i] = '-';
		wall_y[i][0] = '/';
		wall_y[i][WIDTH] = '/';
	}
	player_pos_X = START_POS_X;
	player_pos_Y = START_POS_Y;
	playerDirection = START_playerDirection;
	wall_y[0][1] = '/';
	//initilize_end

	//printMaze();
	//usleep(1000000);

	//初手移動
	move_sakaime();
	wall_get();
	Gx=0;
	//tansaku
	for (int move_num = 0; move_num <= 1000; move_num++) {
		//INPUT255
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < WIDTH; j++) {
				map[i][j] = walk;
			}
		}
		//goal_X,Y
		map[goal_y_1][goal_x_1] = 0;
		map[goal_y_1][goal_x_2] = 0;
		map[goal_y_2][goal_x_1] = 0;
		map[goal_y_2][goal_x_2] = 0;

		//walk_count = 0;

		STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
		STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

		pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_1);
		pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_2);
		pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_1);
		pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_2);

		//caluculate
		for (adati_count = 0; adati_count <= 256; adati_count++) {
			XY = popStack_walk(&stack_walk_x, &stack_walk_y);

			// スタックが空の場合はループを終了する
			if (XY.x == 65535 && XY.y == 65535) {
				break;
			}

			if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) {//左
				if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
					map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
					}
				}
			}
			if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) {//下
				if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
					map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
					}
				}
			}
			if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) {//右
				if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
					map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
					}
				}
			}
			if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) {//上
				if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
					map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
					}
				}
			}

		}
		kabe_count++;
		record_map[player_pos_Y][player_pos_X]=1;
		wall_get_called = 0;
		Gx_15_flag=0;
		PID_wall_flag=5;
		//さいしょ
		while (1) {
		    if(Gx >= 1 && !wall_get_called) {
		        wall_get();
		        wall_get_called = 1;
		    }
			if (Gx >= 10) {
				PID_wall_flag=0;
/*				player_move_K();*/
				player_move_Z();
				break;
			}
		}
		Gx = 0;
		//printMaze();
		if (map[player_pos_Y][player_pos_X] == walk) {
			break;
		}
		if ((player_pos_X == goal_x_1 || player_pos_X == goal_x_2) && (player_pos_Y == goal_y_1 || player_pos_Y == goal_y_2)) {
			goal_move();
			ZURU2();
/*			ZURU();
			ZURU2();*/
			//modori	//初手移動
/*			move_sakaime();
			wall_get();*/
//			ZURU();
			break;
		}
	}
}

int isAlreadyPushed(STACK_T_x* stack_x, STACK_T_y* stack_y, int x, int y) {
	for (int i = stack_x->head_x; i != stack_x->tail_x; i = (i + 1) % MAX_QUEUE_NUM) {
		if (stack_x->data_x[i] == x && stack_y->data_y[i] == y) {
			return 1; // (x, y) がすでにスタックに含まれている
		}
	}
	return 0; // (x, y) がスタックに含まれていない
}

void player_move_K() {
	//うえ
	if (map[player_pos_Y][player_pos_X] == map[player_pos_Y + 1][player_pos_X] + 1 && wall_x[player_pos_Y + 1][player_pos_X] != '-') {
		player_north_K();
	}
	//みぎ
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X + 1] + 1 && wall_y[player_pos_Y][player_pos_X + 1] != '/') {
		player_east_K();
	}
	//ひだり
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X - 1] + 1 && wall_y[player_pos_Y][player_pos_X] != '/') {
		player_west_K();
	}
	//した
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y - 1][player_pos_X] + 1 && wall_x[player_pos_Y][player_pos_X] != '-') {
		player_south_K();
	}

}

void player_north_K() {//きたにうごく
	if (playerDirection == UP)
	{
/*		player_move_Z_MAX();*/
		move_k_f();//コメント外して上の消して
		playerDirection = UP;
	}
	else if (playerDirection == DOWN) {
		turn_back();
		player_pos_Y += 1;
		playerDirection = UP;
	}
	else if (playerDirection == RIGHT) {
		player_pos_Y += 1;
		//kabeate
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_L();
		}
		playerDirection = UP;
	}
	else if (playerDirection == LEFT) {
		player_pos_Y += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
			kabe_count = 0;
			kabeate_R_k();
			motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_R();
		}
		playerDirection = UP;
	}

}
void player_south_K() {//みなみにうごく
	if (playerDirection == UP) {
		turn_back();
		player_pos_Y -= 1;
		playerDirection = DOWN;
	}
	else if (playerDirection == DOWN) {
		move_k_f();
		playerDirection = DOWN;
	}
	else if (playerDirection == RIGHT) {
		player_pos_Y -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_R_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_R();
		}
		playerDirection = DOWN;
	}
	else if (playerDirection == LEFT) {
		player_pos_Y -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_L();
		}
		playerDirection = DOWN;
	}

}
void player_east_K() {//ひがしにうごく
	if (playerDirection == UP) {
		player_pos_X += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_R_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_R();
		}
		playerDirection = RIGHT;
	}
	else if (playerDirection == DOWN) {
		player_pos_X += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_L();
		}
		playerDirection = RIGHT;
	}
	else if (playerDirection == RIGHT) {
		move_k_f();
		playerDirection = RIGHT;
	}
	else if (playerDirection == LEFT) {
		turn_back();
		player_pos_X += 1;
		playerDirection = RIGHT;
	}
}
void player_west_K() {//にしにうごく
	if (playerDirection == UP) {
		player_pos_X -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				motor_daikei(100, 400, 400, 600, 120);
			}else{
			move_slalom_L();
			}
		}
	else if (playerDirection == DOWN) {
		player_pos_X -= 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_R_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			move_slalom_R();
		}
	}
	else if (playerDirection == RIGHT) {
		turn_back();
		player_pos_X -= 1;
	}
	else if (playerDirection == LEFT) {
		move_k_f();
	}
	playerDirection = LEFT;
}

void move_one() {
	motor_daikei(400, 400, 400, 600, 180);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X += 1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}

void move_one_notret() {
	motor_daikei(400, 400, 400, 600, 180);
}

void turn_left() {
	printf("turn_left\n\r");
//	motor_daikei_turn_left(100, 500, 100, 500, 48);
	if (playerDirection == UP) {
		playerDirection = RIGHT;
	}
	else if (playerDirection == DOWN) {
		playerDirection = LEFT;
	}
	else if (playerDirection == RIGHT) {
		playerDirection = DOWN;
	}
	else if (playerDirection == LEFT) {
		playerDirection = UP;
	}
	PID_flag_senkai = 0;
}

void turn_right() {
	//printf("turn_right\n\r");
	//motor_daikei_turn_right(100, 500, 100, 500, 48);
	if (playerDirection == UP)
		playerDirection = LEFT;
	else if (playerDirection == DOWN)
		playerDirection = RIGHT;
	else if (playerDirection == RIGHT)
		playerDirection = UP;
	else if (playerDirection == LEFT)
		playerDirection = DOWN;
	PID_flag_senkai = 0;
}

void turn_back() {//みかくにん
	if(wall_flag[0]==1&&wall_flag[3]==1){
		if(wall_flag[2]==1){
			motor_daikei(300, 300,200, 800, 60);
			motor_daikei_turn_left(200, 200, 200, 500, 69);
			motor_daikei_back_no_PID(100, 300, 100, 500, 68);
			motor_daikei_no_PID(100, 300, 100, 500, 35);
			motor_daikei_turn_left(200, 200, 200, 500, 68);
			motor_daikei_back_no_PID(100, 500, 100, 500, 100);
		}
		else  if(wall_flag[1]==1){
				motor_daikei(300, 300,200, 800, 60);
				motor_daikei_turn_right(200, 200, 200, 500, 69);
				motor_daikei_back_no_PID(100, 300, 100, 500, 68);
				motor_daikei_no_PID(100, 300, 100, 500, 35);
				motor_daikei_turn_right(200, 200, 200, 500, 68.5);
				motor_daikei_back_no_PID(100, 300, 100, 500, 100);
		}
		else{
			motor_daikei(300, 300,200, 800, 60);
			motor_daikei_turn_left(200, 200, 200, 500, 128);
			motor_daikei_back_no_PID(100, 500, 100, 500, 50);
		}
	}
	else{
		motor_daikei(300, 300,200, 800, 60);
		motor_daikei_turn_left(200, 200, 200, 500, 128);
		motor_daikei_back_no_PID(100, 500, 100, 500, 50);
	}
	kabe_count = 0;

	motor_daikei(100, 400, 400, 600, 120);
}

void slalom_left() {
	//printf("slalom_L\n\r");
	motor_daikei_slarom_L(100,500,100,500,210);
	//motor_daikei(300, 300, 300, 400, 80);
	//motor_daikei_turn_left(200, 200, 200, 500, 68);
	//motor_daikei(300, 300, 300, 400, 90);
	if (playerDirection == UP)
		playerDirection = LEFT;
	else if (playerDirection == DOWN)
		playerDirection = RIGHT;
	else if (playerDirection == RIGHT)
		playerDirection = UP;
	else if (playerDirection == LEFT) {
		playerDirection = DOWN;
	}
}
void slalom_right() {
	//printf("slalom_R\r\n");
	motor_daikei_slarom_R(100,500,100,500,210);
	/*motor_daikei(300, 300, 300, 400, 80);
	motor_daikei_turn_right(200, 200, 200, 500, 68);
	motor_daikei(300, 300, 300, 400, 90);*/
	if (playerDirection == UP) {
		playerDirection = RIGHT;
	}
	else if (playerDirection == DOWN) {
		playerDirection = LEFT;
	}
	else if (playerDirection == RIGHT) {
		playerDirection = DOWN;
	}
	else if (playerDirection == LEFT) {
		playerDirection = UP;
	}
}

void kabeate_start(){
	if(wall_flag[2]==1){
			motor_daikei(300, 300,200, 800, 60);
			motor_daikei_turn_left(200, 200, 200, 500, 68);
			motor_daikei_back_no_PID(100, 300, 100, 500, 70);
			motor_daikei_no_PID(100, 300, 100, 500, 35);
			motor_daikei_turn_left(200, 200, 200, 500, 68);
			motor_daikei_back_no_PID(100, 500, 100, 500, 100);
		}
		else if(wall_flag[1]==1){
				motor_daikei(300, 300,200, 800, 60);
				motor_daikei_turn_right(200, 200, 200, 500, 68);
				motor_daikei_back_no_PID(100, 300, 100, 500, 70);
				motor_daikei_no_PID(100, 300, 100, 500, 35);
				motor_daikei_turn_right(200, 200, 200, 500, 68);
				motor_daikei_back_no_PID(100, 300, 100, 500, 100);
			}
		else{
			motor_daikei(300, 300,200, 800, 60);
			motor_daikei_turn_left(200, 200, 200, 500, 128);
			motor_daikei_back_no_PID(100, 500, 100, 500, 140);
		}
		kabe_count = 0;
}
void kabeate_R_k(){
	motor_daikei(300, 300, 200, 800, 60);
	motor_daikei_turn_right(200, 200, 200, 500, 128);
	motor_daikei_back_no_PID(100, 500, 100, 500, 140);
	motor_daikei_no_PID(100, 300, 100, 500, 35);
	motor_daikei_turn_left(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 120);
}
void kabeate_L_k(){
	motor_daikei(300, 300,200, 800, 60);
	motor_daikei_turn_left(200, 200, 200, 500, 128);
	motor_daikei_back_no_PID(100, 500, 100, 500, 140);
	motor_daikei_no_PID(100, 300, 100, 500, 35);
	motor_daikei_turn_right(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 120);
}
void kabeate_R() {
	/*motor_daikei(300, 300, 300, 400, 100);
	motor_daikei_turn_right(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 90);
	motor_daikei_no_PID(100, 500, 100, 500, 35);
	motor_daikei_turn_left(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 105);*/
	//CENTER_L = g_sensor[1];
	//CENTER_R = g_sensor[2];
/*	motor_daikei(300, 300, 0, 800, 60);
	motor_daikei_turn_right(200, 200, 200, 500, 65);
	motor_daikei_back_no_PID(100,500, 100, 500, 35);
	motor_daikei_turn_right(200, 200, 200, 500, 65);
	motor_daikei_back_no_PID(100, 500, 100, 500, 105);*///kowareteru

	if (playerDirection == UP) {
		playerDirection = DOWN;
	}
	else if (playerDirection == DOWN) {
		playerDirection = UP;
	}
	else if (playerDirection == RIGHT) {
		playerDirection = LEFT;
	}
	else if (playerDirection == LEFT) {
		playerDirection = RIGHT;
	}
	move_sakaime();
}
void kabeate_L() {
	/*motor_daikei(300, 300, 300, 400, 100);
	motor_daikei_turn_right(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 90);
	motor_daikei_no_PID(100, 500, 100, 500, 35);
	motor_daikei_turn_right(200, 200, 200, 500, 68);
	motor_daikei_back_no_PID(100, 500, 100, 500, 105);*/
	//CENTER_L = g_sensor[1];
	//CENTER_R = g_sensor[2];
/*	motor_daikei(300, 300, 0, 800, 60);
	motor_daikei_turn_left(200, 200, 200, 500, 65);
	motor_daikei_back_no_PID(100, 500, 100, 500, 110);
	motor_daikei_no_PID(100, 500, 100, 500, 35);
	motor_daikei_no_PID(100, 500, 100, 500, 35);
	motor_daikei_turn_left(200, 200, 200, 500, 65);
	motor_daikei_back_no_PID(100, 500, 100, 500, 105);*///kowaretru

	if (playerDirection == UP) {
		playerDirection = DOWN;
	}
	else if (playerDirection == DOWN) {
		playerDirection = UP;
	}
	else if (playerDirection == RIGHT) {
		playerDirection = LEFT;
	}
	else if (playerDirection == LEFT) {
		playerDirection = RIGHT;
	}
	move_sakaime();
}

void move_20() {
	//printf("20cmmove_end\n");
}

void move_160() {
	//printf("160cm\n");
}
void move_slalom_R() {
	if (playerDirection == UP) {
		playerDirection = RIGHT;
	}
	else if (playerDirection == DOWN) {
		playerDirection = LEFT;
	}
	else if (playerDirection == RIGHT) {
		playerDirection = DOWN;
	}
	else if (playerDirection == LEFT) {
		playerDirection = UP;
	}
	Gx=0;
	//	motor_daikei(400,400,400,400,3);
	while(1){
		if(g_sensor[0]>=sensor_kabegire_0&&g_sensor[3]>=sensor_kabegire_3){
			break;
		}
		if(Gx>=20){
			break;
		}
	}

	motor_daikei_slarom_R(400,400,400,400,100);
	motor_daikei(400, 400, 400, 600, 26);
}
void move_slalom_L() {
	Gx=0;
	if (playerDirection == UP)
		playerDirection = LEFT;
	else if (playerDirection == DOWN)
		playerDirection = RIGHT;
	else if (playerDirection == RIGHT)
		playerDirection = UP;
	else if (playerDirection == LEFT) {
		playerDirection = DOWN;
	}
	while(1){
		if(g_sensor[0]>=sensor_kabegire_0&&g_sensor[3]>=sensor_kabegire_3){
			break;
		}
		if(Gx>=20){
			break;
		}
	}
	motor_daikei_slarom_L(400,400,400,400,100);
	motor_daikei(400, 400, 400, 600, 26);
}

void move_sakaime() {
	motor_daikei(100, 400, 400, 600, 120);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X += 1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}
void move_sakaime_k() {
	motor_daikei(400, 400, 400, 600, 60);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X += 1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}

void move_k_f() {
	motor_daikei(400, 400, 400, 400, 160);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X += 1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}


}
void kabeate_goal(){
	if(wall_flag[0]==1&&wall_flag[3]==1){
			if(wall_flag[2]==1){
				motor_daikei(300, 300,200, 800, 60);
				motor_daikei_turn_left(200, 200, 200, 500, 68.5);
				motor_daikei_back_no_PID(100, 300, 100, 500, 68);
				motor_daikei_no_PID(100, 300, 100, 500, 35);
				motor_daikei_turn_left(200, 200, 200, 500, 68);
				motor_daikei_back_no_PID(100, 500, 100, 500, 100);
			}
			else  if(wall_flag[1]==1){
					motor_daikei(300, 300,200, 800, 60);
					motor_daikei_turn_right(200, 200, 200, 500, 68.5);
					motor_daikei_back_no_PID(100, 300, 100, 500, 68);
					motor_daikei_no_PID(100, 300, 100, 500, 35);
					motor_daikei_turn_right(200, 200, 200, 500, 68.5);
					motor_daikei_back_no_PID(100, 300, 100, 500, 100);
			}
			else{
				motor_daikei(300, 300,200, 800, 60);
				motor_daikei_turn_left(200, 200, 200, 500, 128);
				motor_daikei_back_no_PID(100, 500, 100, 500, 60);
			}
		}
		else{
			motor_daikei(300, 300,200, 800, 60);
			motor_daikei_turn_left(200, 200, 200, 500, 128);
			motor_daikei_back_no_PID(100, 500, 100, 500, 60);
		}
		if (playerDirection == UP)
		{
			playerDirection = DOWN;
		}
		else if (playerDirection == DOWN)
		{
			playerDirection = UP;
		}
		else if (playerDirection == RIGHT)
		{
			playerDirection = LEFT;
		}
		else if (playerDirection == LEFT)
		{
			playerDirection = RIGHT;
		}
}

void saitan() {

	// INPUT255
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			map[i][j] = walk;
		}
	}

	// goal_X,Y
	map[goal_y_1][goal_x_1] = 0;
	map[goal_y_1][goal_x_2] = 0;
	map[goal_y_2][goal_x_1] = 0;
	map[goal_y_2][goal_x_2] = 0;

	// walk_count = 0;

	STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
	STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

	pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_1);
	pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_2);
	pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_1);
	pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_2);

	// caluculate
	for (adati_count = 0; adati_count <= 256; adati_count++) {
		XY = popStack_walk(&stack_walk_x, &stack_walk_y);

		// スタックが空の場合はループを終了する
		if (XY.x == 65535 && XY.y == 65535) {
			break;
		}

		if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) { // 左
			if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) { // 下
			if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
				}
			}
		}
		if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) { // 右
			if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) { // 上
			if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
				}
			}
		}
	}
	printMaze();

	saitan_count=0;
	saitan_X = START_POS_X;
	saitan_Y = START_POS_Y+1;
	playerDirection = UP;

	while (1) {
		if (map[saitan_Y][saitan_X] == map[saitan_Y + 1][saitan_X] + 1 && wall_x[saitan_Y + 1][saitan_X] != '-') {
			saitan_Y++;
			if (playerDirection == UP) {
				if(saitan_F_flag==1){
					move_saitan[saitan_count]++;
				}
				else{
					saitan_F_flag=1;
					saitan_count++;
					move_saitan[saitan_count] = 10;
				}
			}
			else if (playerDirection == DOWN) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == RIGHT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == LEFT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 2;
			}
			playerDirection = UP;
		}
		//みぎ
		else if (map[saitan_Y][saitan_X] == map[saitan_Y][saitan_X + 1] + 1 && wall_y[saitan_Y][saitan_X + 1] != '/') {
			saitan_X++;
			if (playerDirection == UP) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == DOWN) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == RIGHT) {
				if(saitan_F_flag==1){
					move_saitan[saitan_count]++;
				}
				else{
					saitan_F_flag=1;
					saitan_count++;
					move_saitan[saitan_count] = 10;
				}
			}
			else if (playerDirection == LEFT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 4;
			}
			playerDirection = RIGHT;
		}
		//ひだり
		else if (map[saitan_Y][saitan_X] == map[saitan_Y][saitan_X - 1] + 1 && wall_y[saitan_Y][saitan_X] != '/') {
			saitan_X--;
			if (playerDirection == UP) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == DOWN) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == RIGHT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == LEFT) {
				if(saitan_F_flag==1){
					move_saitan[saitan_count]++;
				}
				else{
					saitan_F_flag=1;
					saitan_count++;
					move_saitan[saitan_count] = 10;
				}
			}
			playerDirection = LEFT;
		}
		//した
		else if (map[saitan_Y][saitan_X] == map[saitan_Y - 1][saitan_X] + 1 && wall_x[saitan_Y][saitan_X] != '-') {
			saitan_Y--;
			if (playerDirection == UP) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == DOWN) {
				if(saitan_F_flag==1){
					move_saitan[saitan_count]++;
				}
				else{
					saitan_F_flag=1;
					saitan_count++;
					move_saitan[saitan_count] = 10;
				}
			}
			else if (playerDirection == RIGHT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == LEFT) {
				saitan_F_flag=0;
				saitan_count++;
				move_saitan[saitan_count] = 3;
			}
			playerDirection = DOWN;
		}
		else if (map[saitan_Y][saitan_X] == 0|| map[saitan_Y][saitan_X] == 255) {
			break;
		}
/*		saitan_count++;*/
	}


	//おおまわり
	for (int STEP = 1; STEP <= saitan_count; STEP++) {
	    if (10 <= move_saitan[STEP] && move_saitan[STEP] < 30) {
	        if (move_saitan[STEP + 1] == 2 && move_saitan[STEP + 2] == 2
	            && 10 <= move_saitan[STEP + 3] && move_saitan[STEP + 3] < 30) {
	            move_saitan[STEP]--;
	            move_saitan[STEP + 1] = 200;
	            move_saitan[STEP + 2] = 200;

	            moveArrayElementsBack(move_saitan, STEP+1, saitan_count);
	            saitan_count--;
	        }

	        if (move_saitan[STEP + 1] == 3 && move_saitan[STEP + 2] == 3
	            && 10 <= move_saitan[STEP + 3] && move_saitan[STEP + 3] < 30) {


	            move_saitan[STEP]--;
	            move_saitan[STEP + 1] = 300;
	            move_saitan[STEP + 2] = 300;
	               move_saitan[STEP + 3]--;

	            moveArrayElementsBack(move_saitan, STEP + 1, saitan_count);
	            saitan_count--;
	        }
	    }
	    while (1) {
	        int flag_9=0;
	        for (int STEP = 1; STEP <= saitan_count; STEP++) {
	            if (move_saitan[STEP] == 9) {
	                flag_9 = 1;
	                moveArrayElementsBack(move_saitan, STEP, saitan_count);
		            saitan_count--;
	            }
	        }
	        if (flag_9 != 1) {
	            break;
	        }
	    }
	}
    printf("\n配列の中身2:\n");
    for (int STEP = 1; STEP <= saitan_count; STEP++) {
        printf("%d ", move_saitan[STEP]);
    }

    if(10<=move_saitan[1]&&move_saitan[1]<30 ){
    	saitan_move_acc();
    	move_saitan[1]--;
    }else{
    	saitan_move_slalom_R();
    }
	for (int STEP = 1; STEP <= saitan_count; STEP++) {
		if (10<=move_saitan[STEP]&&move_saitan[STEP]<30 ) {
			for(int i=10;i<=move_saitan[STEP];i++){
				//printf("\n\r%d_moveone\n\r",i-9);
				saitan_daikei(700, 700, 700, 600, 180);
			}

			saitan_move_gen();
		}else if (move_saitan[STEP] == 200) {
/*			printf("omawariR");*/
			saitan_move_O_R_180();
			if(10<=move_saitan[STEP+1]&&move_saitan[STEP+1]<30){
				saitan_move_acc();
				//printf("800\n\r");
			}else if(move_saitan[STEP+1] == 200){
				saitan_move_one_500();
				//printf("600_R\n\r");
			}else if(move_saitan[STEP+1] == 300){
				saitan_move_one_500();
				//printf("600_L\n\r");
			}else if(move_saitan[STEP+1] == 2){
				saitan_move_one_500();
				//printf("500_R\n\r");
			}
			else if(move_saitan[STEP+1] == 3){
				saitan_move_one_500();
				//printf("500_L\n\r");
			}
			else{
				//printf("バグ\n\r");
			}
		}
		else if (move_saitan[STEP] == 300) {
			//printf("omawariL");
			saitan_move_O_L_180();
			if(10<=move_saitan[STEP+1]&&move_saitan[STEP+1]<30){
				//printf("800\n\r");
				saitan_move_acc();
			}else if(move_saitan[STEP+1] == 200){
				saitan_move_one_500();
				//printf("600_R\n\r");
			}else if(move_saitan[STEP+1] == 300){
				saitan_move_one_500();
				//printf("600_L\n\r");
			}else if(move_saitan[STEP+1] == 2){
				saitan_move_one_500();
				//printf("500_R\n\r");
			}
			else if(move_saitan[STEP+1] == 3){
				saitan_move_one_500();
				//printf("500_L\n\r");
			}
			else{
				printf("バグ\n\r");
			}
		}else if(move_saitan[STEP]==2){
			//printf("slalom_R");
			saitan_move_slalom_R();
			if(10<=move_saitan[STEP+1]&&move_saitan[STEP+1]<30){
				saitan_move_acc();
				//printf("800\n\r");
			}else if(move_saitan[STEP+1] == 200){
				saitan_move_one_500();
				//printf("600_R\n\r");
			}else if(move_saitan[STEP+1] == 300){
				saitan_move_one_500();
				//printf("600_L\n\r");
			}else if(move_saitan[STEP+1] == 2){
				saitan_move_one_500();
				//printf("500_R\n\r");
			}
			else if(move_saitan[STEP+1] == 3){
				saitan_move_one_500();
				//printf("500_L\n\r");
			}
			else{
				printf("バグ\n\r");
			}
		}else if(move_saitan[STEP]==3){
			//printf("slalom_L");
			saitan_move_slalom_R();
			if(10<=move_saitan[STEP+1]&&move_saitan[STEP+1]<30){
				saitan_move_acc();
				//printf("800\n\r");
			}else if(move_saitan[STEP+1] == 200){
				saitan_move_one_500();
				//printf("600_R\n\r");
			}else if(move_saitan[STEP+1] == 300){
				saitan_move_one_500();
				//printf("600_L\n\r");
			}else if(move_saitan[STEP+1] == 2){
				saitan_move_one_500();
				//printf("500_R\n\r");
			}
			else if(move_saitan[STEP+1] == 3){
				saitan_move_one_500();
				//printf("500_L\n\r");
			}
			else{
				printf("バグ\n\r");
			}
		}
		else{
			printf("ばぐです%d\n",STEP);
		}
	}

}


void saitan_start() {

	// INPUT255
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			map[i][j] = walk;
		}
	}

	// goal_X,Y
	map[START_POS_Y][START_POS_X] = 0;


	// walk_count = 0;

	STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
	STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

	pushStack_walk(&stack_walk_x, &stack_walk_y, START_POS_X, START_POS_Y);


	// caluculate
	for (adati_count = 0; adati_count <= 256; adati_count++) {
		XY = popStack_walk(&stack_walk_x, &stack_walk_y);

		// スタックが空の場合はループを終了する
		if (XY.x == 65535 && XY.y == 65535) {
			break;
		}

		if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) { // 左
			if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) { // 下
			if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
				}
			}
		}
		if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) { // 右
			if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) { // 上
			if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
				}
			}
		}
	}
	//printMaze();
	int saitan_count = 0;
	saitan_X = goal_x_1;
	saitan_Y = goal_y_1;
	saitan_count=0;
	saitan_X = START_POS_X;
	saitan_Y = START_POS_Y;
	playerDirection = UP;

	while (1) {

		if (map[saitan_Y][saitan_X] == map[saitan_Y + 1][saitan_X] + 1 && wall_x[saitan_Y + 1][saitan_X] != '-') {
			saitan_Y++;
			if (playerDirection == UP) {
				move_saitan[saitan_count] = 1;
			}
			else if (playerDirection == DOWN) {
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == RIGHT) {
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == LEFT) {
				move_saitan[saitan_count] = 2;
			}
			playerDirection = UP;
		}
		//みぎ
		else if (map[saitan_Y][saitan_X] == map[saitan_Y][saitan_X + 1] + 1 && wall_y[saitan_Y][saitan_X + 1] != '/') {
			saitan_X++;
			if (playerDirection == UP) {
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == DOWN) {
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == RIGHT) {
				move_saitan[saitan_count] = 1;
			}
			else if (playerDirection == LEFT) {
				move_saitan[saitan_count] = 4;
			}
			playerDirection = RIGHT;
		}
		//ひだり
		else if (map[saitan_Y][saitan_X] == map[saitan_Y][saitan_X - 1] + 1 && wall_y[saitan_Y][saitan_X] != '/') {
			saitan_X--;
			if (playerDirection == UP) {
				move_saitan[saitan_count] = 3;
			}
			else if (playerDirection == DOWN) {
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == RIGHT) {
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == LEFT) {
				move_saitan[saitan_count] = 1;
			}
			playerDirection = LEFT;
		}
		//した
		else if (map[saitan_Y][saitan_X] == map[saitan_Y - 1][saitan_X] + 1 && wall_x[saitan_Y][saitan_X] != '-') {
			saitan_Y--;
			if (playerDirection == UP) {
				move_saitan[saitan_count] = 4;
			}
			else if (playerDirection == DOWN) {
				move_saitan[saitan_count] = 1;
			}
			else if (playerDirection == RIGHT) {
				move_saitan[saitan_count] = 2;
			}
			else if (playerDirection == LEFT) {
				move_saitan[saitan_count] = 3;
			}
			playerDirection = DOWN;
		}
		else if (map[saitan_Y][saitan_X] == map[START_POS_Y][START_POS_X] || map[saitan_Y][saitan_X] == 255) {
			break;
		}
		saitan_count++;
	}
	for (int STEP = 0; STEP < saitan_count; STEP++) {
		//if (move_saitan[STEP] == 1) {
		//	printf("move_180\n");
		//}
		//else if (move_saitan[STEP] == 2) {
		//	printf("slalom_R\n");
		//}
		//else if (move_saitan[STEP] == 3) {
		//	printf("slalom_L\n");
		//}
	}
	kabeate_R();
}

void ZURU(){
	player_pos_X = START_POS_X;
	player_pos_Y = START_POS_Y;
	playerDirection = START_playerDirection;
	//初手移動
		move_sakaime();
		wall_get();
		//tansaku
		for (int move_num = 0; move_num <= 1000; move_num++) {
			//INPUT255
			for (int i = 0; i < WIDTH; i++) {
				for (int j = 0; j < WIDTH; j++) {
					map[i][j] = walk;
				}
			}
			//goal_X,Y
			map[goal_y_1][goal_x_1] = 0;
			map[goal_y_1][goal_x_2] = 0;
			map[goal_y_2][goal_x_1] = 0;
			map[goal_y_2][goal_x_2] = 0;

			//walk_count = 0;

			STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
			STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

			pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_1);
			pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_1, goal_y_2);
			pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_1);
			pushStack_walk(&stack_walk_x, &stack_walk_y, goal_x_2, goal_y_2);

			//caluculate
			for (adati_count = 0; adati_count <= 256; adati_count++) {
				XY = popStack_walk(&stack_walk_x, &stack_walk_y);

				// スタックが空の場合はループを終了する
				if (XY.x == 65535 && XY.y == 65535) {
					break;
				}

				if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) {//左
					if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
						map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
						if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
							pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
						}
					}
				}
				if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) {//下
					if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
						map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
						if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
							pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
						}
					}
				}
				if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) {//右
					if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
						map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
						if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
							pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
						}
					}
				}
				if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) {//上
					if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
						map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
						if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
							pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
						}
					}
				}

			}
		//	printMaze();
/*			kabe_count++;*/
			record_path_X=player_pos_X;
			record_path_Y=player_pos_Y;
			record_map[player_pos_Y][player_pos_X]=1;
			//さいしょ
			while (1) {
				if (Gx >= 20) {
					PID_wall_flag=0;
					player_move_Z();
					break;
				}
			}
			//sakaime
			Gx = 0;
			PID_wall_flag=5;
			wall_get();

			if (map[player_pos_Y][player_pos_X] == walk) {
				break;
			}
			if ((player_pos_X == goal_x_1 || player_pos_X == goal_x_2) && (player_pos_Y == goal_y_1 || player_pos_Y == goal_y_2)) {
				//move_one();
/*				move_one_notret();
				turn_back();*/
				goal_move();
				break;
			}
		}
}
void ZURU2(){

for (int move_num = 0; move_num <= 1000; move_num++) {

	//INPUT255
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			map[i][j] = walk;
		}
	}
	//goal_X,Y
	map[START_POS_Y][START_POS_X] = 0;

	//walk_count = 0;

	STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
	STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

	pushStack_walk(&stack_walk_x, &stack_walk_y, START_POS_X, START_POS_Y);

	//caluculate
	for (adati_count = 0; adati_count <= 256; adati_count++) {
		XY = popStack_walk(&stack_walk_x, &stack_walk_y);

		// スタックが空の場合はループを終了する
		if (XY.x == 65535 && XY.y == 65535) {
			break;
		}

		if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) {//左
			if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) {//下
			if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
				}
			}
		}
		if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) {//右
			if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
				map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
				}
			}
		}
		if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) {//上
			if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
				map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
				if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
					pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
				}
			}
		}

	}
	//kabe_count++;
	record_map[player_pos_Y][player_pos_X]=1;
	wall_get_called = 0;
	Gx_15_flag=0;
	PID_wall_flag=5;
	//さいしょ
	while (1) {
	    if(Gx >= 1 && !wall_get_called) {
	        wall_get();
	        wall_get_called = 1;
	    }
		if (Gx >= 10) {
			PID_wall_flag=0;
/*				player_move_K();*/
			player_move_Z();
			break;
		}
	}

	if (map[player_pos_Y][player_pos_X] == walk) {
		break;
	}
	if (player_pos_X == START_POS_X && player_pos_Y == START_POS_Y) {
		end_wall_flag();
		kabeate_start();
		break;
	}
}
}

void player_move_Z() {
	//うえ
	if (map[player_pos_Y][player_pos_X] == map[player_pos_Y + 1][player_pos_X] + 1 && wall_x[player_pos_Y + 1][player_pos_X] != '-') {
		player_north_Z();
	}
	//みぎ
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X + 1] + 1 && wall_y[player_pos_Y][player_pos_X + 1] != '/') {
		player_east_Z();
	}
	//ひだり
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X - 1] + 1 && wall_y[player_pos_Y][player_pos_X] != '/') {
		player_west_Z();
	}
	//した
	else if (map[player_pos_Y][player_pos_X] == map[player_pos_Y - 1][player_pos_X] + 1 && wall_x[player_pos_Y][player_pos_X] != '-') {
		player_south_Z();
	}

}
/*if (map[player_pos_Y][player_pos_X] == map[player_pos_Y + 1][player_pos_X] + 1 && wall_x[player_pos_Y + 1][player_pos_X] != '-') {
	player_north_K();
}*/

void player_north_Z() {//きたにうごく
	if (playerDirection == UP)
	{
		if(acc_flag==1){
			if ((map[player_pos_Y][player_pos_X] == map[player_pos_Y + 1][player_pos_X] + 1 && wall_x[player_pos_Y + 1][player_pos_X] != '-')&&
							(map[player_pos_Y][player_pos_X] == map[player_pos_Y + 2][player_pos_X] + 2 && wall_x[player_pos_Y + 2][player_pos_X] != '-')){
				if(	MAX_V_count>=8){
					player_move_Z_MAX();
				}else if(	MAX_V_count>=5){
					player_move_Z_MAX_m();
					MAX_V_count++;
				}else{
					player_move_Z_MAX_s();
					MAX_V_count++;
				}
			}
			else{
				player_move_Z_gensoku();
				Gx=0;
				acc_flag=0;
			}
		}

		else if(((map[player_pos_Y][player_pos_X] != map[player_pos_Y+1 ][player_pos_X+1] + 2)||wall_y[player_pos_Y+1 ][player_pos_X+1]=='-')&&
				((map[player_pos_Y][player_pos_X] != map[player_pos_Y+1 ][player_pos_X-1] + 2)||wall_y[player_pos_Y+1 ][player_pos_X]=='-')&&
				(player_pos_Y!=15&&player_pos_Y!=16&&(map[player_pos_Y][player_pos_X] == map[player_pos_Y + 1][player_pos_X] + 1 && wall_x[player_pos_Y + 1][player_pos_X] != '-')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y + 2][player_pos_X] + 2 && wall_x[player_pos_Y + 2][player_pos_X] != '-')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y + 3][player_pos_X] + 3 && wall_x[player_pos_Y + 3][player_pos_X] != '-')&&
				record_map[player_pos_Y+1][player_pos_X]==1&&record_map[player_pos_Y+2][player_pos_X]==1&&record_map[player_pos_Y+3][player_pos_X]==1
				)){
			acc_flag=1;
			player_move_Z_acc();
			slalom_flag=0;
		}else{
			player_move_K();
			slalom_flag=0;
		}
	}
	else if (playerDirection == DOWN) {
		turn_back();
		slalom_flag = 0;
		player_pos_Y += 1;
		playerDirection = UP;
	}
	else if (playerDirection == RIGHT) {
		player_pos_Y += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				slalom_flag=0;
				kabeate_L_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_L();
		}
		playerDirection = UP;
	}
	else if (playerDirection == LEFT) {
		player_pos_Y += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
			kabe_count = 0;
			slalom_flag=0;
			kabeate_R_k();
			motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_R();
		}
		playerDirection = UP;
	}

}
void player_south_Z() {//みなみにうごく
	if (playerDirection == UP) {
		turn_back();
		slalom_flag=0;
		player_pos_Y -= 1;
		playerDirection = DOWN;
	}
	else if (playerDirection == DOWN) {
		if(acc_flag==1){
			if ((map[player_pos_Y][player_pos_X] == map[player_pos_Y-1 ][player_pos_X] + 1 && wall_x[player_pos_Y ][player_pos_X] != '-')&&
							(map[player_pos_Y][player_pos_X] == map[player_pos_Y -2][player_pos_X] + 2 && wall_x[player_pos_Y - 1][player_pos_X] != '-')){
				if(	MAX_V_count>=8){
					player_move_Z_MAX();
				}else if(	MAX_V_count>=5){
					player_move_Z_MAX_m();
					MAX_V_count++;
				}else{
					player_move_Z_MAX_s();
					MAX_V_count++;
				}
			}
			else{
				player_move_Z_gensoku();
				slalom_flag=0;
				Gx=0;
				acc_flag=0;
			}
		}else if(player_pos_Y!=0||player_pos_Y!=1){
			if(((map[player_pos_Y][player_pos_X] != map[player_pos_Y-1][player_pos_X+1] + 2)||wall_y[player_pos_Y-1 ][player_pos_X+1]=='-')&&
			((map[player_pos_Y][player_pos_X] != map[player_pos_Y-1][player_pos_X-1] + 2)||wall_y[player_pos_Y-1 ][player_pos_X]=='-')&&
			(player_pos_Y!=1&&player_pos_Y!=0&&(map[player_pos_Y][player_pos_X] == map[player_pos_Y -1][player_pos_X] + 1 && wall_x[player_pos_Y ][player_pos_X] != '-')&&
					(map[player_pos_Y][player_pos_X] == map[player_pos_Y - 2][player_pos_X] + 2 && wall_x[player_pos_Y - 1][player_pos_X] != '-')&&
					(map[player_pos_Y][player_pos_X] == map[player_pos_Y - 3][player_pos_X] + 3 && wall_x[player_pos_Y - 2][player_pos_X] != '-')&&
					record_map[player_pos_Y-1][player_pos_X]==1&&record_map[player_pos_Y-2][player_pos_X]==1&&record_map[player_pos_Y-3][player_pos_X]==1
					)){
				acc_flag=1;
				slalom_flag=0;
				player_move_Z_acc();
			}else{
				slalom_flag=0;
				player_move_K();
			}
		}
		else{
			slalom_flag=0;
			player_move_K();
		}
		playerDirection = DOWN;
	}
	else if (playerDirection == RIGHT) {
		player_pos_Y -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				slalom_flag=0;
				kabeate_R_k();
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_R();
		}
		playerDirection = DOWN;
	}
	else if (playerDirection == LEFT) {
		player_pos_Y -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				slalom_flag=0;
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_L();
		}
		playerDirection = DOWN;
	}

}
void player_east_Z() {//ひがしにうごく
	if (playerDirection == UP) {
		player_pos_X += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_R_k();
				slalom_flag=0;
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_R();
		}
		playerDirection = RIGHT;
	}
	else if (playerDirection == DOWN) {
		player_pos_X += 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				slalom_flag=0;
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_L();
		}
		playerDirection = RIGHT;
	}
	else if (playerDirection == RIGHT) {
		if(acc_flag==1){
			if ((map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X+1] + 1 && wall_y[player_pos_Y][player_pos_X+1] != '/')&&
							(map[player_pos_Y][player_pos_X] == map[player_pos_Y][player_pos_X+2] + 2 && wall_y[player_pos_Y ][player_pos_X+2] != '/')){
			if(	MAX_V_count>=8){
					player_move_Z_MAX();
				}else if(	MAX_V_count>=5){
					player_move_Z_MAX_m();
					MAX_V_count++;
				}else{
					player_move_Z_MAX_s();
					MAX_V_count++;
				}
			}
			else{
				slalom_flag=0;
				player_move_Z_gensoku();
				Gx=0;
				acc_flag=0;
			}
		}else if ((((map[player_pos_Y][player_pos_X] != map[player_pos_Y+1 ][player_pos_X+1] + 2)||wall_x[player_pos_Y+1 ][player_pos_X+1]=='-')&&
				((map[player_pos_Y][player_pos_X] != map[player_pos_Y-1 ][player_pos_X+1] + 2)||wall_x[player_pos_Y ][player_pos_X+1]=='-')
				&&player_pos_X!=15&&player_pos_X!=16&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X+1] + 1 && wall_y[player_pos_Y ][player_pos_X+1] != '/')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X+2] + 2 && wall_y[player_pos_Y ][player_pos_X+2] != '/')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X+3] + 3 && wall_y[player_pos_Y ][player_pos_X+3] != '/')&&
				record_map[player_pos_Y][player_pos_X+1]==1&&record_map[player_pos_Y][player_pos_X+2]==1&&record_map[player_pos_Y][player_pos_X+3]==1
				)){
			acc_flag=1;
			slalom_flag=0;
			player_move_Z_acc();
		}else{
			slalom_flag=0;
			player_move_K();
		}
		playerDirection = RIGHT;
	}
	else if (playerDirection == LEFT) {
		turn_back();
		slalom_flag=0;
		player_pos_X += 1;
		playerDirection = RIGHT;
	}
}
void player_west_Z() {//にしにうごく
	if (playerDirection == UP) {
		player_pos_X -= 1;
			if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[2] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_L_k();
				slalom_flag=0;
				motor_daikei(100, 400, 400, 600, 120);
			}else{
				slalom_flag=1;
				move_slalom_L();
			}
		}
	else if (playerDirection == DOWN) {
		player_pos_X -= 1;
		if ((wall_flag[0] == 1 || wall_flag[3] == 1) && wall_flag[1] == 1&&kabe_count >= kabeate_count) {
				kabe_count = 0;
				kabeate_R_k();
				slalom_flag=0;
				motor_daikei(100, 400, 400, 600, 120);
		}else{
			slalom_flag=1;
			move_slalom_R();
		}
	}
	else if (playerDirection == RIGHT) {
		turn_back();
		slalom_flag=0;
		player_pos_X -= 1;
	}
	else if (playerDirection == LEFT) {
		if(acc_flag==1){
			if ((map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X-1] + 1 && wall_y[player_pos_Y ][player_pos_X] != '/')&&
							(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X-2] + 2 && wall_y[player_pos_Y ][player_pos_X-1] != '/')){
				if(	MAX_V_count>=8){
					player_move_Z_MAX();
				}else if(	MAX_V_count>=5){
					player_move_Z_MAX_m();
					MAX_V_count++;
				}else{
					player_move_Z_MAX_s();
					MAX_V_count++;
				}
			}
			else{
				slalom_flag=0;
				player_move_Z_gensoku();
				Gx=0;
				acc_flag=0;
			}
		}else if ((((map[player_pos_Y][player_pos_X] != map[player_pos_Y-1 ][player_pos_X-1] + 2)||wall_x[player_pos_Y ][player_pos_X-1]=='-')&&
				((map[player_pos_Y][player_pos_X] != map[player_pos_Y+1 ][player_pos_X-1] + 2)||wall_x[player_pos_Y+1 ][player_pos_X-1]=='-'))
				&&player_pos_X!=0&&player_pos_X!=1
				&&(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X-1] + 1 && wall_y[player_pos_Y ][player_pos_X] != '/')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X-2] + 2 && wall_y[player_pos_Y ][player_pos_X-1] != '/')&&
				(map[player_pos_Y][player_pos_X] == map[player_pos_Y ][player_pos_X-3] + 3 && wall_y[player_pos_Y ][player_pos_X-2] != '/')&&
				record_map[player_pos_Y][player_pos_X-1]==1&&record_map[player_pos_Y][player_pos_X-2]==1&&record_map[player_pos_Y][player_pos_X-3]==1
				){
			acc_flag=1;
			slalom_flag=0;
			player_move_Z_acc();
		}else{
			slalom_flag=0;
			player_move_K();
		}
	}
	playerDirection = LEFT;
}

void player_move_Z_acc(){
	motor_daikei_acc_Z(400, V_500, V_500,800, 160);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X +=1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}
void player_move_Z_MAX(){
	saitan_motor_daikei(V_500, V_500, V_500, 600, 170);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X +=1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}
void player_move_Z_MAX_s(){
	saitan_motor_daikei(V_500, V_500, V_500, 600, 162);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X +=1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}
void player_move_Z_MAX_m(){
	saitan_motor_daikei(V_500, V_500, V_500, 600, 180);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X +=1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}

void player_move_Z_gensoku(){
	saitan_motor_daikei_gensoku(V_500,V_500, 400, 800, 158);
	if (playerDirection == UP)
	{
		player_pos_Y += 1;
	}
	else if (playerDirection == DOWN)
	{
		player_pos_Y -= 1;
	}
	else if (playerDirection == RIGHT)
	{
		player_pos_X +=1;
	}
	else if (playerDirection == LEFT)
	{
		player_pos_X -= 1;
	}
}

void goal_move(){
	record_path_X=player_pos_X;
	record_path_Y=player_pos_Y;
	record_map[player_pos_Y][player_pos_X]=1;
	wall_get();
	move_one();
	record_path_X=player_pos_X;
	record_path_Y=player_pos_Y;
	record_map[player_pos_Y][player_pos_X]=1;
	wall_get();
	kabeate_goal();
	//INPUT255
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < WIDTH; j++) {
				map[i][j] = walk;
			}
		}
		map[START_POS_Y][START_POS_X] = 0;


		STACK_T_x stack_walk_x = { .head_x = 0, .tail_x = 0 };  // head_xとtail_xメンバーを0
		STACK_T_y stack_walk_y = { .head_y = 0, .tail_y = 0 };  // head_yとtail_yメンバーを0

		pushStack_walk(&stack_walk_x, &stack_walk_y, START_POS_X, START_POS_Y);

		for (adati_count = 0; adati_count <= 256; adati_count++) {
			XY = popStack_walk(&stack_walk_x, &stack_walk_y);

			if (XY.x == 65535 && XY.y == 65535) {
				break;
			}

			if (wall_y[XY.y][XY.x] == ' ' && XY.x != 0) {
				if (map[XY.y][XY.x - 1] > map[XY.y][XY.x]) {
					map[XY.y][XY.x - 1] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x - 1, XY.y);
					}
				}
			}
			if (wall_x[XY.y][XY.x] == ' ' && XY.y != 0) {
				if (map[XY.y - 1][XY.x] > map[XY.y][XY.x]) {
					map[XY.y - 1][XY.x] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y - 1);
					}
				}
			}
			if (wall_y[XY.y][XY.x + 1] == ' ' && XY.x != 15) {
				if (map[XY.y][XY.x + 1] > map[XY.y][XY.x]) {
					map[XY.y][XY.x + 1] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x + 1, XY.y);
					}
				}
			}
			if (wall_x[XY.y + 1][XY.x] == ' ' && XY.y != 15) {
				if (map[XY.y + 1][XY.x] > map[XY.y][XY.x]) {
					map[XY.y + 1][XY.x] = map[XY.y][XY.x] + 1;
					if (!isAlreadyPushed(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1)) {
						pushStack_walk(&stack_walk_x, &stack_walk_y, XY.x, XY.y + 1);
					}
				}
			}
		}
		record_map[player_pos_Y][player_pos_X]=1;
		move_sakaime();


}

void move_saitan_1000(){
	saitan_move_sakaime();
	for (int STEP = 1; STEP <= saitan_count; STEP++) {
			if (9<=move_saitan[STEP] &&move_saitan[STEP] < 30) {//10=180
				for(int STEP_count=10;STEP_count<=move_saitan[STEP];STEP_count++){
					saitan_move_one_no_PID();
				}
			}else if (move_saitan[STEP] == 2) {
				Gx=0;
				while(Gx>=20){
					break;
				}
				saitan_move_slalom_R() ;
			}
			else if (move_saitan[STEP] == 3) {
				Gx=0;
				while(Gx>=20){
					break;
				}
				saitan_move_slalom_L() ;
		}
		goal_move();
	}
}

void move_saitan_1000_return(){
	for (int STEP =saitan_count; STEP >=1; STEP--) {
			if (9<=move_saitan[STEP] &&move_saitan[STEP] < 30) {//10=180
				for(int STEP_count=10;STEP_count<=move_saitan[STEP];STEP_count++){
					saitan_move_one() ;
				}
			}else if (move_saitan[STEP] == 2) {
				Gx=0;
				while(Gx>=20){
					break;
				}
				saitan_move_slalom_L() ;
			}
			else if (move_saitan[STEP] == 3) {
				Gx=0;
				while(Gx>=20){
					break;
				}
				saitan_move_slalom_R() ;
		}
		goal_move();
	}
}

void moveArrayElementsBack(int array[], int start, int end) {
    for (int i = start; i <= end; i++) {
        array[i] = array[i+1];
    }
}

void	end_wall_flag(){
	for(int i=0;i<=16;i++) {
		for(int j=0;j<=16;j++){
			printf("%d",wall_X_flag[i][j]);
			if(	wall_X_flag[i][j] ==0){
				wall_x[i ][j] = '-';
			}
			if(wall_Y_flag[i][j] ==0){
				wall_y[i ][j] = '/';
			}
		}
	}
}

//wall_x[2][1] = '-';
//wall_x[1][2] = '-';
//wall_x[2][3] = '-';
//wall_x[4][3] = '-';
//wall_x[6][3] = '-';
//wall_x[12][3] = '-';
//wall_x[15][3] = '-';
//wall_x[1][4] = '-';
//wall_x[3][4] = '-';
//wall_x[5][4] = '-';
//wall_x[7][4] = '-';
//wall_x[9][4] = '-';
//wall_x[10][4] = '-';
//wall_x[12][4] = '-';
//wall_x[13][4] = '-';
//wall_x[14][4] = '-';
//wall_x[15][4] = '-';
//wall_x[1][5] = '-';
//wall_x[2][5] = '-';
//wall_x[4][5] = '-';
//wall_x[6][5] = '-';
//wall_x[8][5] = '-';
//wall_x[9][5] = '-';
//wall_x[10][5] = '-';
//wall_x[11][5] = '-';
//wall_x[14][5] = '-';
//wall_x[15][5] = '-';
//wall_x[1][6] = '-';
//wall_x[3][6] = '-';
//wall_x[5][6] = '-';
//wall_x[8][6] = '-';
//wall_x[10][6] = '-';
//wall_x[12][6] = '-';
//wall_x[13][6] = '-';
//wall_x[14][6] = '-';
//wall_x[15][6] = '-';
//wall_x[1][7] = '-';
//wall_x[2][7] = '-';
//wall_x[3][7] = '-';
//wall_x[4][7] = '-';
//wall_x[6][7] = '-';
//wall_x[7][7] = '-';
//wall_x[9][7] = '-';
//wall_x[11][7] = '-';
//wall_x[13][7] = '-';
//wall_x[15][7] = '-';
//wall_x[7][8] = '-';
//wall_x[9][8] = '-';
//wall_x[13][8] = '-';
//wall_x[15][8] = '-';
//wall_x[6][9] = '-';
//wall_x[8][9] = '-';
//wall_x[10][9] = '-';
//wall_x[15][9] = '-';
//wall_x[1][10] = '-';
//wall_x[2][10] = '-';
//wall_x[3][10] = '-';
//wall_x[5][10] = '-';
//wall_x[9][10] = '-';
//wall_x[10][10] = '-';
//wall_x[11][10] = '-';
//wall_x[13][10] = '-';
//wall_x[14][10] = '-';
//wall_x[1][11] = '-';
//wall_x[2][11] = '-';
//wall_x[3][11] = '-';
//wall_x[4][11] = '-';
//wall_x[5][11] = '-';
//wall_x[10][11] = '-';
//wall_x[13][11] = '-';
//wall_x[14][11] = '-';
//wall_x[15][11] = '-';
//wall_x[3][12] = '-';
//wall_x[4][12] = '-';
//wall_x[5][12] = '-';
//wall_x[9][12] = '-';
//wall_x[11][12] = '-';
//wall_x[12][12] = '-';
//wall_x[14][12] = '-';
//wall_x[1][13] = '-';
//wall_x[4][13] = '-';
//wall_x[5][13] = '-';
//wall_x[8][13] = '-';
//wall_x[9][13] = '-';
//wall_x[10][13] = '-';
//wall_x[12][13] = '-';
//wall_x[13][13] = '-';
//wall_x[15][13] = '-';
//wall_x[2][14] = '-';
//wall_x[4][14] = '-';
//wall_x[5][14] = '-';
//wall_x[6][14] = '-';
//wall_x[7][14] = '-';
//wall_x[8][14] = '-';
//wall_x[11][14] = '-';
//wall_x[14][14] = '-';
//wall_y[1][0] = '/';
//for (i = 2; i <= 14; i++) {
//	wall_y[i][1] = '/';
//}
//for (i = 1; i <= 4; i++) {
//	wall_y[i][2] = '/';
//}
//for (i = 6; i <= 15; i++) {
//	wall_y[i][2] = '/';
//}
//for (i = 1; i <= 1; i++) {
//	wall_y[i][3] = '/';
//}
//for (i = 3; i <= 3; i++) {
//	wall_y[i][3] = '/';
//}
//for (i = 5; i <= 5; i++) {
//	wall_y[i][3] = '/';
//}
//for (i = 7; i <= 9; i++) {
//	wall_y[i][3] = '/';
//}
//for (i = 11; i <= 14; i++) {
//	wall_y[i][3] = '/';
//}
//for (i = 2; i <= 2; i++) {
//	wall_y[i][4] = '/';
//}
//for (i = 4; i <= 4; i++) {
//	wall_y[i][4] = '/';
//}
//for (i = 6; i <= 7; i++) {
//	wall_y[i][4] = '/';
//}
//for (i = 10; i <= 11; i++) {
//	wall_y[i][4] = '/';
//}
//for (i = 13; i <= 13; i++) {
//	wall_y[i][4] = '/';
//}
//for (i = 1; i <= 1; i++) {
//	wall_y[i][5] = '/';
//}
//for (i = 3; i <= 3; i++) {
//	wall_y[i][5] = '/';
//}
//for (i = 5; i <= 5; i++) {
//	wall_y[i][5] = '/';
//}
//for (i = 7; i <= 7; i++) {
//	wall_y[i][5] = '/';
//}
//wall_y[2][6] = '/';
//wall_y[4][6] = '/';
//wall_y[6][6] = '/';
//wall_y[8][6] = '/';
//wall_y[3][7] = '/';
//wall_y[5][7] = '/';
//for (i = 7; i <= 9; i++) {
//	wall_y[i][7] = '/';
//}
//wall_y[13][7] = '/';
//wall_y[1][8] = '/';
//wall_y[3][8] = '/';
//wall_y[4][8] = '/';
//wall_y[6][8] = '/';
//wall_y[9][8] = '/';
//wall_y[11][8] = '/';
//wall_y[14][8] = '/';
//for (i = 1; i <= 5; i++) {
//	wall_y[i][9] = '/';
//}
//wall_y[7][9] = '/';
//for (i = 10; i <= 12; i++) {
//	wall_y[i][9] = '/';
//}
//wall_y[14][9] = '/';
//wall_y[1][10] = '/';
//wall_y[3][10] = '/';
//wall_y[4][10] = '/';
//wall_y[6][10] = '/';
//wall_y[8][10] = '/';
//wall_y[11][10] = '/';
//wall_y[6][11] = '/';
//wall_y[7][11] = '/';
//wall_y[9][11] = '/';
//wall_y[12][11] = '/';
//wall_y[15][11] = '/';
//wall_y[1][12] = '/';
//for (i = 6; i <= 8; i++) {
//	wall_y[i][12] = '/';
//}
//wall_y[10][12] = '/';
//wall_y[12][12] = '/';
//wall_y[15][12] = '/';
//wall_y[0][13] = '/';
//wall_y[2][13] = '/';
//wall_y[6][13] = '/';
//wall_y[7][13] = '/';
//wall_y[9][13] = '/';
//wall_y[11][13] = '/';
//wall_y[1][14] = '/';
//wall_y[3][14] = '/';
//wall_y[5][14] = '/';
//wall_y[7][14] = '/';
//wall_y[9][14] = '/';
//wall_y[10][14] = '/';
//wall_y[12][14] = '/';
//wall_y[13][14] = '/';
//wall_y[1][15] = '/';
//wall_y[5][15] = '/';
//wall_y[7][15] = '/';
//wall_y[9][15] = '/';
//wall_y[11][15] = '/';
//wall_y[5][15] = '/';
//wall_y[7][15] = '/';
//wall_y[9][15] = '/';
//wall_y[11][15] = '/';
//wall_y[13][15] = '/';
//wall_y[14][15] = '/';
