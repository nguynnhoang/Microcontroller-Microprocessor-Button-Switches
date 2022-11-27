/*
 * input_processing.c
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#include "input_processing.h"

// Timer parameter
typedef enum {
	SCAN_TIMER,
	BLINK_TIMER
} TimerMilArrayAccess;
const unsigned int NO_OF_MIL_TIMERS = 2;
const unsigned int NO_OF_SEC_TIMERS = 2;
const unsigned int scanTime = 200;
const unsigned int blinkTime = 200;

// LED parameter
#define NO_OF_SEG7 4
const uint16_t seg7_port[7] = {SEG0, SEG1, SEG2, SEG3, SEG4, SEG5, SEG6};
const uint16_t seg7_en[NO_OF_SEG7] = {EN0, EN1, EN2, EN3};
uint8_t buffer_led7[NO_OF_SEG7] = {0,1,2,4};


// Button parameter
#define NO_OF_BUTTONS 2

typedef enum {
	STATE_RELEASED,
	STATE_PRESSED
} ButtonState;
ButtonState* buttonState;


const uint16_t button_port[NO_OF_BUTTONS] = {BTN_MODE, BTN_FUNC};

// Traffic parameter
typedef enum {
	NORMAL,
	PLUS_RED,
	PLUS_GREEN,
	PLUS_YELLOW,
	MINUS_RED,
	MINUS_GREEN,
	MINUS_YELLOW,
	CONFIRM,
} ModeState;
ModeState trafficMode = NORMAL;
const uint16_t traffic_port[6] = {RED_13, GREEN_13, YELLOW_13, RED_24, GREEN_24, YELLOW_24};
uint8_t traffic_time[6] = {9,6,3,9,6,3};
uint8_t traffic_time_update[6] = {};

/**
  * @brief  Initialize input processing
  * @note   None
  *
  * @param  port: Array of GPIO port for buttons
  * @param  no_buttons: Number of buttons
  * @retval None
  *
  */


void init_fsm_input_processing(TIM_HandleTypeDef* htim) {
	init_timer(htim, NO_OF_SEC_TIMERS, NO_OF_MIL_TIMERS);
	init_7SEG(seg7_en, seg7_port, buffer_led7, NO_OF_SEG7);
	init_btn_reading(button_port, NO_OF_BUTTONS);
	init_traffic_control(traffic_port, traffic_time, traffic_time_update);

	buttonState = (ButtonState*)malloc(NO_OF_BUTTONS*sizeof(ButtonState));

	for(int i = 0; i < NO_OF_BUTTONS; i++) {
		 buttonState[i] = STATE_RELEASED;
	}
}



/**
  * @brief 	To read a single click(press then release) of buttons
  * @note   None
  *
  * @param  None
  * @retval None
  */
void fsm_for_input_processing(void) {
	for(int i = 0; i < NO_OF_BUTTONS; i++) {
		switch(buttonState[i]) {
		case STATE_RELEASED:
			if(is_btn_pressed(i)) {
				buttonState[i] = STATE_PRESSED;
			}
			break;
		case STATE_PRESSED:
			if(!is_btn_pressed(i)) {
				buttonState[i] = STATE_RELEASED;
				processing(i);
			}
			break;
		}
	}
}

void fsm_for_traffic_control(void) {
	switch(trafficMode) {
	case NORMAL:
		traffic_display();
		setBuffer7SEG(secCounter(TRAFFIC_13_TIMER), secCounter(TRAFFIC_24_TIMER));
		break;
	case PLUS_RED:
	case PLUS_GREEN:
	case PLUS_YELLOW:
		setBuffer7SEG(traffic_time[trafficMode-1]-1, 2);
		blinkLEDTraffic();
		break;
	case MINUS_RED:
	case MINUS_GREEN:
	case MINUS_YELLOW:
		setBuffer7SEG(traffic_time[trafficMode-4]-1, 3);
		blinkLEDTraffic();
		break;
	case CONFIRM:
		setBuffer7SEG(44, 44);
		blinkLEDTraffic();
		break;
	}
	scan7SEG();
}

void processing(BTN_array_access button) {
	static uint8_t updateFlag = 0;
	switch(button) {
	case BTN_MODE:
		switch(trafficMode) {
		case NORMAL:
			controlSecTimer(TRAFFIC_13_TIMER, STOP);
			controlSecTimer(TRAFFIC_24_TIMER, STOP);
			break;
		case CONFIRM:
			if(updateFlag) {
				updateFlag = 0;
				updateNextPeriod();
			}
			else {
				copyArray(traffic_time,traffic_time_update, 6);
			}
			controlSecTimer(TRAFFIC_13_TIMER, START);
			controlSecTimer(TRAFFIC_24_TIMER, START);
		default:
			break;
		}
		clear_traffic_control();
		trafficMode = (trafficMode != CONFIRM)? trafficMode+1 : NORMAL;
		break;
	case BTN_FUNC:
		switch(trafficMode) {
		case PLUS_RED:
		case PLUS_GREEN:
		case PLUS_YELLOW:
			increase(traffic_time);
			break;
		case MINUS_RED:
		case MINUS_GREEN:
		case MINUS_YELLOW:
			decrease(traffic_time);
			break;
		case CONFIRM:
			updateFlag = 1;
			break;
		default:
			break;
		}
		break;
	}
}


void setBuffer7SEG(uint8_t firstPair, uint8_t secondPair) {
	buffer_led7[0] = firstPair / 10;
	buffer_led7[1] = firstPair % 10;
	buffer_led7[2] = secondPair / 10;
	buffer_led7[3] = secondPair % 10;
}

void scan7SEG() {
	static uint8_t index = 0;

	if(milFlag(SCAN_TIMER)) {
		setMilTimer(SCAN_TIMER, scanTime);
		display7SEG(index++);
		if(index == 4) index = 0;
	}
}

//			RV          GV    YV
//	|-----------------|------|---|
//	|------------|----|----------|
//        GH       YH      RH
void increase(uint8_t* buffer) {
	switch(trafficMode) {
	case PLUS_RED:
		if(buffer[GREEN_13]-1 > 0 && buffer[RED_24]-1 > 1) {
			buffer[RED_13]++;
			buffer[GREEN_13]++;

			buffer[RED_24]++;
			buffer[GREEN_24]++;
		}
		break;
	case PLUS_GREEN:
		if(buffer[RED_13]-1 > 1 && buffer[GREEN_24]-1 > 0) {
			buffer[GREEN_13]++;
			buffer[RED_13]++;

			buffer[GREEN_24]++;
			buffer[RED_24]++;
		}
		break;
	case PLUS_YELLOW:
		if(buffer[GREEN_13]-1 > 0) {
			buffer[YELLOW_13]++;
			buffer[RED_13]++;

			buffer[YELLOW_24]++;
			buffer[RED_24]++;
		}
		break;
	default:
		break;
	}
}

void decrease(uint8_t* buffer) {
	switch(trafficMode) {
	case MINUS_RED:
		if(buffer[RED_13]-1 > 1 && buffer[GREEN_13]-1 > 0) {
			buffer[RED_13]--;
			buffer[GREEN_13]--;

			buffer[RED_24]--;
			buffer[GREEN_24]--;
		}
		break;
	case MINUS_GREEN:
		if(buffer[GREEN_13]-1 > 0 && buffer[RED_24]-1 > 1) {
			buffer[GREEN_13]--;
			buffer[RED_13]--;

			buffer[GREEN_24]--;
			buffer[RED_24]--;
		}
		break;
	case MINUS_YELLOW:
		if(buffer[YELLOW_13]-1 > 0) {
			buffer[YELLOW_13]--;
			buffer[RED_13]--;

			buffer[YELLOW_24]--;
			buffer[RED_24]--;
		}
		break;
	default:
		break;
	}
}

void blinkLEDTraffic() {
	if(trafficMode == NORMAL) return;

	if(milFlag(BLINK_TIMER)) {
		setMilTimer(BLINK_TIMER, blinkTime);
		if(trafficMode == PLUS_RED || trafficMode == MINUS_RED) {
			HAL_GPIO_TogglePin(LED_Port, RED_13 | RED_24);
		}
		else if(trafficMode == PLUS_GREEN || trafficMode == MINUS_GREEN) {
			HAL_GPIO_TogglePin(LED_Port, GREEN_13 | GREEN_24);
		}
		else if(trafficMode == PLUS_YELLOW || trafficMode == MINUS_YELLOW){
			HAL_GPIO_TogglePin(LED_Port, YELLOW_13 | YELLOW_24);
		}
		else if(trafficMode == CONFIRM){
			HAL_GPIO_TogglePin(LED_Port, YELLOW_13 | YELLOW_24 | GREEN_13 | GREEN_24 | RED_13 | RED_24);
		}
	}
}
