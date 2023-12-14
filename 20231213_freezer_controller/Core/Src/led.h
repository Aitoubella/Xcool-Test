
#ifndef SRC_LED_H_
#define SRC_LED_H_
#include <stdint.h>
#include "main.h"
#define MAX_LED          10

typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t pin;
	uint16_t togle_total_count; //Total togle count
	uint16_t on_total_count;
	uint16_t off_total_count;
	uint16_t togle_count;     //number of togle times
	uint16_t on_count;  //duty on
	uint16_t off_count; //duty off
	uint8_t state;
	uint8_t state_backup;
}led_t;

void led_init(void);
void led_on(led_t *x);
void led_off(led_t *x);
void led_add(led_t* l);
void led_start_togle(led_t* l, uint16_t on_ms, uint16_t off_ms, uint16_t count);
void led_stop_togle(led_t *l);
void led_task_interrupt(uint8_t ms);
#endif 
