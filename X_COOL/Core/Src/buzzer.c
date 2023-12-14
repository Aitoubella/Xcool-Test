#include "buzzer.h"
#include "led.h"
#include "main.h"
#define BUZZER_ON()          HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
#define BUZZER_OFF()         HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
#define BUZZER_TOGLE()       HAL_GPIO_TogglePin(Buzzer_GPIO_Port, Buzzer_Pin)

#define BUZZER            {Buzzer_GPIO_Port, Buzzer_Pin}

led_t buzzer = BUZZER;

void buzzer_init(void)
{
	led_add(&buzzer);
}

void buzzer_togle(uint16_t on_ms, uint16_t off_ms, uint16_t count)
{
	led_start_togle(&buzzer, on_ms, off_ms, count);
}

void buzzer_stop(void)
{
	led_off(&buzzer);
}
