/*
 * bms.h
 *
 *  Created on: Nov 17, 2023
 *      Author: Loc
 */

#ifndef SRC_BMS_H_
#define SRC_BMS_H_
#include "BQ25731.h"
typedef struct
{
	uint8_t is_charging;
	uint8_t is_charge;
	uint16_t max_charge_voltage;
	uint16_t bat_min_voltage;
	uint16_t bat_max_discharg_voltage;
	uint16_t charge_current;
	uint16_t discharge_current;
	uint16_t bat_voltage;
	uint16_t sys_voltage;
	uint16_t bus_voltage;
	uint16_t power_sys;
	uint8_t bat_percent;
	uint32_t count_charge;
	uint32_t count_discharge;
	uint8_t full_charge_count;
	uint32_t q_lost_charge;
	uint32_t q_lost_discharge;
	uint32_t current_cap_mA;
	uint32_t discharge_delay;
	ChargerStatus_t* status;
}charge_info_t;

HAL_StatusTypeDef bms_init(void);
charge_info_t* bms_get_charge_info(void);
uint32_t bms_voltage_to_percent(uint32_t vol);
#endif /* SRC_BMS_H_ */
