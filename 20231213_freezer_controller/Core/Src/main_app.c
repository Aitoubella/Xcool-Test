/*
 * main_app.c
 *
 *  Created on: Nov 3, 2023
 *      Author: Loc
 */


#include "lcd_interface.h"
#include "lcd_ui.h"
#include "DS1307.h"
#include "RTD.h"
#include <stdbool.h>
#include "flash.h"
#include "event.h"
#include "buzzer.h"
//#include "logging.h"
#include "board.h"
#include "power_board.h"
#include "bms.h"
#include "PID.h"
#include "File_Handling.h"
#include "ili9341.h"

#define MAIN_TASK_TICK_MS    100 //ms
#define RTC_TASK_TICK_MS          1000
#define BAT_OUT_OF_VALUE     7 //in perent
#define MINUTE_TO_COUNT(x) (x*60*1000/MAIN_TASK_TICK_MS) //convert minute to tick count in main task

#define buzzer_lid_warning()          buzzer_togle(50, 2000, 0);
#define buzzer_over_temp_warning()    buzzer_togle(100, 1000, 0);
#define buzzer_under_temp_warning()   buzzer_togle(100, 1000, 0);

#define KP                1
#define KD                0
#define KI                0

double limit_max = 0;
double limit_min = 0;

#define CHAMBER_TEMPERATURES_SENSOR           RTD6
#define LID_SWITCH_SENSOR                     RTD4

#define LID_CLOSE_DELAY_MINS                   1

typedef enum
{
	MAIN_NORMAL_STATE  = 0,
	MAIN_WARNING_WAITING_STATE,
}main_state_t;

enum
{
	TURN_OFF = 0,
	TURN_ON
};

typedef enum
{
	BUZZER_OFF_STATE = 0,
	BUZZER_OFF_WAITING_STATE,
	BUZZER_ON_STATE,
	BUZZER_ON_WATING_STATE,
}buzzer_state_t;
typedef struct
{
	uint32_t under_min_temp;
	uint32_t over_max_temp;
	uint32_t lid_open;
	uint32_t alarm_mute;
}alarm_count_t;

typedef enum
{
	NONE_SAVE_STATE = 0,
	CHANGE_DATA_STATE,
	NEED_SAVE_STATE,
}save_state_t;

typedef struct
{
	uint8_t fan1; //Internal fan
	uint8_t fan2;
	uint8_t cmprsr;
	uint8_t cmprsr_fan;
}output_ctrl_t;
output_ctrl_t ctl = {0};
alarm_count_t alarm_count;

uint32_t lid_delay_count = 0;
uint32_t logging_count = 0;
main_state_t main_state = MAIN_NORMAL_STATE;
buzzer_state_t buzzer_state  = BUZZER_OFF_STATE;
save_state_t save_state = NONE_SAVE_STATE;
double pid_output;
static event_id main_app_id;
static event_id rtc_time_id;
extern char USERPath[4];
extern char USBHPath[4];
char file_name_src[20];
char file_name_dst[20];
lcd_inter_t setting = {
	.op_mode = OPERATION_MODE_FREEZER,
	.pwr_mode = POWER_MODE_AC,
	.spk_mode = SPEAKER_MODE_ON,
	.bat_value = 100, //%
	.bat_state = BATTERY_STATE_NOT_CHARGE,
	.bat_signal = BATTER_WARNING_LOW,
	.alarm_bat = 15, //%
	.alarm_lid = 2,  //mins
	.alarm_temperature_delay = 1, //mins
	.alarm_temperature_deviation = 5, // Celcius
	.temperature = 31,//Celcius
	.logging_interval = 1,//Mins
	.temperature_fridge = 31,//Celcius
	.temperature_freezer = 31,//Celcius
	.temp_offset = 0, //Celcius
	.datetime.year = 2023,
	.datetime.month = 11,
	.datetime.day = 3,
	.alarm_mute_duration = 0,
};
extern lcd_inter_t lcd;
extern lcd_state_t lcd_state;




uint8_t lcd_get_set_cb(lcd_get_set_evt_t evt, void* value)
{

	switch((uint8_t)evt)
	{
	   case LCD_SET_OPERATION_MODE_EVT:
		   if(setting.op_mode != *((operation_mode_t *)value))
		   {
			   save_state = CHANGE_DATA_STATE;
			   setting.op_mode = *((operation_mode_t *)value);
		   }
		   break;
		case LCD_SET_DATETIME_EVT:
			datetime_t* dt = (datetime_t *)value;
			//Check save data to DS1307 ok
			if((DS1307_SetDate(dt->day, dt->month, dt->year) == HAL_OK) &&
				(DS1307_SetTime(dt->hour, dt->minute, dt->second) == HAL_OK))
			{
				memcpy((uint8_t *)&setting.datetime,(uint8_t *)dt,sizeof(datetime_t));
			}

			break;

		case LCD_SET_TEMPERATURE_FRIDGE_EVT:
			if(setting.temperature_fridge != *((int8_t *)value))
			{
				save_state = CHANGE_DATA_STATE;
				setting.temperature_fridge = *((int8_t *)value);
			}
			break;

		case LCD_SET_TEMPERATURE_FREEZER_EVT:
			if(setting.temperature_freezer != *((int8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.temperature_freezer = *((int8_t *)value);
			}
			break;

		case LCD_SET_ALARM_TEMPERATURE_DEVIATION_EVT:
			if(setting.alarm_temperature_deviation != *((int8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.alarm_temperature_deviation = *((int8_t *)value);
			}

			break;

		case LCD_SET_ALARM_TEMPERATURE_DELAY_EVT:
			if(setting.alarm_temperature_delay != *((uint8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.alarm_temperature_delay = *((uint8_t *)value);
			}
			break;

		case LCD_SET_ALARM_BAT_EVT:
			if(setting.alarm_bat != *((uint8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.alarm_bat = *((uint8_t *)value);
			}

			break;

		case LCD_SET_ALARM_LID_EVT:
			if(setting.alarm_lid != *((uint8_t *)value))
			{
				save_state = CHANGE_DATA_STATE;
				setting.alarm_lid = *((uint8_t *)value);
			}

			break;

		case LCD_SET_LOGGING_INTERVAL_EVT:
			if(setting.logging_interval != *((uint8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.logging_interval = *((uint8_t *)value);
			}
			break;

		case LCD_SET_TEMP_OFFSET_EVT:
			if(setting.temp_offset != *((uint8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.temp_offset = *((uint8_t *)value);
			}
			break;
		case LCD_SET_LARM_MUTE_DURATION_EVT:
			if(main_state != MAIN_NORMAL_STATE) //Set alarm mute only effect when in warning
			{
				if(setting.alarm_mute_duration != *((uint8_t *)value))
				{
					setting.alarm_mute_duration = *((uint8_t *)value);
				}
			}
			break;

		case LCD_USB_INSERT_DOWNLOAD_EVT:
			printf("\nStart download ");
			sprintf(file_name_src,"%s%s",USERPath,LOG_FILE_NAME);
			sprintf(file_name_dst,"%s%s",USBHPath,LOG_FILE_NAME);
			if(copy_file(file_name_dst,file_name_src) == FR_OK)
			{
				printf("success!");
				return 1;
			}
			else
			{
				printf("failed!");
				return 0;
			}
			break;
		case LCD_MAIN_FRAME_EVT:
			if(save_state == CHANGE_DATA_STATE)
			{
				save_state = NEED_SAVE_STATE;
			}
			break;
		case LCD_POWER_OFF_EVT:
//			LED_L(); //Turn off Back Light led
			pwr_ctrl_off();
			break;
		case LCD_PWER_ON_EVT:
//			LED_H(); //Turn on  Back Light led
			pwr_ctrl_on();
			break;
	}
	if(save_state == NEED_SAVE_STATE)// setting param change?
	{
		__disable_irq(); //Disable all global interrupt to save data safety
		flash_mgt_save((uint32_t *)&setting, sizeof(lcd_inter_t)); //Save setting to flash if any change
		__enable_irq(); //Enable all global interrupt
		save_state = NONE_SAVE_STATE;
	}
	return 1;
}


void rtc_task(void)
{
	DS1307_GetDate(&setting.datetime.day, &setting.datetime.month, &setting.datetime.year);
	DS1307_GetTime(&setting.datetime.hour, &setting.datetime.minute, &setting.datetime.second);
}

uint8_t get_bat_value(void)
{

//	if(chrg->bat_voltage > chrg->bat_min_voltage)
//	{
//		bat_value =  chrg->bat_voltage - chrg->bat_min_voltage;
//		bat_value = bat_value * 100 /(chrg->max_charge_voltage - chrg->bat_min_voltage);
//	}
//	if(bat_value > 100) bat_value = 100;

 	return (uint8_t)(bms_get_charge_info()->bat_percent);
}

battery_state_t get_bat_state(void)
{
	if(bms_get_charge_info()->is_charging)
	{
		return BATTERY_STATE_CHARGING;
	}
	return BATTERY_STATE_NOT_CHARGE;
}

power_mode_t get_power_mode(void)
{
	if(HAL_GPIO_ReadPin(DCDC_VS_3V3_GPIO_Port, DCDC_VS_3V3_Pin) == GPIO_PIN_SET)
	{
		return POWER_MODE_DC;
	}else if(HAL_GPIO_ReadPin(ACDC_VS_3V3_GPIO_Port, ACDC_VS_3V3_Pin) == GPIO_PIN_SET)
	{
		return POWER_MODE_AC;
	}else
	{
		return POWER_MODE_BAT;
	}
}



lid_state_t get_lid_state(void)
{
	if(rtd_get_adc_voltage(LID_SWITCH_SENSOR) < 2000)
	{
		return LID_CLOSE;
	}
	return LID_OPEN;
}





void main_task(void)
{
	static lcd_inter_t* lcd_param;


	//Get temperature with temperature offset from setting
	setting.temperature = (int16_t) (rtd_get_temperature(CHAMBER_TEMPERATURES_SENSOR)) + setting.temp_offset;
	//Get bat status
	setting.bat_value = get_bat_value();
	setting.bat_state = get_bat_state();
	//Get power status
	setting.pwr_mode = get_power_mode();
	//Get Lid state
	setting.lid_state = get_lid_state();


	//Check exeed temperature setting
	if(setting.op_mode == OPERATION_MODE_FRIDEGE)
	{
		htr_off(); //Heater on in freezer mode,off in refrigerator off
		PID_set_point(setting.temperature_fridge); //PID set new point
		limit_min = - KP*setting.temperature_fridge;//Calculate limit when reach min warning delta temperature(current - setpoint)
		limit_max = KP*setting.temperature_fridge;//Calculate limit when reach max warning delta temperature (current - setpoint)
//		PID_limit(limit_min,limit_max);
		if(setting.temperature <= (setting.temperature_fridge - setting.alarm_temperature_deviation))    //Check under min temperature
		{
			//Increase count for delay check later
			alarm_count.under_min_temp += 1;

		}else if(setting.temperature >= (setting.temperature_fridge + setting.alarm_temperature_deviation))//Check over max temperature
		{
			//Increase count for delay check later
			alarm_count.over_max_temp += 1;
		}else
		{
			alarm_count.over_max_temp = 0;
			alarm_count.under_min_temp = 0;
		}
	}else if(setting.op_mode == OPERATION_MODE_FREEZER)
	{
		htr_on(); //Heater on in freezer mode,off in refrigerator off
		PID_set_point(setting.temperature_freezer); //PID set new point
		limit_min = - KP*setting.temperature_freezer;//Calculate limit when reach min warning delta temperature(current - setpoint)
		limit_max = KP*setting.temperature_fridge;//Calculate limit when reach max warning delta temperature (current - setpoint)
//		PID_limit(limit_min,limit_max);
		if(setting.temperature <= (setting.temperature_freezer - setting.alarm_temperature_deviation))   //Check under min temperature
		{
			//Increase count for delay check later
			alarm_count.under_min_temp += 1;
		}else if (setting.temperature >= (setting.temperature_freezer + setting.alarm_temperature_deviation))   //Check over max temperature
		{
			//Increase count for delay check later
			alarm_count.over_max_temp += 1;
		}else
		{
			alarm_count.over_max_temp = 0;
			alarm_count.under_min_temp = 0;
		}
	}


	//PID compute process
	if(is_rtd_started())
	{
		PID_compute(rtd_get_temperature(CHAMBER_TEMPERATURES_SENSOR) + setting.temp_offset);
		pid_output = PID_getOutput();
	}

	if(pid_output < -2.2) //Reach max temperature->down temp
	{
		ctl.cmprsr = TURN_ON;
		ctl.cmprsr_fan = TURN_ON;
		ctl.fan1 = TURN_ON;
		ctl.fan2 = TURN_ON;
	}

	if(pid_output > 0)
	{
		ctl.cmprsr = TURN_OFF;
		ctl.cmprsr_fan = TURN_OFF;
		ctl.fan1 = TURN_OFF;
		ctl.fan2 = TURN_OFF;
	}

	//Lid check open
	if(setting.lid_state == LID_OPEN)
	{
		//Increase count for delay check later
		alarm_count.lid_open += 1;
		lid_delay_count = 0;
		//Lid switch output – when lid is opened, compressor, condenser fan and chamber fan turned off.
		ctl.cmprsr = TURN_OFF;
		ctl.cmprsr_fan = TURN_OFF;
		ctl.fan1 = TURN_OFF;
		ctl.fan2 = TURN_OFF;
	}else //Lid close
	{
		alarm_count.lid_open = 0; //All turns back on when lid is closed although we require a compressor on delay of 1-2mins(settable in service mode)
		lid_delay_count ++;
	}
   //All turns back on when lid is closed although we require a compressor on delay of 1-2 mins(settable in service mode)
	if(lid_delay_count < MINUTE_TO_COUNT(LID_CLOSE_DELAY_MINS))
	{
		ctl.cmprsr = TURN_OFF;
		ctl.cmprsr_fan = TURN_OFF;
		ctl.fan1 = TURN_OFF;
		ctl.fan2 = TURN_OFF;
	}

	//Check in DC mode
	if(setting.pwr_mode == POWER_MODE_AC)
	{
		//Chamber fan always on in AC operation
		ctl.fan1 = TURN_ON;
		ctl.fan2 = TURN_ON;
	}

	if(ctl.cmprsr == TURN_ON) cmprsr_power_on();
	else cmprsr_power_off();
	if(ctl.cmprsr_fan == TURN_ON) cmprsr_fan_on();
	else cmprsr_fan_off();
	if(ctl.fan1 == TURN_ON) fan1_on();
	else fan1_off();
	if(ctl.fan2 == TURN_ON) fan2_on();
	else fan2_off();


	//Check bat value in percent
	if(setting.bat_value <= BAT_OUT_OF_VALUE)
	{
		setting.bat_signal = BATTERY_OUT_OF_BAT;
	}else if(setting.bat_value <= setting.alarm_bat)
	{
		setting.bat_signal = BATTER_WARNING_LOW;

	}else
	{
		setting.bat_signal = BATTERY_NORMAL;
	}

	//Check alarm happen
	if(alarm_count.lid_open >= MINUTE_TO_COUNT(setting.alarm_lid)) //Warning lid higher priority
	{
		setting.warning_type = WARNING_TYPE_LID_OPEN;
	}else if(alarm_count.over_max_temp >= MINUTE_TO_COUNT(setting.alarm_temperature_delay))
	{
		setting.warning_type = WARNING_TYPE_OVER_MAX_TEMP;
	}else if(alarm_count.under_min_temp >= MINUTE_TO_COUNT(setting.alarm_temperature_delay))
	{
		setting.warning_type = WARNING_TYPE_UNDER_MIN_TEMP;
	}else
	{
		setting.warning_type = WARNING_TYPE_NONE;
	}

  /*
   * Note lcd_state need to consider carefully for set lcd_state
   * */
	switch((uint8_t)lcd_state)
	{
		case LCD_MAIN_STATE:
			//Check any warning
			if(setting.warning_type == WARNING_TYPE_LID_OPEN) //Warning lid higher priority
			{
				lcd_state = LCD_WARNING_TYPE_LID_OPEN_STATE; //Change lcd state
				lcd_interface_show(lcd_state);  //show warning on lcd
			}else if(setting.warning_type == WARNING_TYPE_OVER_MAX_TEMP)
			{
				lcd_state = LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE; //Change lcd state
				lcd_interface_show(lcd_state);         //show warning on lcd
			}else if(setting.warning_type == WARNING_TYPE_UNDER_MIN_TEMP)
			{
				lcd_state = LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE;//Change lcd state
				lcd_interface_show(lcd_state); //show warning on lcd
			}else //Has no warning
			{
				//Check current value is differ from lcd value -> update lcd
				lcd_param =  lcd_interface_get_param();
				if(lcd_param->op_mode != setting.op_mode || lcd_param->pwr_mode != setting.pwr_mode
				  || lcd_param->temperature != setting.temperature || lcd_param->bat_state != setting.bat_state
				  || lcd_param->bat_value != setting.bat_value || lcd_param->spk_mode != setting.spk_mode
				  || lcd_param->bat_signal != setting.bat_signal)
				{
					//Update lcd main frame
					lcd_param->op_mode = setting.op_mode;
					lcd_param->pwr_mode = setting.pwr_mode;
					lcd_param->temperature = setting.temperature;
					lcd_param->bat_state = setting.bat_state;
					lcd_param->bat_value = setting.bat_value;
					lcd_param->spk_mode = setting.spk_mode;
					lcd_param->bat_signal = setting.bat_signal;
					//Reload main frame
					lcd_interface_show(lcd_state);
				}
			}
		break;
		case LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE:
		case LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE:
		case LCD_WARNING_TYPE_LID_OPEN_STATE:
			if(setting.warning_type == WARNING_TYPE_NONE)  //Waiting for end warning
			{
				lcd_state = LCD_MAIN_STATE;
				lcd_interface_show(lcd_state);
			}
			break;
		default:
			break;
	}

	switch((uint8_t)main_state)
	{
		case MAIN_NORMAL_STATE:
			if(setting.warning_type == WARNING_TYPE_LID_OPEN) //Warning lid higher priority
			{
				buzzer_lid_warning();
				//logging_write(LOG_FILE_NAME, &setting);//Log imediataly when warning
				main_state = MAIN_WARNING_WAITING_STATE;//Move to waiting state.
			}else if(setting.warning_type == WARNING_TYPE_OVER_MAX_TEMP)
			{
				 buzzer_over_temp_warning();
				//logging_write(LOG_FILE_NAME, &setting);//Log immediately when warning
				main_state = MAIN_WARNING_WAITING_STATE;//Move to waiting state.
			}else if(setting.warning_type == WARNING_TYPE_UNDER_MIN_TEMP)
			{
				buzzer_under_temp_warning();
				//logging_write(LOG_FILE_NAME, &setting);//Log immediately when warning
				main_state = MAIN_WARNING_WAITING_STATE;//Move to waiting state.
			}
			break;
		case MAIN_WARNING_WAITING_STATE:

			if(setting.warning_type == WARNING_TYPE_NONE) //Waiting for end warning
			{
				main_state = MAIN_NORMAL_STATE; //Return to normal state
				if(buzzer_state != BUZZER_OFF_WAITING_STATE)
				{
					buzzer_state = BUZZER_OFF_STATE;
				}
			}else //Still warning?
			{
				if(setting.alarm_mute_duration > 0) //Delay alarm sound?
				{
					setting.spk_mode = SPEAKER_MODE_OFF; //LCD speaker off
					if(buzzer_state != BUZZER_OFF_WAITING_STATE)
					{
						buzzer_state = BUZZER_OFF_STATE;
					}
					alarm_count.alarm_mute ++;
					if(alarm_count.alarm_mute >= MINUTE_TO_COUNT(setting.alarm_mute_duration))
					{
						setting.spk_mode = SPEAKER_MODE_ON; //Lcd speaker on again
						setting.alarm_mute_duration = 0;//Reset alarm mute to 0
						lcd.alarm_mute_duration = 0; //Reset lcd alarm mute to 0
						alarm_count.alarm_mute = 0;
					}
				}else //Delay alarm sound timeout or not set
				{
					if(buzzer_state != BUZZER_ON_WATING_STATE)
					{
						buzzer_state = BUZZER_ON_STATE;
					}
				}
			}
			break;
	}

	switch((uint8_t)buzzer_state)
	{
		case BUZZER_OFF_STATE:
			buzzer_stop();
			buzzer_state = BUZZER_OFF_WAITING_STATE;
			break;
		case BUZZER_OFF_WAITING_STATE:
			break;
		case BUZZER_ON_STATE:
			if(setting.warning_type == WARNING_TYPE_LID_OPEN) //Warning lid higher priority
			{
				buzzer_lid_warning();
			}else if(setting.warning_type == WARNING_TYPE_OVER_MAX_TEMP)
			{
				buzzer_over_temp_warning();
			}else if(setting.warning_type == WARNING_TYPE_UNDER_MIN_TEMP)
			{
				buzzer_under_temp_warning();
			}
			buzzer_state = BUZZER_ON_WATING_STATE;
			break;
		case BUZZER_ON_WATING_STATE:

			break;
	}


	//Logging timeline
	logging_count++;
	if(logging_count >= MINUTE_TO_COUNT(setting.logging_interval))
	{
		logging_count = 0;
		sprintf(file_name_src,"%s%s",USERPath,LOG_FILE_NAME);
		logging_write(file_name_src, &setting);//Log when reach logging interval in setting.
	}
}

void main_app_init(void)
{
	//Init extend board
	power_board_init();
	//On 12V for fan1,fan2,htr
	pwr_12v_on(); //On power 12V
	//Read setting from storage
	flash_mgt_read((uint32_t *)&setting, sizeof(lcd_inter_t));
	//Load all current param to lcd param
	memcpy((uint8_t *)&lcd,(uint8_t *)&setting, sizeof(lcd_inter_t));
	setting.alarm_mute_duration = 0; //Reset mute alarm duration, because it must be zero. alarm duration only effect when set from LCD
    //PID set param
    PID_init();
	PID_tune(KP,KI,KD);
	PID_set_divisor(1);
	//LCD ui
	lcd_ui_init(); //Init lvgl, porting
	lcd_interface_init(); //Init api and show main frame
	event_add(main_task, &main_app_id, MAIN_TASK_TICK_MS);
	event_active(&main_app_id);
	rtc_task(); //Update time when start;
	event_add(rtc_task, &rtc_time_id, RTC_TASK_TICK_MS);
	event_active(&rtc_time_id);
}
