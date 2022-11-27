/*
 * software_timer.c
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#include "software_timer.h"

static unsigned int TIMER_CYCLE; // Timer interrupt period
static unsigned int NO_OF_MIL_TIMERS; // number of timer run in millisecond
static unsigned int NO_OF_SEC_TIMERS; // number of timer run in second

//store counter
static unsigned int* timer_mil_counter;
static unsigned int* timer_sec_counter;

//store timeout flag
static uint8_t* timer_mil_flag;
static uint8_t* timer_sec_flag;

//store stop flag
static uint8_t* timer_mil_stop;
static uint8_t* timer_sec_stop;


//Initialize second based timers and millisecond based timers
void init_timer(TIM_HandleTypeDef *tim, unsigned int num_sec, unsigned int num_mil) {
	// initialize parameter for software timer
	TIMER_CYCLE = (tim->Instance->ARR+1) * (tim->Instance->PSC+1) / TIMER_CLOCK_FREQ;
	NO_OF_MIL_TIMERS = num_mil;//num_sec: number of second based timers
	NO_OF_SEC_TIMERS = num_sec;//num_mil: number of millisecond based timers
	// initialize array for counter and flag
	timer_mil_counter = (unsigned int*)malloc(NO_OF_MIL_TIMERS * sizeof(unsigned int));
	timer_sec_counter = (unsigned int*)malloc(NO_OF_SEC_TIMERS * sizeof(unsigned int));
	timer_mil_flag = (uint8_t*)malloc(NO_OF_MIL_TIMERS * sizeof(uint8_t));
	timer_sec_flag = (uint8_t*)malloc(NO_OF_SEC_TIMERS * sizeof(uint8_t));
	timer_mil_stop = (uint8_t*)malloc(NO_OF_MIL_TIMERS * sizeof(uint8_t));
	timer_sec_stop = (uint8_t*)malloc(NO_OF_SEC_TIMERS * sizeof(uint8_t));
	// init value for array
	for(int i = 0; i < NO_OF_MIL_TIMERS; i++) {
		timer_mil_counter[i] = 0;
		timer_mil_flag[i] = 1;
		timer_mil_stop[i] = START;
	}
	for(int i = 0; i < NO_OF_SEC_TIMERS; i++) {
		timer_sec_counter[i] = 0;
		timer_sec_flag[i] = 1;
		timer_sec_stop[i] = STOP;
	}
}


//Begin a specified timer with a countdown value
uint8_t setMilTimer(uint8_t timer, unsigned int time_interval) {
	if((time_interval * 1000) % TIMER_CYCLE != 0) {
		return 0;
	}
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}

	timer_mil_counter[timer] = time_interval / TIMER_CYCLE;
	timer_mil_flag[timer] = 0;
	return 1;
}

uint8_t setSecTimer(uint8_t timer, unsigned int time_interval) {
	if((time_interval * 1000) % TIMER_CYCLE != 0) {
		return 0;
	}
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	timer_sec_counter[timer] = time_interval*1000 / TIMER_CYCLE;
	timer_sec_flag[timer] = 0;
	return 1;
}

//check whether the timer is done its counting down or not
uint8_t milFlag(uint8_t timer) {
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	return timer_mil_flag[timer];//1: count down, otherwise 0
}

uint8_t secFlag(uint8_t timer) {
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	return timer_sec_flag[timer];//1: count down, otherwise 0
}

//get its current counting value in mil timer
unsigned int milCounter(uint8_t timer) {
	if(timer >= NO_OF_MIL_TIMERS) {
		exit(-1);
	}
	return timer_mil_counter[timer];
}


//get its current counting value in sec timer
unsigned int secCounter(uint8_t timer) {
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	return timer_sec_counter[timer] * TIMER_CYCLE / 1000;
}


//clear the done counting flag
void resetMilFlag(uint8_t timer) {
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	timer_mil_flag[timer] = 0;
}

//clear the done counting flag
void resetSecFlag(uint8_t timer) {
	if(timer >= NO_OF_SEC_TIMERS) {
		exit(-1);
	}
	timer_sec_flag[timer] = 0;
}

//stop or continue the mil timer
void controlMilTimer(uint8_t timer, control_timer control) {
	//timer: a specific timer
	timer_mil_stop[timer] = control;//stop or continue command, START or STOP
}

//stop or continue the sec timer
void controlSecTimer(uint8_t timer, control_timer control) {
	//timer: a specific timer
	timer_sec_stop[timer] = control;//stop or continue command, START or STOP
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	btn_reading();
	for(int i = 0; i < NO_OF_MIL_TIMERS; i++) {
		if(timer_mil_stop[i] == STOP) continue;
		if(timer_mil_counter[i] > 0) {
			timer_mil_counter[i]--;
			if(timer_mil_counter[i] == 0) {
				timer_mil_flag[i] = 1;
			}
		}
	}

	for(int i = 0; i < NO_OF_SEC_TIMERS; i++) {
		if(timer_sec_stop[i] == STOP) continue;
		if(timer_sec_counter[i] > 0) {
			timer_sec_counter[i]--;
			if(timer_sec_counter[i] == 0) {
				timer_sec_flag[i] = 1;
			}
		}
	}
}

