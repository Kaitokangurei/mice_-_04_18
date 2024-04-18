/*
 * PL_timer.h
 *
 *  Created on: Nov 17, 2023
 *      Author: ovqxp
 */

#ifndef INC_PL_TIMER_H_
#define INC_PL_TIMER_H_

#include "stm32l4xx_hal.h"

extern volatile uint32_t g_timCount;

void pl_timer_init(void);
void pl_timer_count(void);

void wait_ms(uint32_t);

#endif /* SRC_PL_TIMER_H_ */
