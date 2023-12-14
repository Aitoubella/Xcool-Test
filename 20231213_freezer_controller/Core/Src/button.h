#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "gpio.h"
#define BUTTON_NUM                   3
typedef enum
{
	BTN_ENTER = 0,
	BTN_UP,
	BTN_DOWN,
	BTN_MAX
}button_num_t;



typedef enum
{
	BUTTON_SHORT_PRESS = 0,
	BUTTON_PUSH,
	BUTTON_RELEASE,
	BUTTON_HOLD_1_SEC,
	BUTTON_HOLD_2_SEC,
	BUTTON_HOLD_3_SEC,
	BUTTON_HOLD_10_SEC,
	BUTTON_TAP_TAP,
}btn_evt_t;


typedef void (*btn_cb_t)(uint8_t btn_num, btn_evt_t evt);

void button_init(btn_cb_t cb);

#endif
