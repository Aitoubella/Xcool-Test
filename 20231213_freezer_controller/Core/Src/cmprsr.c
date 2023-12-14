/*
 * cmprsr.c
 *
 *  Created on: Nov 8, 2023
 *      Author: Loc
 */


#include "cmprsr.h"
#include "main.h"
#include "event.h"

event_id cmprsr_led_id;

enum
{
	LED_LOGIC_LOW = 0,
	LED_LOGIC_HIGH
};

typedef enum
{
	LED_INPUT_HIGH_STATE = 0,
	LED_INPUT_LOW_STATE,
}led_input_state_t;

typedef enum
{
	BATTERY_FAULT_VOLTAGE = 1, /*Battery protection cut-out (The voltage is outside the cut-out setting)*/
	FAN_OVER_CURRENT, /*(The fan loads the compressor driver with more than 1A peak*/
	MORTOR_START_ERROR, /*(The rotor is blocked or the differential pressure in the refrigeration system is too high (> 6 bar))*/
	MINIMUM_MOTOR_SPEED_ERROR, /*r (if the refrigeration system is too heavily loaded, the motor cannot maintain minimum
								speed 1850 rpm or the controller cannot find the rotor position)*/
	CMPRSR_DRIVER_TEMP_TOO_HIGH, /*if the refrigerator is too heavily loaded, or the ambient temperature is high,
                            the compressor driver will run too hot (case temperature > 75°C))*/
	CONTROLLER_HARDWARE_FAIL, /*Controller detects abnormal parameters*/

}cmprsr_fault_t;
void cmprsr_get_led_get_fault_task(void)
{

}


void cmprsr_init(void)
{
	event_add(cmprsr_get_led_get_fault_task, &cmprsr_led_id, 10);
	event_active(&cmprsr_led_id);
}
