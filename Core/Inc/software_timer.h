/*
 * software_timer.h
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"
#include <stdlib.h>
#include "input_reading.h"

#define TIMER_CLOCK_FREQ 8000

typedef enum {
	START = 0,
	STOP = 1
} control_timer;

void init_timer(TIM_HandleTypeDef *tim, unsigned int num_sec, unsigned int num_mil);
uint8_t setMilTimer(uint8_t timer, unsigned int value);
uint8_t setSecTimer(uint8_t timer, unsigned int value);
uint8_t milFlag(uint8_t timer);
uint8_t secFlag(uint8_t timer);
unsigned int milCounter(uint8_t timer);
unsigned int secCounter(uint8_t timer);
void resetMilFlag(uint8_t timer);
void resetSecFlag(uint8_t timer);
void controlMilTimer(uint8_t timer, control_timer control);
void controlSecTimer(uint8_t timer, control_timer control);

#endif /* INC_SOFTWARE_TIMER_H_ */
