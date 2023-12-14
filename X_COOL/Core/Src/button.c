#include "button.h"
#include "event.h"
#include <stdint.h>
#include "main.h"
event_id button_id;

#define BUTTON_DEBOUNCE_DEF                {0,0}

#define BUTTON_1                          {BTN_ENTER_GPIO_Port, BTN_ENTER_Pin, BUTTON_DEBOUNCE_DEF}
#define BUTTON_2                          {BTN_UP_GPIO_Port, BTN_UP_Pin, BUTTON_DEBOUNCE_DEF}
#define BUTTON_3                          {BTN_DOWN_GPIO_Port, BTN_DOWN_Pin, BUTTON_DEBOUNCE_DEF}
#define BUTTON_LIST                       {BUTTON_1, BUTTON_2,BUTTON_3}


#define TICK_1000_MS                     (1000)
#define TICK_2000_MS                     (2000)
#define TICK_3000_MS                     (3000)
#define TICK_10000_MS                    (10000)
#define TICK_PUSH_MS                     (50)


static void btn_evt_cb_default(uint8_t btn_num, btn_evt_t evt);
static btn_cb_t btn_evt_cb = btn_evt_cb_default;

typedef struct
{
    uint32_t depressed;
    uint32_t previous;
}debounce_t ;


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
}button_t;

enum
{
	BUTTON_START_STATE = 0,
	BUTTON_PUSH_STATE,   //1
	BUTTON_RELEASE_STATE, //2
	BUTTON_HOLDING_STATE,
	BUTTON_HOLD_1S_STATE, //4
	BUTTON_HOLDING_1S_STATE,
	BUTTON_HOLD_2S_STATE, //5
	BUTTON_HOLDING_2S_STATE,
	BUTTON_HOLD_3S_STATE, //7
	BUTTON_HOLDING_3S_STATE,
	BUTTON_HOLD_10S_STATE, //7
	BUTTON_HOLDING_10S_STATE,
};

static button_t btn[BTN_MAX] = BUTTON_LIST;


/**
 * @brief Debounce button to avoid noise
 * @param debounce: pointer to btn debounce struct
 * @param current: Button state input
 *
 * */
uint32_t button_debounce(debounce_t* debounce, uint32_t current)
{
	 /* state of the button debouncing, see below */

	debounce->depressed |= (debounce->previous & current); /* set depressed */
	debounce->depressed &= (debounce->previous | current); /* clear released */
	debounce->previous   = current; /* update the history */
  return debounce->depressed;
}


GPIO_PinState button_read_pin(button_num_t num)
{
	return HAL_GPIO_ReadPin(btn[num].gpio, btn[num].pin);
}

void btn_evt_cb_default(uint8_t btn_num, btn_evt_t evt)
{

}


void button_task(void)
{
	for(uint8_t btn_num = 0; btn_num < BTN_MAX; btn_num++)
	{
		uint32_t logic = button_debounce(&btn[btn_num].debounce, button_read_pin(btn_num));
		if(btn[btn_num].logic_active == logic)
		{
			btn[btn_num].push_count ++;

		}else //Release
		{
			btn[btn_num].state = BUTTON_RELEASE_STATE;
		}

		switch(btn[btn_num].state)
		{
		case BUTTON_START_STATE:
			if(btn[btn_num].push_count > TICK_PUSH_MS)//push at least 10ms
			{
				btn[btn_num].state = BUTTON_PUSH_STATE;
			}
			break;
		case BUTTON_PUSH_STATE:
			btn_evt_cb(btn_num, BUTTON_PUSH);
			btn[btn_num].state =  BUTTON_HOLDING_STATE;
			break;
		case BUTTON_HOLDING_STATE:
			if(btn[btn_num].push_count > TICK_1000_MS)
			{
				btn[btn_num].state = BUTTON_HOLD_1S_STATE;
			}
			break;

		case BUTTON_RELEASE_STATE:
			if(btn[btn_num].push_count > 10 && btn[btn_num].push_count < 500)
			{
				btn_evt_cb(btn_num, BUTTON_SHORT_PRESS);
			}else
			{
				if(btn[btn_num].push_count > TICK_PUSH_MS) btn_evt_cb(btn_num, BUTTON_RELEASE);
			}
			btn[btn_num].push_count = 0; //Reset push_count
			btn[btn_num].state = BUTTON_START_STATE;
			break;
		case BUTTON_HOLD_1S_STATE:
			btn_evt_cb(btn_num, BUTTON_HOLD_1_SEC);
			btn[btn_num].state = BUTTON_HOLDING_1S_STATE;
			break;
		case BUTTON_HOLDING_1S_STATE:
			if(btn[btn_num].push_count > TICK_2000_MS)
			{
				btn[btn_num].state = BUTTON_HOLD_2S_STATE;
			}
			break;
		case BUTTON_HOLD_2S_STATE:
			btn_evt_cb(btn_num, BUTTON_HOLD_2_SEC);
			btn[btn_num].state = BUTTON_HOLDING_2S_STATE;
			break;
		case BUTTON_HOLDING_2S_STATE:
			if(btn[btn_num].push_count > TICK_3000_MS)
			{
				btn[btn_num].state = BUTTON_HOLD_3S_STATE;
			}
			break;
		case BUTTON_HOLD_3S_STATE:
			btn_evt_cb(btn_num, BUTTON_HOLD_3_SEC);
			btn[btn_num].state = BUTTON_HOLDING_3S_STATE;
			break;
		case BUTTON_HOLDING_3S_STATE:
			if(btn[btn_num].push_count > TICK_10000_MS)
			{
				btn[btn_num].state = BUTTON_HOLD_10S_STATE;
			}
			break;
		case BUTTON_HOLD_10S_STATE:
			btn_evt_cb(btn_num, BUTTON_HOLD_10_SEC);
			btn[btn_num].state = BUTTON_HOLDING_10S_STATE;
			break;
		case BUTTON_HOLDING_10S_STATE: //Trap ultil button release
			break;
		default:
			break;
		}
	}

}


void button_init(btn_cb_t cb)
{
	if(cb) btn_evt_cb = cb;
	event_add(button_task, &button_id, 1); //event task init run loop 1 ms
	event_active(&button_id);
}


