/*
 * input_reading.c
 *
 *  Created on: Nov 12, 2022
 *      Author: MSI
 */

#include "input_reading.h"

unsigned int NO_OF_BTN;
const uint16_t* btnPort;

//the buffer that the final result is stored after debouncing
static GPIO_PinState* btnBuffer;
//two buffers for debouncing
static GPIO_PinState* debounceBtnBuffer1;
static GPIO_PinState* debounceBtnBuffer2;

void init_btn_reading(const uint16_t* port, unsigned int no_of_btn) {
	NO_OF_BTN = no_of_btn;//number of buttons
	btnPort = port; //Array of GPIO port for buttons
	btnBuffer = (GPIO_PinState*)malloc(NO_OF_BTN*sizeof(GPIO_PinState));
	debounceBtnBuffer1 = (GPIO_PinState*)malloc(NO_OF_BTN*sizeof(GPIO_PinState));
	debounceBtnBuffer2 = (GPIO_PinState*)malloc(NO_OF_BTN*sizeof(GPIO_PinState));
	for(int i = 0; i < NO_OF_BTN; i++) {
		btnBuffer[i] = GPIO_PIN_SET;
		debounceBtnBuffer1[i] = GPIO_PIN_SET;
		debounceBtnBuffer2[i] = GPIO_PIN_SET;
	}
}


//Read the current state of button, then compare with the previous state to debounce.The valid state will be store in a buffer
void btn_reading(void) {
	for(uint8_t i = 0; i < NO_OF_BTN; i++) {
		debounceBtnBuffer2[i] = debounceBtnBuffer1[i];
		debounceBtnBuffer1[i] = HAL_GPIO_ReadPin(BTN_Port, btnPort[i]);
		if(debounceBtnBuffer1[i] == debounceBtnBuffer2[i]) {
			btnBuffer[i] = debounceBtnBuffer1[i];
		}
	}
}

//check if button is currently pressed
unsigned char is_btn_pressed(uint8_t index) {
	if(index >= NO_OF_BTN) return 0;
	return (btnBuffer[index] == GPIO_PIN_SET);
}

