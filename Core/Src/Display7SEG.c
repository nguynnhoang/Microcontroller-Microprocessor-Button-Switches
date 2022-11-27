/*
 * Display7SEG.c
 *
 *  Created on: Nov 13, 2022
 *      Author: MSI
 */

#include "Display7SEG.h"

// Hex code for displaying a single led7
static uint8_t seg7Hex[] = {0x01, 0x4F, 0x12, 0x06, 0x4C, 0x24, 0x20, 0x0F, 0x00, 0x04, 0xff}; // last one is for clear
static unsigned int NO_OF_7SEG;
// Array of GPIO port for each segment
static const uint16_t* seg7Port;
// Array of GPIO port for each Anode
static const uint16_t* seg7EN;
// pointer to the user buffer
static uint8_t* seg7Pointer;
// represent for all segment pin
static uint16_t allSEGPin;

void init_7SEG(const uint16_t* en, const uint16_t* port, uint8_t* buffer, unsigned int no_led7) {
	NO_OF_7SEG = no_led7;

	seg7EN = en;//Array of GPIO port for seg7 Anode
	seg7Port = port;//Array of GPIO port for segments
	seg7Pointer = buffer;// Array of value for each seg7
	allSEGPin = seg7Port[0] | seg7Port[1] | seg7Port[2] | seg7Port[3] | seg7Port[4] | seg7Port[5] | seg7Port[6];
}

void display7SEG(unsigned int index) {
	HAL_GPIO_WritePin(SEG_Port, allSEGPin, GPIO_PIN_SET);

	uint8_t hexcode = seg7Hex[seg7Pointer[index]];
	for(int i = 0; i < 7; i++) {
		if((hexcode & (0x40 >> i)) == 0)
			HAL_GPIO_WritePin(SEG_Port, seg7Port[i], GPIO_PIN_RESET);
	}

	for(int i = 0;  i < NO_OF_7SEG; i++) {
		if(i == index)
			HAL_GPIO_WritePin(SEG_Port, seg7EN[i], GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(SEG_Port, seg7EN[i], GPIO_PIN_SET);
	}
}

