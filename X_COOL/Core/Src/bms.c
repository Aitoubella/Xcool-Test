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
#include <math.h>
#include "printf.h"
event_id bms_id;
#define BMS_TASK_DUTY_MS               500
//#define USE_TEST                       1 //Dev test
#ifdef USE_TEST
#define BATTERY_CELL                  4
#define BAT_FULL_CAP                  2000 //mAH
#define MAX_CHARGE_CURRENT            2000//mA
#define BAT_CEL_FULL_CHARGE_VOLTAGE   4150
#define BAT_CEL_MIN_VOLTAGE           3000
#define BAT_CEL_PROTECT_VOLTAGE       2500
#define BAT_CEL_RECHARGE_VOLTAGE      4000
#else
//4 cells, 3.7vdc each, 56amphr. Total battery 14.4v 56ahr
#define BATTERY_CELL                  4
#define BAT_FULL_CAP                  57000 //mAH
#define MAX_CHARGE_CURRENT            8000//mA
#define BAT_CEL_FULL_CHARGE_VOLTAGE   4100
#define BAT_CEL_MIN_VOLTAGE           2600
#define BAT_CEL_PROTECT_VOLTAGE       2500
#define BAT_CEL_RECHARGE_VOLTAGE      3900
#endif

#define TIME_CHECK_REMOVE_BAT_SECOND   3
#define CHARGE_CURRENT_RESOLUTION    128 //mA Need to check datasheet. BQ25731:128mA


#define BAT_FULCHARGE_VOLTAGE      (BAT_CEL_FULL_CHARGE_VOLTAGE*BATTERY_CELL)
#define BAT_MIN_VOLTATE            (BAT_CEL_MIN_VOLTAGE*BATTERY_CELL)
#define BAT_PROTECT_VOTAGE         (BAT_CEL_PROTECT_VOLTAGE*BATTERY_CELL)
#define BAT_RECHARGING_VOLTAGE     (BAT_CEL_RECHARGE_VOLTAGE*BATTERY_CELL)
#define CHARGE_PRODUCTIVITY        80//Percent
#define POWER_SUPPLY_VOLTAGE_MIN    (BAT_FULCHARGE_VOLTAGE + 2000) //mV
#define TIME_FULL_CHARGE_SECOND     15

#define DISCHARGE_CURRENT_BOARD_SLEEP   200 //mA
#define FULL_CHARGE_CURRENT_LOST         1

#define SECOND_TO_COUNT(x)       (1000*x/BMS_TASK_DUTY_MS)
//bms_t __attribute__(( section(".noinitdata") )) bms =
//{
//	.is_charging = 0,
//};
bms_t bms =
{
	.is_charging = 0,
};
typedef struct
{
	uint16_t max_charge_voltage;
	uint16_t bat_min_voltage;
	uint16_t bat_max_discharg_voltage;
	uint16_t charge_current;
	uint16_t discharge_current;
	uint16_t bat_voltage;
	uint16_t sys_voltage;
	uint16_t bus_voltage;
	uint16_t power_sys;
	ChargerStatus_t* status;
}charge_info_t;

static bq25731_t bq25731;

static charge_info_t charge =
{
	.status = &bq25731.ChargerStatus,
	.max_charge_voltage = BAT_FULCHARGE_VOLTAGE,
	.bat_min_voltage = BAT_MIN_VOLTATE,
};



typedef enum
{
	BMS_START_STATE = 0,
	BMS_CHARGING_STATE,
	BMS_CHARGING_WAITING_STATE,
	BMS_CHARGE_FULL_STATE,
	BMS_CHECK_BAT_REMOVE_STATE,
	BMS_BAT_REMOVE_WATING_STATE,
	BMS_CHARGE_FULL_WATING_STATE,
	BMS_DISCHARGE_STATE,
	BMS_DISCHARGE_WAITING_STATE,
	BMS_BAT_SHUTDOWN_STATE,
	BMS_BAT_TURN_OFF_CHARGE_STATE,
	BMS_BAT_TURN_OFF_CHARGE_WATING_STATE
}bms_state_t;

bms_state_t bms_state = BMS_START_STATE;




typedef struct
{
	uint32_t a;
	uint32_t b;
}bat_state_voltage_t;

//Value get from this document: https://arxiv.org/ftp/arxiv/papers/1803/1803.10654.pdf

#ifdef USE_TEST
#define MAX_BAT_STATE            8
bat_state_voltage_t bat_state_table[MAX_BAT_STATE] = {{9,26667},{75,254750},{147, 510210}, {411,1468647},
		                                             {181,619337},{120,392892},{101, 316465},{93, 28259}};
uint16_t bat_voltage_state[MAX_BAT_STATE] = {3850, 3530, 3625, 3676, 3759, 3925, 4024, 4300};
#else
#define MAX_BAT_STATE            8
bat_state_voltage_t bat_state_table[MAX_BAT_STATE] = {{9,23125},{16,137975},{139, 436106}, {159,501582},
		                                             {222,715548},{139,430551},{93, 264812},{74, 196294}};
uint16_t bat_voltage_state[MAX_BAT_STATE] = {3100, 3220, 3300, 3370, 3420, 3580, 3700, 3850};
#endif
// Function to calculate SOC based on Coulomb counting
//Use when charging
void bms_charging_calculate(bms_t* bms, uint32_t current_mA, uint32_t time_ms)
{
    // Calculate charge consumed (Coulombs)
	bms->count_discharge = 0;
	bms->cap_mAh += current_mA * time_ms*CHARGE_PRODUCTIVITY/100000;
	if(bms->cap_mAh > BAT_FULL_CAP*3600) bms->cap_mAh = BAT_FULL_CAP*3600;
}

void bms_full_charge_waiting_calculate(bms_t* bms, uint32_t mA_lost, uint32_t time_ms)
{
	if(bms->cap_mAh > (mA_lost * time_ms/1000))
	{
		bms->cap_mAh = bms->cap_mAh - mA_lost * time_ms/1000;
	}else
	{
		bms->cap_mAh = 0;
	}
}

void bms_discharge_waiting_calculate(bms_t* bms,uint32_t current_mA, uint32_t time_ms)
{
	if(current_mA == 0) current_mA = DISCHARGE_CURRENT_BOARD_SLEEP; //Minimum board current
	bms->count_charge = 0;
    // Calculate SOC as a percentage
	if(bms->cap_mAh > (current_mA * time_ms/1000))
	{
		bms->cap_mAh = bms->cap_mAh - current_mA * time_ms/1000;
	}else
	{
		bms->cap_mAh = 0;
	}
}

void bms_charge_full_recalibrate(bms_t* bms)
{
	if(bms->count_charge)
		bms->q_lost_charge = (BAT_FULL_CAP*3600 - bms->cap_mAh)/bms->count_charge;
	bms->count_charge = 0; //Reset count cycle
	bms->count_discharge = 0;
	bms->cap_mAh = BAT_FULL_CAP*3600;
}

void bms_charge_exhaust_recalibrate(bms_t* bms)
{
	if(bms->count_discharge)
		bms->q_lost_discharge = bms->cap_mAh/bms->count_discharge;
	bms->count_charge = 0;
	bms->count_discharge = 0;
	bms->cap_mAh = 0;
}



void bms_charge_update_cap(bms_t* bms, uint32_t vol)
{
	if(bms->cap_mAh != 0) return;
	uint8_t bat_state = MAX_BAT_STATE - 1;
	vol /= BATTERY_CELL;
	if(vol < BAT_CEL_MIN_VOLTAGE) return;
	for(uint8_t i = 0; i < MAX_BAT_STATE; i++)
	{
		if(vol < bat_voltage_state[i])
		{
			bat_state = i;
			break;
		}
	}
	uint32_t percent = (vol * bat_state_table[bat_state].a - bat_state_table[bat_state].b)/1000;
	if(percent > 100) percent = 100;
	bms->cap_mAh = percent * BAT_FULL_CAP * 36;
}



uint8_t bms_bat_percent(void)
{
	return (uint8_t)(round((double)bms.cap_mAh*100/(BAT_FULL_CAP *3600)));
}

void bms_task(void)
{
//	bq25731_get_charge_status(&bq25731);
	bq25731_get_charge_discharge_current(&bq25731, &charge.charge_current,&charge.discharge_current);
	bq25731_get_sys_and_bat_voltage(&bq25731, &charge.bat_voltage, &charge.sys_voltage);
	bq25731_get_vbus_psys(&bq25731,&charge.bus_voltage, &charge.power_sys);
	switch((uint8_t)bms_state)
	{
		case BMS_START_STATE:
			bms_charge_update_cap(&bms,charge.bat_voltage); //Get first cap mAH when first time
			bq25731_set_charge_current(0); //Change voltage and current to 0 for detect bat
			if(charge.bus_voltage > POWER_SUPPLY_VOLTAGE_MIN) //Check bat voltage and sys voltage ok -> charge
			{
				if(charge.bat_voltage > BAT_PROTECT_VOTAGE)
				{
					bms_state = BMS_CHARGING_STATE;
					bms.is_charging = 1;
					bms.is_charge = 1;
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
			bq25731_set_charge_voltage(BAT_FULCHARGE_VOLTAGE); //Set charge voltage
			bq25731_set_charge_current(MAX_CHARGE_CURRENT); //Turn on charge
			bms.full_charge_count = 0;
			bms_state = BMS_CHARGING_WAITING_STATE;
			break;
		case BMS_CHARGING_WAITING_STATE:
			bms_charging_calculate(&bms,charge.charge_current, BMS_TASK_DUTY_MS);
			if(charge.charge_current <= CHARGE_CURRENT_RESOLUTION)
			{
				bms.full_charge_count++;
				if(charge.charge_current == 0)
				{
					bms.full_charge_count += SECOND_TO_COUNT(TIME_FULL_CHARGE_SECOND/3);
				}
			}else
			{
				bms.full_charge_count = 0;
			}
			if(bms.full_charge_count > SECOND_TO_COUNT(TIME_FULL_CHARGE_SECOND))    //Charge in 15s to charge 95% and not stress to protect battery
			{
				bms_state = BMS_CHECK_BAT_REMOVE_STATE;
				bms.remove_check_count = 0;
				bq25731_set_charge_current(0);
				bms.is_charge = 0;
			}
			if(charge.bus_voltage < POWER_SUPPLY_VOLTAGE_MIN) //If power off
			{
				bms.is_charge = 0;
				bms_state = BMS_DISCHARGE_STATE;
			}
			break;
		case BMS_CHECK_BAT_REMOVE_STATE:
			bms.remove_check_count ++;
			if(bms.remove_check_count < SECOND_TO_COUNT(TIME_CHECK_REMOVE_BAT_SECOND)) break;
			if(charge.bat_voltage < 5000)
			{
				bms_charge_exhaust_recalibrate(&bms);
				bms.is_charging = 0;
				bms_state = BMS_BAT_REMOVE_WATING_STATE;
			}else
			{
				bms_state = BMS_CHARGE_FULL_STATE;
			}
			break;
		case BMS_BAT_REMOVE_WATING_STATE:
			if(charge.bat_voltage >= (BAT_PROTECT_VOTAGE - 1000))
			{
				bms_state = BMS_START_STATE;
			}
			break;
		case BMS_CHARGE_FULL_STATE:
			bms_charge_full_recalibrate(&bms);
			bms_state = BMS_CHARGE_FULL_WATING_STATE;
			break;
		case BMS_CHARGE_FULL_WATING_STATE:
			if(charge.bus_voltage < POWER_SUPPLY_VOLTAGE_MIN) //If power off
			{
				bms_state = BMS_DISCHARGE_STATE; //Change power source to bat
			}
			if(charge.bat_voltage < 5000) //Check if bat is remove
			{
				bms_charge_exhaust_recalibrate(&bms);
				bms_state = BMS_BAT_REMOVE_WATING_STATE;
			}else if(charge.bat_voltage < charge.bat_min_voltage) //Check if bat is  destroy
			{
				bms_state = BMS_BAT_SHUTDOWN_STATE;
			}else if (charge.bat_voltage < BAT_RECHARGING_VOLTAGE)
			{
				bms_state = BMS_START_STATE;
			}
			break;
		case BMS_DISCHARGE_STATE: //bat provide power
			bms_state = BMS_DISCHARGE_WAITING_STATE;
			bq25731_set_charge_current(0);
			bms.is_charging = 0;
			bms.is_charge = 0;
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
			bms_charge_update_cap(&bms,charge.bat_voltage);//Get first cap mAH when first time
			bms_discharge_waiting_calculate(&bms, charge.discharge_current, BMS_TASK_DUTY_MS);
			break;
		case BMS_BAT_SHUTDOWN_STATE:
			bms.is_charging = 0;
			bms_charge_exhaust_recalibrate(&bms);
			if(charge.bus_voltage > POWER_SUPPLY_VOLTAGE_MIN) //Check power back on
			{
				if(charge.bat_voltage > BAT_PROTECT_VOTAGE)
				{
					bms_state = BMS_START_STATE;
				}
			}
			break;
		case BMS_BAT_TURN_OFF_CHARGE_STATE:
			bq25731_set_charge_current(0);
			bms.is_charge = 0;
			bms.is_charging = 0;
			bms_state = BMS_BAT_TURN_OFF_CHARGE_WATING_STATE;
			break;
		case BMS_BAT_TURN_OFF_CHARGE_WATING_STATE:

			break;
	}
}

HAL_StatusTypeDef bms_init(void)
{
	//Config battery param

	HAL_StatusTypeDef status = 0;
	status = bq25731_charge_option_0_clear_bit(0, EN_LPWR_BIT); //Disable low power function for ADC convert block work
	if(status != HAL_OK) return status;
	status = bq25731_clear_bit_reg(CHARGE_OPTION_0_REG, 0, EN_OOA_BIT);//Reduce noise for audio
	if(status != HAL_OK) return status;
	status = bq25731_charge_option_1(0, EN_IBAT_BIT);   //Enable Ibat buffer
	if(status != HAL_OK) return status;
	status = bq25731_charge_option_3(0,EN_ICO_MODE_BIT); //Enable Auto mode
	if(status != HAL_OK) return status;
//	status = bq25731_set_charge_voltage(BAT_FULCHARGE_VOLTAGE); //Set charge voltage
	status = bq25731_set_charge_current(0);
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

bms_t* bms_get_charge_info(void)
{
	return &bms;
}


void bms_off_charge(void)
{
	bms_state = BMS_BAT_TURN_OFF_CHARGE_STATE;
}

void bms_on_charge(void)
{
	bms_state = BMS_START_STATE;
}

