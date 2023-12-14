/*
 * led_signal.c
 *
 *  Created on: Dec 7, 2023
 *      Author: Loc
 */


#include "led_signal.h"
#include "led.h"

#define LED_SIGNAL            {LED_GPIO_Port, LED_Pin}

led_t led_signal = LED_SIGNAL;

void led_signal_init(void)
{
	led_add(&led_signal);
}

void led_signal_togle(uint16_t on_ms, uint16_t off_ms, uint16_t count)
{
	led_start_togle(&led_signal, on_ms, off_ms, count);
}

void led_signal_stop(void)
{
	led_off(&led_signal);
}
