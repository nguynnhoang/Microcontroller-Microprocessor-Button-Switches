/*
 * traffic_control.h
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#ifndef INC_TRAFFIC_CONTROL_H_
#define INC_TRAFFIC_CONTROL_H_

#include "main.h"
#include <stdlib.h>
#include "software_timer.h"

typedef enum {
	RED_13,
	YELLOW_13,
	GREEN_13,
	RED_24,
	YELLOW_24,
	GREEN_24

} traffic_array_access;

typedef enum {
	TRAFFIC_13_TIMER,
	TRAFFIC_24_TIMER
} timer_array_access;

void init_traffic_control(const uint16_t* port, uint8_t* time, uint8_t* timeUpdate);
void traffic_display(void);
void clear_traffic_control(void);
uint8_t getUpdateFlag(void);
void updateNextPeriod(void);
void copyArray(uint8_t* from, uint8_t* to, int n);

#endif /* INC_TRAFFIC_CONTROL_H_ */
