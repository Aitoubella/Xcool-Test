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

typedef struct
{
    uint32_t depressed;
    uint32_t previous;
}debounce_t ;



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


typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	debounce_t debounce;
	uint8_t logic_active;
	btn_cb_t cb;
	uint32_t time_glictch;
	uint32_t push_count;
	uint32_t release_count;
	uint8_t state;
	uint8_t lock;
}button_t;

extern button_t btn[BTN_MAX];


void button_init(btn_cb_t cb);
void button_lock(button_t* btn);
void button_unlock(button_t* btn);
#endif
