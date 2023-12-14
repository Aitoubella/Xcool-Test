/*
 * led_signal.h
 *
 *  Created on: Dec 7, 2023
 *      Author: Loc
 */

#ifndef SRC_LED_SIGNAL_H_
#define SRC_LED_SIGNAL_H_
#include <stdint.h>
void led_signal_init(void);
void led_signal_togle(uint16_t on_ms, uint16_t off_ms, uint16_t count);
void led_signal_stop(void);

#endif /* SRC_LED_SIGNAL_H_ */
