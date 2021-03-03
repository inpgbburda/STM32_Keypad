/*
 * keypad.c
 *
 *      Author: Bartłomiej Burda
 */
#include "keypad.h"

/*define dimensions of keypad*/
#define HIGHT 4
#define WIDTH 4

/*define time required to debounce keypad in miliseconds*/
#define KP_DEBOUNCE_MS 20

/*define GPIOs array used to connect rows of keyboard*/
static GPIO_TypeDef* Row_Ports[HIGHT] = {ROW_1_GPIO_Port, ROW_2_GPIO_Port, ROW_3_GPIO_Port, ROW_4_GPIO_Port};
static uint16_t Row_Pins[HIGHT] = {ROW_1_Pin, ROW_2_Pin, ROW_3_Pin, ROW_4_Pin};

/*define GPIOs array used to connect columns of keyboard*/
static GPIO_TypeDef* Col_Ports[WIDTH] = {COL_1_GPIO_Port, COL_2_GPIO_Port, COL_3_GPIO_Port, COL_4_GPIO_Port};
static uint16_t Col_Pins[WIDTH] = {COL_1_Pin,COL_2_Pin, COL_3_Pin, COL_4_Pin};

/*define constant matrix representing every key on the keypad*/
const char KeyMatrix[HIGHT][WIDTH] = {
		{'1', '2', '3', 'A' },
		{'4', '5', '6', 'B' },
		{'7', '8', '9', 'C'	},
		{'*', '0', '#', 'D' }
};

void SetKeypadRowLow(uint8_t row) {
	for(int idx=0; idx<HIGHT; idx++){
		if(idx == row){
			HAL_GPIO_WritePin(Row_Ports[idx], Row_Pins[idx], 0);
		}else{
			HAL_GPIO_WritePin(Row_Ports[idx], Row_Pins[idx], 1);
		}
	}
}

char DebounceSingleKey(bool is_pressed, char key_to_debounce){

	static State state_machine = Reset;
	static uint32_t start_time = 0;
	uint32_t current_time = HAL_GetTick();
	char key = '\0';

	switch (state_machine) {
	case Reset:
		state_machine = Start;
		break;
	case Start:
		if (is_pressed == true) {
			state_machine = Go;
		}
		break;
	case Go:
		start_time = HAL_GetTick();
		state_machine = Wait;
		break;
	case Wait:
		current_time = HAL_GetTick();
		if (is_pressed == false) {
			state_machine = Reset;
		}
		if (current_time - start_time > KP_DEBOUNCE_MS) {
			state_machine = Lock;
		}
		break;
	case Lock:
		if (is_pressed == false)
			state_machine = Trigger;
		break;
	case Trigger:
		key = key_to_debounce;
		state_machine = Reset;
		break;
	}
	return key;
}

/**
  * @brief Return pressed char from keyboard with debouncing in non-blocking mode.
  * To trigger key press button for predeclared time and release.
  * @note   When any key is pressed return that key, in other case return '\0'
  * @retval Char pressed key
  */
char ReadKeypad() {

	bool is_any_key_pressed = false;

	static char pressed_key = '\0';
	int r_idx = 0;
	int c_idx = 0;
	int row = 0;
	int col = 0;

	for (row = 0; row < WIDTH; row++) {
		SetKeypadRowLow(row);
		for (col = 0; col < WIDTH; col++) {
			if(HAL_GPIO_ReadPin(Col_Ports[col], Col_Pins[col]) == 0){
				r_idx = row;
				c_idx = col;
				is_any_key_pressed = true;
			}
		}
	}
	if (is_any_key_pressed == true) {
		pressed_key = KeyMatrix[r_idx][c_idx];
	}
	return DebounceSingleKey(is_any_key_pressed, pressed_key);
}

