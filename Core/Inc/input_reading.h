/*
 * input_reading.h
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"
#include <stdlib.h>

void init_btn_reading(const uint16_t* port, unsigned int no_of_btn);
void btn_reading(void);
unsigned char is_btn_pressed(uint8_t index);

#endif /* INC_INPUT_READING_H_ */
