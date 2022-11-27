/*
 * Display7SEG.h
 *
 *  Created on: Nov 13, 2022
 *      Author: MSI
 */

#ifndef INC_DISPLAY7SEG_H_
#define INC_DISPLAY7SEG_H_

#include "main.h"
#include <stdlib.h>

void init_7SEG(const uint16_t* en, const uint16_t* port, uint8_t* buffer, unsigned int no_led7);
void display7SEG(unsigned int index);

#endif /* INC_DISPLAY7SEG_H_ */
