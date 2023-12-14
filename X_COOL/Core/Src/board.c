/*
 * board.c
 *
 *  Created on: Oct 23, 2023
 *      Author: Loc
 */


#include "board.h"
#include "event.h"
#include "DS1307.h"
#include "lcd_ui.h"
#include "button.h"
#include "buzzer.h"


/*
 * Get input from CHRG_OK
 * Open drain active high indicator to inform the system good power source is connected to
the charger input. Connect to the pullup rail via 10-kΩ resistor. When VBUS rises above 3.5
V and falls below 25.8 V, CHRG_OK is HIGH after 50-ms deglitch time. When VBUS falls
below 3.2 V or rises above 26.8 V, CHRG_OK is LOW. When one of SYSOVP, SYSUVP,
ACOC, TSHUT, BATOVP, BATOC or force converter off faults occurs, CHRG_OK is asserted
LOW.
*/

charge_status_t get_charge_status(void)
{
	if(HAL_GPIO_ReadPin(CHRG_OK_GPIO_Port,CHRG_OK_Pin) == GPIO_PIN_SET)
	{
		return CHARGE_STATUS_NORMAL;
	}
	return CHARGE_STATUS_FALT;
}


/*
 * LED (Fault ID Light) .
A 10mA light emitting diode (LED) (6) can be connected between the terminals
+ and D. In case the compressor driver detects an operational error, the diode
will flash a number of times. The number of flashes depends on what kind of
operational error was recorded. Each flash will last 1/5 second. After the actual
number of flashes, there will be a delay with no flashes, so that the sequence
for each error recording is repeated every 1 minute.
*/

cmprsr_status_t get_cmprsr_status(void)
{
	if(HAL_GPIO_ReadPin(CMPRSR_D_GPIO_Port,CMPRSR_D_Pin) == GPIO_PIN_SET)
	{
		return CMPRSR_STATUS_NORMAL;
	}
		return CMPRSR_STATUS_FAILT;
}

/*
 * @brief On 5V for controller board
 * */
void pwr_ctrl_on(void)
{
	HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_RESET);
}

void pwr_ctrl_off(void)
{
	HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_SET);
}
