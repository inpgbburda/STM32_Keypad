/*
 * keypad.h
 *
 *  Created on: 3 mar 2021
 *      Author: Bartłomiej Burda
 */

#ifndef INC_KEYPAD_H_

#include "stm32f3xx_hal.h"
#include "main.h"


typedef enum {true, false} bool;
typedef enum { Reset, Start, Go, Wait, Trigger, Lock}State;


void SetKeypadRowLow(uint8_t row);
char DebounceSingleKey(bool is_pressed, char key_to_debounce);
char ReadKeypad();

#define INC_KEYPAD_H_



#endif /* INC_KEYPAD_H_ */
