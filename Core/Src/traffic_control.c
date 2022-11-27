/*
 * traffic_control.c
 *
 *  Created on: Nov 13, 2022
 *      Author: MSI
 */

#include "traffic_control.h"

typedef enum {
	STATE_RED,
	STATE_GREEN,
	STATE_YELLOW,
	STATE_UPDATE,
} traffic_state;

// fsm state for display traffic lights(vertical road: 13 and horizontal road: 24)
static traffic_state state13, state24;

// provided by user: Array of port for traffic lights
static const uint16_t* lightPort;

// provided by user: Array of time value for each traffic light
static uint8_t* lightTime;
static uint8_t* lightTimeUpdate;


static uint16_t allpinLED; // represent for all traffic lights pin
static uint8_t isUpdate = 0;

void copyArray(uint8_t* from, uint8_t* to, int n) {
	for(int i = 0; i < n; i++) {
		to[i] = from[i];
	}
}

void init_traffic_control(const uint16_t* port, uint8_t* time, uint8_t* timeUpdate) {
	lightPort = port;//Array of GPIO port for traffic lights
	lightTime = time;// Array of time interval for each traffic light
	lightTimeUpdate = timeUpdate;

	state13 = STATE_YELLOW;
	state24 = STATE_RED;

	allpinLED = lightPort[0] | lightPort[1] | lightPort[2] | lightPort[3] | lightPort[4] | lightPort[5];
	HAL_GPIO_WritePin(LED_Port, allpinLED, GPIO_PIN_SET);
}

//An FSM for displaying traffic lights according to its lighting time.
void traffic_display(void) {
	switch(state13) {
	case STATE_RED:
		if(secFlag(TRAFFIC_13_TIMER)) {
			setSecTimer(TRAFFIC_13_TIMER, lightTime[GREEN_13]);
			state13 = STATE_GREEN;
			clear_traffic_control();
			break;
		}

		HAL_GPIO_WritePin(LED_Port, lightPort[RED_13], GPIO_PIN_RESET);
		break;
	case STATE_GREEN:
		if(secFlag(TRAFFIC_13_TIMER)) {
			setSecTimer(TRAFFIC_13_TIMER, lightTime[YELLOW_13]);
			state13 = STATE_YELLOW;
			clear_traffic_control();
			break;
		}
		HAL_GPIO_WritePin(LED_Port, lightPort[GREEN_13], GPIO_PIN_RESET);
		break;
	case STATE_YELLOW:
		if(secFlag(TRAFFIC_13_TIMER)) {
			state13 = STATE_UPDATE;
			clear_traffic_control();
			break;
		}
		HAL_GPIO_WritePin(LED_Port, lightPort[YELLOW_13],GPIO_PIN_RESET);
		break;
	case STATE_UPDATE:
		if(getUpdateFlag()) {
			copyArray(lightTimeUpdate, lightTime, 6);
		}
		setSecTimer(TRAFFIC_13_TIMER, lightTime[RED_13]);
		state13 = STATE_RED;
		break;
	}

	switch(state24) {
	case STATE_GREEN:
		if(secFlag(TRAFFIC_24_TIMER)) {
			setSecTimer(TRAFFIC_24_TIMER, lightTime[YELLOW_24]);
			state24 = STATE_YELLOW;
			clear_traffic_control();
			break;
		}
		HAL_GPIO_WritePin(LED_Port, lightPort[GREEN_24], GPIO_PIN_RESET);
		break;
	case STATE_YELLOW:
		if(secFlag(TRAFFIC_24_TIMER)) {
			setSecTimer(TRAFFIC_24_TIMER, lightTime[RED_24]);
			state24 = STATE_RED;
			clear_traffic_control();
			break;
		}
		HAL_GPIO_WritePin(LED_Port, lightPort[YELLOW_24], GPIO_PIN_RESET);
		break;
	case STATE_RED:
		if(secFlag(TRAFFIC_24_TIMER)) {
			state24 = STATE_UPDATE;
			clear_traffic_control();
			break;
		}
		HAL_GPIO_WritePin(LED_Port, lightPort[RED_24], GPIO_PIN_RESET);
		break;
	case STATE_UPDATE:
		if(getUpdateFlag()) {
			copyArray(lightTimeUpdate, lightTime, 6);
		}
		setSecTimer(TRAFFIC_24_TIMER, lightTime[GREEN_24]);
		state24 = STATE_GREEN;
		break;
	}

}

//turn off all traffic lights.
void clear_traffic_control(void) {
	HAL_GPIO_WritePin(LED_Port, allpinLED, GPIO_PIN_SET);
}

//check if the display is done its period.
uint8_t getUpdateFlag(void) {
	if(isUpdate) {
		isUpdate = 0;
		return 1;
	}
	return 0;//A flag to indicate the period is done
}
void updateNextPeriod(void) {
	isUpdate = 1;
}
