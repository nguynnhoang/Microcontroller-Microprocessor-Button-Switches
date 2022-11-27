/*
 * input_processing.h
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "Display7SEG.h"
#include "main.h"
#include <stdlib.h>
#include "software_timer.h"
#include "traffic_control.h"


typedef enum {
	BTN_MODE,
	BTN_FUNC
} BTN_array_access;

void init_fsm_input_processing(TIM_HandleTypeDef* htim);
void fsm_for_input_processing(void);
void fsm_for_traffic_control(void);
void setBuffer7SEG(uint8_t firstPair, uint8_t secondPair);
void processing(BTN_array_access button);
void scan7SEG(void);
void increase(uint8_t* buffer);
void decrease(uint8_t* buffer);
void blinkLEDTraffic(void);

#endif /* INC_INPUT_PROCESSING_H_ */
