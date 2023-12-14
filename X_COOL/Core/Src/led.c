#include "led.h"
#include "main.h"
#include "event.h"
#define LED_TASK_INTERRUPT      1
#if(LED_TASK_INTERRUPT != 1)
static event_id led_id;
#endif
#define LED_LOGIC_ON        GPIO_PIN_SET
#define LED_LOGIC_OFF       GPIO_PIN_RESET


#define LED_TICK             10

#define MS_TO_COUNT(x)        (x/LED_TICK)

#define LED_ON(gpio,pin)    HAL_GPIO_WritePin(gpio, pin, LED_LOGIC_ON);
#define LED_OFF(gpio,pin)    HAL_GPIO_WritePin(gpio, pin, LED_LOGIC_OFF);
enum
{
	LED_STOP_STATE = 0,
	LEDS_STOP_WATING_STATE,
	LED_ON_STATE,
	LED_ON_WATING_STATE,
	LED_OFF_STATE,
	LED_OFF_WAITING_STATE,
};

led_t* led_list[5] = {0};//Contain
static uint8_t led_count = 0;
static uint8_t led_initialize = 0;

void led_on(led_t *x)
{
	led_start_togle(x, 10000, 0, 100);
}
void led_off(led_t *x)
{
	led_start_togle(x, 0, 10000, 100);
}



void led_task(void)
{
	for(uint8_t i = 0; i < led_count; i ++)
	{
	   switch(led_list[i]->state)
	   {
		case LED_ON_STATE:
			if(led_list[i]->on_total_count == 0)//Not duty for on
			{
				led_list[i]->state = LED_OFF_STATE; //Move to off state
			}else
			{
				LED_ON(led_list[i]->gpio,led_list[i]->pin);
				led_list[i]->state = LED_ON_WATING_STATE;
			}
		break;

		case LED_ON_WATING_STATE:
			led_list[i]->on_count ++;
			if(led_list[i]->on_count > led_list[i]->on_total_count) //Reach max count on;
			{
				led_list[i]->on_count = 0; //move to next state
				led_list[i]->state = LED_OFF_STATE;
			}
		break;

		case LED_OFF_STATE:
			if(led_list[i]->off_total_count == 0)
			{
				led_list[i]->state = LED_ON_STATE;
			}else
			{
				LED_OFF(led_list[i]->gpio,led_list[i]->pin);
				led_list[i]->state = LED_OFF_WAITING_STATE;
			}
		break;

		case LED_OFF_WAITING_STATE:
			led_list[i]->off_count ++;
			if(led_list[i]->off_count > led_list[i]->off_total_count)
			{
				led_list[i]->off_count = 0;
				led_list[i]->state = LED_ON_STATE;
				if(led_list[i]->togle_total_count > 0)
				{
					led_list[i]->togle_count ++;
					if(led_list[i]->togle_count >= led_list[i]->togle_total_count)
					{
						led_list[i]->togle_count = 0;
						led_list[i]->state = LED_STOP_STATE;
					}
				}

			}
			break;

		case LED_STOP_STATE:
			LED_OFF(led_list[i]->gpio,led_list[i]->pin);
			led_list[i]->state = LEDS_STOP_WATING_STATE;
			break;

		case LEDS_STOP_WATING_STATE:
			break;
		}
	}
}

void led_init(void)
{
	if(led_initialize) return; //Check if led library is init before
	led_initialize = 1;
#if(LED_TASK_INTERRUPT != 1)
	event_add(led_task, &led_id, LED_TICK);
	event_active(&led_id);
#endif
}

#if(LED_TASK_INTERRUPT == 1)
/*
 * @bref This function run on interrupt, in case event task is delay use for sound to generate buzzer timing exactly
 *       This function must be place in 1ms interrupt handler
 * @param ms :task timing running.
 * */
void led_task_interrupt(uint8_t ms)
{
	static uint8_t ms_tick = 0;
	ms_tick ++;
	if(ms_tick >= ms)
	{
		ms_tick = 0;
		led_task();
	}
}
#endif
void led_add(led_t* l)
{
	if(led_count >= MAX_LED)
	{
		return;
	}
	if(l) led_list[led_count++] = l; //Pointer to list
}

/**
 * @brief Start togle with init param
 * @return none
 * */
void led_start_togle(led_t* l, uint16_t on_ms, uint16_t off_ms, uint16_t count)
{
	l->on_total_count = MS_TO_COUNT(on_ms);
	l->off_total_count = MS_TO_COUNT(off_ms);
	l->togle_total_count = count;
	l->on_count = 0;
	l->off_count = 0;
	l->togle_count = 0;
	l->state = LED_ON_STATE;
}

void led_stop_togle(led_t *l)
{
	l->state_backup = l->state;
	l->state = LED_STOP_STATE;
}


void led_continue_togle(led_t *l)
{
	l->state = l->state_backup;
}






