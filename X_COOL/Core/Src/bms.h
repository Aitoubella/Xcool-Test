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
	uint8_t bat_percent;
	uint8_t is_charging;
	uint8_t is_charge;
	uint8_t full_charge_count;
	uint8_t remove_check_count;
	int32_t q_lost_charge;
	int32_t q_lost_discharge;
	uint32_t count_charge;
	uint32_t count_discharge;
	uint32_t discharge_delay;
	uint32_t delay_start;
	uint32_t cap_mAh;
}bms_t;


HAL_StatusTypeDef bms_init(void);
bms_t* bms_get_charge_info(void);
uint8_t bms_bat_percent(void);
void bms_off_charge(void);
void bms_on_charge(void);
#endif /* SRC_BMS_H_ */
