/*
 * bms.c
 *
 *  Created on: Nov 17, 2023
 *      Author: Loc
 */
#include "BQ25731.h"
#include "main.h"
#include "event.h"
#include "bms.h"
#include "lcd_ui.h"
event_id bms_id;
#define BMS_TASK_DUTY_MS               500
#define USE_TEST                       1
#ifdef USE_TEST
#define BATTERY_CELL                  4
#define POWER_SUPPLY_VOLTAGE_MIN      22000 //mV
#define BAT_FULL_CAP                  2000 //mAH
#define MAX_CHARGE_CURRENT            2000//mA
#else
//4 cells, 3.7vdc each, 56amphr. Total battery 14.4v 56ahr
#define BATTERY_CELL                  4
#define POWER_SUPPLY_VOLTAGE_MIN      22000 //mV
#define BAT_FULL_CAP                  56000 //mAH
#define MAX_CHARGE_CURRENT            8000//mA
#endif


#define CHARGE_CURRENT_RESOLUTION    128 //mA Need to check datasheet. BQ25731:128mA
#define BAT_CEL_MIN_VOLTAGE          3000

#define BAT_FULCHARGE_VOLTAGE      (4150*BATTERY_CELL)
#define BAT_MIN_VOLTATE            (BAT_CEL_MIN_VOLTAGE*BATTERY_CELL)
#define BAT_PROTECT_VOTAGE         (2500*BATTERY_CELL)
#define BAT_RECHARGING_VOLTAGE     (4100*BATTERY_CELL)
#define CHARGE_PRODUCTIVITY        80//Percent

#define TIME_COUNT_FULL_CHARGE     (15 * 1000/ BMS_TASK_DUTY_MS)

#define DISCHARGE_CURRENT_BOARD_SLEEP   50 //mA

static bq25731_t bq25731;

static charge_info_t charge =
{
	.status = &bq25731.ChargerStatus,
	.max_charge_voltage = BAT_FULCHARGE_VOLTAGE,
	.bat_min_voltage = BAT_MIN_VOLTATE,
	.is_charging = 0,
};



typedef enum
{
	BMS_START_STATE = 0,
	BMS_CHARGING_STATE,
	BMS_CHARGING_WAITING_STATE,
	BMS_CHARGE_FULL_STATE,
	BMS_CHARGE_FULL_WATING_STATE,
	BMS_DISCHARGE_STATE,
	BMS_DISCHARGE_WAITING_STATE,
	BMS_BAT_SHUTDOWN_STATE,
}bms_state_t;

bms_state_t bms_state = BMS_START_STATE;


typedef struct
{
	uint32_t a;
	uint32_t b;
}bat_state_voltage_t;
//Value get from this document: https://arxiv.org/ftp/arxiv/papers/1803/1803.10654.pdf
#define MAX_BAT_STATE            8
bat_state_voltage_t bat_state_table[MAX_BAT_STATE] = {{9,26667},{75,254750},{147, 510210}, {411,1468647},
		                                             {181,619337},{120,392892},{101, 316465},{93, 28259}};
uint16_t bat_voltage_state[MAX_BAT_STATE] = {3450, 3530, 3625, 3676, 3759, 3925, 4024, 4300};
// Function to calculate SOC based on Coulomb counting
uint32_t SOC_charge_calculate(uint32_t current_mA, uint32_t time_ms)
{
    // Calculate charge consumed (Coulombs)
	charge.count_discharge = 0;
	charge.current_cap_mA = charge.current_cap_mA + current_mA * time_ms*CHARGE_PRODUCTIVITY/100000 - charge.q_lost_charge;
	if(charge.current_cap_mA > BAT_FULL_CAP*3600) charge.current_cap_mA = BAT_FULL_CAP*3600;
    return charge.current_cap_mA * 100/(BAT_FULL_CAP*3600);
}

uint32_t SOC_discharge_calculate(uint32_t current_mA, uint32_t time_ms)
{
	if(current_mA == 0) current_mA = DISCHARGE_CURRENT_BOARD_SLEEP;
	charge.count_charge = 0;
    // Calculate SOC as a percentage
	if(charge.current_cap_mA > (current_mA * time_ms/1000 + charge.q_lost_discharge))
	{
		charge.current_cap_mA = charge.current_cap_mA - current_mA * time_ms/1000 + charge.q_lost_discharge;
	}else
	{
		charge.current_cap_mA = 0;
	}
    return charge.current_cap_mA * 100/(BAT_FULL_CAP*3600);
}

void SOC_charge_full_recalibrate(void)
{
//	charge.q_lost_charge = (BAT_FULL_CAP*3600 - charge.current_cap_mA)/charge.count_charge;
	charge.count_charge = 0;
	charge.count_discharge = 0;
	charge.current_cap_mA = BAT_FULL_CAP*3600;
}

void SOC_charge_exhaust_recalibrate(void)
{
	charge.q_lost_discharge = charge.current_cap_mA/charge.count_discharge;
	charge.count_charge = 0;
	charge.count_discharge = 0;
	charge.current_cap_mA = 0;
}



void bms_charge_update_cap(void)
{
	charge.current_cap_mA = charge.bat_percent * BAT_FULL_CAP *3600/ 100;
}



uint32_t bms_voltage_to_percent(uint32_t vol)
{
	uint8_t bat_state = 0;
	vol /= BATTERY_CELL;
	if(vol < BAT_CEL_MIN_VOLTAGE) return 0;
	for(uint8_t i = 0; i < MAX_BAT_STATE; i++)
	{
		if(vol < bat_voltage_state[i])
		{
			bat_state = i;
			break;
		}
	}
	uint32_t percent = (vol * bat_state_table[bat_state].a - bat_state_table[bat_state].b)/1000;
	if(percent >100) percent = 100;
	return percent;
}

void bms_task(void)
{
	bq25731_get_charge_status(&bq25731);
	bq25731_get_charge_discharge_current(&bq25731, &charge.charge_current,&charge.discharge_current);
	bq25731_get_sys_and_bat_voltage(&bq25731, &charge.bat_voltage, &charge.sys_voltage);
	bq25731_get_vbus_psys(&bq25731,&charge.bus_voltage, &charge.power_sys);

	switch((uint8_t)bms_state)
	{
		case BMS_START_STATE:
			charge.bat_percent = bms_voltage_to_percent(charge.bat_voltage);
			bms_charge_update_cap();
			bq25731_set_charge_current(0); //Change voltage and current to 0 for detect bat
			if(charge.bus_voltage > POWER_SUPPLY_VOLTAGE_MIN) //Check bat voltage and sys voltage ok -> charge
			{
				if(charge.bat_voltage > BAT_PROTECT_VOTAGE)
				{
					bms_state = BMS_CHARGING_STATE;
					charge.is_charging = 1;
					charge.is_charge = 1;
				}else
				{
					bms_state = BMS_BAT_SHUTDOWN_STATE;
				}
			}else
			{
				bms_state = BMS_DISCHARGE_STATE;
			}
			break;
		case BMS_CHARGING_STATE:
			bq25731_set_charge_current(MAX_CHARGE_CURRENT); //Turn on charge
			charge.full_charge_count = 0;
			bms_state = BMS_CHARGING_WAITING_STATE;
			break;
		case BMS_CHARGING_WAITING_STATE:
			charge.count_charge ++;
			charge.bat_percent = SOC_charge_calculate(charge.charge_current, BMS_TASK_DUTY_MS);
			if(charge.charge_current <= CHARGE_CURRENT_RESOLUTION)
			{
				charge.full_charge_count++;
				if(charge.full_charge_count > TIME_COUNT_FULL_CHARGE)    //Charge in 15s to charge 95% and not stress to protect battery
				{
					bms_state = BMS_CHARGE_FULL_STATE;
				}
			}
			if(charge.bus_voltage < POWER_SUPPLY_VOLTAGE_MIN) //If power off
			{
				bms_state = BMS_DISCHARGE_STATE;
			}
			break;
		case BMS_CHARGE_FULL_STATE:
			SOC_charge_full_recalibrate();
			bq25731_set_charge_current(0);
			charge.is_charge = 0;
			bms_state = BMS_CHARGE_FULL_WATING_STATE;
			break;
		case BMS_CHARGE_FULL_WATING_STATE:
			if(charge.bat_voltage <= BAT_RECHARGING_VOLTAGE) //If bat voltage is go down minimum -> need to charge again
			{
				bms_state = BMS_START_STATE;
			}
			if(charge.bus_voltage < POWER_SUPPLY_VOLTAGE_MIN) //If power off
			{
				bms_state = BMS_DISCHARGE_STATE;
			}
			break;
		case BMS_DISCHARGE_STATE:
			bms_state = BMS_DISCHARGE_WAITING_STATE;
			bq25731_set_charge_current(0);
			charge.is_charging = 0;
			charge.is_charge = 0;
			break;
		case BMS_DISCHARGE_WAITING_STATE:

			if(charge.bus_voltage > POWER_SUPPLY_VOLTAGE_MIN) //Check power back on
			{
				bms_state = BMS_START_STATE;
			}
			if(charge.bat_voltage < charge.bat_min_voltage) //Shut down power to protect bat
			{
				bms_state = BMS_BAT_SHUTDOWN_STATE;
			}
			break;
		case BMS_BAT_SHUTDOWN_STATE:
			if(charge.bus_voltage > POWER_SUPPLY_VOLTAGE_MIN) //Check power back on
			{
				if(charge.bat_voltage > BAT_PROTECT_VOTAGE)
				{
					bms_state = BMS_START_STATE;
				}
			}
			break;
	}
	if(charge.is_charge == 0)
	{
		charge.bat_percent = bms_voltage_to_percent(charge.bat_voltage);
		bms_charge_update_cap();
	}


}

HAL_StatusTypeDef bms_init(void)
{
	//Config battery param
	HAL_StatusTypeDef status = 0;
	status = bq25731_charge_option_0_clear_bit(0, EN_LPWR_BIT); //Disable low power function for ADC convert block work
	if(status != HAL_OK) return status;
	status = bq25731_charge_option_1(0, EN_IBAT_BIT);   //Enable Ibat buffer
	if(status != HAL_OK) return status;
	status = bq25731_charge_option_3(0,EN_ICO_MODE_BIT); //Enable Auto mode
	if(status != HAL_OK) return status;
	status = bq25731_set_charge_voltage(BAT_FULCHARGE_VOLTAGE); //Set charge voltage
	if(status != HAL_OK) return status;
	status = bq25731_set_bit_reg(INPUT_CURRENT_LIMIT_USE_REG, 0,INPUT_CURRENT_3200_MA_BIT); //Set max current input of power source
	if(status != HAL_OK) return status;
	status = bq25731_set_bit_reg(INPUT_VOLTAGE_REG, 0,INPUT_VOLTAGE_8192_MV_BIT); //Set min drop input voltage to 8192 mV
	if(status != HAL_OK) return status;
	status = bq25731_clear_bit_reg(CHARGE_OPTION_2_REG, EN_EXTILIM_BIT, 0); //Disable external limit current circuit pin
	if(status != HAL_OK) return status;
	status = bq25731_set_bit_reg(CHARGE_OPTION_2_REG,EN_ICHG_IDCHG_BIT,0); //Enable IBAT is charge current
	if(status != HAL_OK) return status;
	status = bq25731_clear_bit_reg(CHARGE_OPTION_3_REG, OTG_VAP_MODE_BIT, 0); //Disable control OTG/VAP use by external pin
	if(status != HAL_OK) return status;
	status = bq25731_clear_bit_reg(CHARGE_OPTION_0_REG, 0, WDTMR_ADJ_ENABLE_175S_BIT);//Clear 2 bit  to disable watchdog
	if(status != HAL_OK) return status;
	//Enable ADC VSYS,VBUS, VBAT ,I charge,I discharge and start ADC with continuous mode
	status = bq25731_set_adc_option(EN_ADC_IIN_BIT|EN_ADC_VBUS_BIT|EN_ADC_VSYS_BIT|EN_ADC_VBAT_BIT|EN_ADC_IDCHG_BIT|EN_ADC_ICHG_BIT, ADC_START_BIT|ADC_CONV_BIT);
	if(status != HAL_OK) return status;

	status = bq25731_read_charge_option_0(&bq25731);
	if(status != HAL_OK) return status;
	status = bq25731_read_charge_option_1(&bq25731);
	if(status != HAL_OK) return status;
	status = bq25731_read_charge_option_2(&bq25731);
	if(status != HAL_OK) return status;
	status = bq25731_read_charge_option_3(&bq25731);
	if(status != HAL_OK) return status;
	status = bq25731_read_reg(ADC_OPTION_REG, (uint8_t *)&bq25731.ADCOption, 2); //Read back ADC option
	if(status != HAL_OK) return status;
//	status = bq25731_set_charge_current(500);
//	if(status != HAL_OK) return status;


//	Add task run 1 second duty
	event_add(bms_task, &bms_id, BMS_TASK_DUTY_MS);
	event_active(&bms_id);

	return status;
}

charge_info_t* bms_get_charge_info(void)
{
	return &charge;
}


