/*
 * event.c
 *
 *  Created on: Mar 25, 2021
 *      Author: lochoang
 */

#include "event.h"
#include <string.h>

event_t event[MAX_EVENT_NUMBER] = {0};
static uint8_t total_event = 0;


/**
 * @func   event_init
 * @brief  Init loop runtask for event
 * @param  None
 * @retval None
 */
void event_init(void)
{
   // Set event control default value
   memset((uint8_t*) event, 0, sizeof(event));
}

/******************************************************************************/
/**
 * @func   event_init
 * @brief  Init loop runtask for event
 * @param  None
 * @retval None
 */
void event_deinit(void)
{
    
}

/**
 * @func   event_add
 * @brief  add event to run
 * @param  callback: function to call
 *         delay_ms: Duty run callback function
 *         round: Number of time that run callback function
 * @retval None
 */

uint8_t event_add(void (* func)(void),event_id *id, uint32_t ms)
{ 
    uint8_t index;
	event_pause();
	for(index = 0; index < MAX_EVENT_NUMBER; index++)
	{
		if(event[index].func == func)
		{
			event_resume();
			return EVENT_ERR;
		}
	}
	for(index = 0; index < MAX_EVENT_NUMBER; index++)
	{
		if(event[index].func == (void*)0)
		{
			event[index].status = EVENT_STATUS_INACTIVE;  //Set inactive for this event
			event[index].func = func;                     //Set callback function
			event[index].delay = ms;                      //Set delay for callback to call
			*id = index;                                  //Set id
			total_event = index;
			event_resume();
			return EVENT_ERR_NONE;
		}
	}
	event_resume();
	return EVENT_ERR;
}
/**
 * @func   event_remove
 * @brief  Remove funtion from list event
 * @param  func: function to remove
 * @retval None
 */
void event_remove(event_id *id)
{
	event_pause();
	event[*id].func = (void*)0;
	event[*id].status = EVENT_STATUS_INACTIVE;  //Set inactive for this event
	event_resume();
}

/**
 * @func   event_set_delay
 * @brief  Set ms for func to call
 * @param  func: function to set from task list
 *         ms:  time in ms
 * @retval None
 */
void event_set_delay(event_id *id, uint32_t ms)
{
	event_pause();
	event[*id].delay = ms;
	event_resume();
}
/**
 * @func   event_set_inactive
 * @brief  remove event func
 * @param  func: function to remove from task list
 * @retval None
 */
void event_active(event_id *id)
{
	event_pause();
	event[*id].status = EVENT_STATUS_ACTIVE;
	event[*id].old_time = GetSystemTick();      //Set current time stamp
	event_resume();
}
/**
 * @func   event_set_inactive
 * @brief  remove event func
 * @param  func: function to remove from task list
 * @retval None
 */
void event_inactive(event_id* id)
{
	event_pause();
	event[*id].status = EVENT_STATUS_INACTIVE;
	event_resume();
}
/**
 * @func   event_run_task
 * @brief  Run task in while loop
 * @param  none
 * @retval None
 */
void event_run_task(void)
{
	static uint8_t event_index = 0;
    event_index++;
    if(event_index > total_event)
	{
		event_index = 0;
	}
    
    if(event[event_index].status == EVENT_STATUS_ACTIVE)
    {
    	event[event_index].elapsed_time = GetTimeElapse(event[event_index].old_time); //Get elapsed time
        if(event[event_index].elapsed_time >= event[event_index].delay)               //Check if time is expired
        {
        	event[event_index].old_time = GetSystemTick();                        //Set time to current  stamp
        	event[event_index].func();                                            //Call callback function
        }
    }
}

/**
 * @func   event_run_task
 * @brief  Run task in while loop
 * @param  none
 * @retval None
 */
void event_run_task_rtos(void)
{
	static uint8_t event_index = 0;
    for(event_index = 0; event_index <= total_event; event_index ++)
    {
        if(event[event_index].status == EVENT_STATUS_ACTIVE)
        {
        	event[event_index].elapsed_time = GetTimeElapse(event[event_index].old_time); //Get elapsed time
            if(event[event_index].elapsed_time >= event[event_index].delay)               //Check if time is expired
            {
            	event[event_index].old_time = GetSystemTick();                        //Set time to current  stamp
            	event[event_index].func();                                            //Call callback function
            }
        }
    }
}


