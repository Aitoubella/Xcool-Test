/*
 * app_event.h
 *
 *  Created on: Mar 25, 2021
 *      Author: lochoang
 */

#ifndef _APP_EVENT_
#define _APP_EVENT_
#include "stm32l4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#define EVENT_ERR                  1
#define EVENT_ERR_NONE             0

#define MAX_EVENT_NUMBER           10
#define EVENT_STATUS_INACTIVE      0
#define EVENT_STATUS_ACTIVE        1

typedef uint8_t event_id;
typedef struct
{
    void (* func)(void);
    uint32_t elapsed_time;
    uint32_t old_time;
    uint32_t delay;
    uint8_t status;
}event_t;
#define GetSystemTick()                       xTaskGetTickCount()
#define GetTimeElapse(a)                      (((GetSystemTick() - a) & 0xFFFFFFFF))
#define event_pause()                         {}
#define event_resume()                        {}

void event_init(void);
void event_deinit(void);
uint8_t event_add(void (* func)(void),event_id *id, uint32_t ms);
void event_set_delay(event_id *id, uint32_t ms);
void event_inactive(event_id *id);
void event_remove(event_id *id);
void event_active(event_id *id);
void event_run_task(void);
void event_run_task_rtos(void);
#endif /* HARD_EVENT_H_ */
