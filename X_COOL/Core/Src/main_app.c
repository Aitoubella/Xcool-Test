/*
 * main_app.c
 *
 *  Created on: Nov 3, 2023
 *      
 */


#include "lcd_interface.h"
#include "lcd_ui.h"
#include "DS1307.h"
#include "RTD.h"
#include <stdbool.h>
#include "flash.h"
#include "event.h"
#include "buzzer.h"
#include "logging.h"
#include "board.h"
#include "power_board.h"
#include "bms.h"
#include "PID.h"
#include "File_Handling.h"
#include "ili9341.h"
#include "button.h"
#include "tem_roll.h"

#define MAIN_TASK_TICK_MS         500 //ms
#define RTC_TASK_TICK_MS          1000
#define BAT_OUT_OF_VALUE     7 //in perent
#define MINUTE_TO_COUNT(x) (x*60*1000/MAIN_TASK_TICK_MS) //convert minute to tick count in main task
#define SECOND_TO_COUNT(x) (x*1000/MAIN_TASK_TICK_MS)
#define buzzer_lid_warning()          buzzer_togle(50, 1500, 0);
#define buzzer_over_temp_warning()    buzzer_togle(100, 1000, 0);
#define buzzer_under_temp_warning()   buzzer_togle(100, 1000, 0);
#define buzzer_lock()                 buzzer_togle(100, 100, 1)
#define buzzer_unlock()               buzzer_togle(100, 100, 2)

#define KP                1
#define KD                0
#define KI                0

double limit_max = 0;
double limit_min = 0;

#define CHAMBER_TEMPERATURES_SENSOR           RTD1
#define LID_SWITCH_SENSOR                     RTD4
#define AMBIENT_TEMPERATURE_SENSOR            RTD5

#define MAX_TEMPERATURE_DIFF_FILTER           7
#define MIN_CHAMBER_TEMPERATURE_LIMIT         -30
#define MAX_CHAMBER_TEMPERATURE_LIMIT         50

#define LID_CLOSE_DELAY_MINS                   1

#define TEMPERATURE_SHOW_INTERVAL              1 //Second
#define CMPRSR_DELAY_ON_MINS                   1 //Minute



#define POWER_12V_RESET_INTERVAL              60 //Second
const char *fw_version = "v42";

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
output_ctrl_t ctl_pre = {0};
alarm_count_t alarm_count;


typedef struct
{
	uint32_t lid;
	uint32_t logging;
	uint32_t temper;
	uint32_t cmprsr;
	uint32_t pwr_12v;
}interval_count_t;

interval_count_t interval_count =
{
	.lid = MINUTE_TO_COUNT(LID_CLOSE_DELAY_MINS),
};


main_state_t main_state = MAIN_NORMAL_STATE;
buzzer_state_t buzzer_state  = BUZZER_OFF_STATE;
save_state_t save_state = NONE_SAVE_STATE;
double pid_output;
static event_id main_app_id;
static event_id led_id;
static event_id rtc_time_id;
extern char USERPath[4];
extern char USBHPath[4];
char file_name_src[20];
char file_name_dst[20];
lcd_inter_t setting  = {
	.op_mode = OPERATION_MODE_FREEZER,
	.pwr_mode = POWER_MODE_AC,
	.spk_mode = SPEAKER_MODE_ON,
	.bat_value = 100, //%
	.bat_state = BATTERY_STATE_NOT_CHARGE,
	.bat_signal = BATTER_WARNING_LOW,
	.alarm_bat = 20, //%
	.alarm_lid = 2,  //mins
	.alarm_temperature_delay = 5, //mins
	.alarm_temperature_deviation = 5, // Celcius
	.temperature = 31,//Celcius
	.logging_interval = 5,//Mins
	.setpoint_fridge = 4,//Celcius
	.setpoint_freezer = -20,//Celcius
	.temp_offset = 0, //Celcius
	.datetime.year = 2023,
	.datetime.month = 11,
	.datetime.day = 3,
	.alarm_mute_duration = 20,//Minute
	.deviation_freezer = 2,
	.deviation_fridge = 2,
	.onoff = DISPLAY_UINIT_YES,
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

		case LCD_SET_TEMPERATURE_FRIDGE_SETPOINT_EVT:
			if(setting.setpoint_fridge != *((int8_t *)value))
			{
				save_state = CHANGE_DATA_STATE;
				setting.setpoint_fridge = *((int8_t *)value);
			}
			break;

		case LCD_SET_TEMPERATURE_FREEZER_SETPOINT_EVT:
			if(setting.setpoint_freezer != *((int8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.setpoint_freezer = *((int8_t *)value);
			}
			break;

		case LCD_SET_TEMPERATURE_FRIDGE_DEVIATION_EVT:
			if(setting.deviation_fridge != *((int8_t *)value))
			{
				save_state = CHANGE_DATA_STATE;
				setting.deviation_fridge = *((int8_t *)value);
			}
			break;

		case LCD_SET_TEMPERATURE_FREEZER_DEVIATION_EVT:
			if(setting.deviation_freezer != *((int8_t *)value))
			{
				 save_state = CHANGE_DATA_STATE;
				setting.deviation_freezer = *((int8_t *)value);
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
			if(setting.alarm_mute_duration != *((uint8_t *)value))
			{
				save_state = CHANGE_DATA_STATE;
				setting.alarm_mute_duration = *((uint8_t *)value);
				alarm_count.alarm_mute =  MINUTE_TO_COUNT(setting.alarm_mute_duration); //Reload alarm_count
			}
			break;

		case LCD_USB_INSERT_DOWNLOAD_EVT:
			printf("\nStart download ");
			sprintf(file_name_src,"%s%s",USERPath,LOG_FILE_NAME);
			sprintf(file_name_dst,"%s%s",USBHPath,LOG_FILE_NAME);
            if(copy_file(file_name_dst,file_name_src) == FR_OK)
            {
                printf("success!");
				return (uint8_t)SETTING_DOWNLOAD_DATA_SUCCESS;
            }
            else
            {
                printf("failed!");
				return (uint8_t)SETTING_DOWNLOAD_DATA_FAILED;
            }
			break;
		case LCD_MAIN_FRAME_EVT:
			if(save_state == CHANGE_DATA_STATE)
			{
				save_state = NEED_SAVE_STATE;
			}
			break;
		case LCD_POWER_OFF_EVT:
			LED_L(); //Turn off Back Light led
			bms_off_charge();
			setting.onoff = DISPLAY_UINIT_NO;
			break;
		case LCD_POWER_ON_EVT:
			setting.onoff = DISPLAY_UINIT_YES;
//			bms_on_charge();
//			LED_H(); //Turn on  Back Light led
			HAL_NVIC_SystemReset();
			break;
		case LCD_POWER_SHORT_PRESS_EVT:
			if(main_state == MAIN_WARNING_WAITING_STATE)
			{
				alarm_count.alarm_mute =  MINUTE_TO_COUNT(setting.alarm_mute_duration); //Reload alarm_count
			}
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
	if(setting.datetime.year < 2023 || setting.datetime.year > 2100)
	{
		DS1307_SetYear(2023);
	}
	DS1307_GetTime(&setting.datetime.hour, &setting.datetime.minute, &setting.datetime.second);
}

uint8_t get_bat_value(void)
{
 	return bms_bat_percent();
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
#define   K               0.5
//k: (0.0 – 1.0)
double expRunningAverage(double curVal,double newVal)
{
  curVal += (newVal - curVal) * K;
  return curVal;
}



void main_task(void)
{
	//Check device is off? -> Not excute next code
	if(setting.onoff == DISPLAY_UINIT_NO) //Off
	{
		buzzer_stop();
		cmprsr_power_off();
		cmprsr_fan_off();
		fan1_off();
		fan2_off();
		htr_off();
		alarm_count.alarm_mute = 0;
		alarm_count.lid_open = 0;
		alarm_count.over_max_temp = 0;
		alarm_count.under_min_temp = 0;
		interval_count.lid = MINUTE_TO_COUNT(LID_CLOSE_DELAY_MINS);
		interval_count.temper = SECOND_TO_COUNT(TEMPERATURE_SHOW_INTERVAL);
		return;
	}

	//temperature delay interval implement
	interval_count.temper ++;
	//Get temperature with temperature offset from setting
	if(interval_count.temper > SECOND_TO_COUNT(TEMPERATURE_SHOW_INTERVAL))
	{
		interval_count.temper = 0;
		if(is_rtd_started())//Wait util rtc started done
		{
			//Get current temperature
			double cur_chamber_temp = rtd_get_temperature(CHAMBER_TEMPERATURES_SENSOR) + setting.temp_offset;
			//Check temperature is value?
			if(cur_chamber_temp >= MIN_CHAMBER_TEMPERATURE_LIMIT && cur_chamber_temp <= MAX_CHAMBER_TEMPERATURE_LIMIT)
			{
				if(tem_roll_enough_data(CHAMBER_TEMPERATURES_SENSOR)) //Check enough data in rolling buffer->apply filter
				{
					//Check if temperature is change in range.
					if(abs(setting.temperature - cur_chamber_temp) <= MAX_TEMPERATURE_DIFF_FILTER)
					{
						tem_roll_put(CHAMBER_TEMPERATURES_SENSOR, cur_chamber_temp);
					}
				}else //Not enough data-> just push data to rolling temperature
				{
					tem_roll_put(CHAMBER_TEMPERATURES_SENSOR, cur_chamber_temp);
				}
			}
		}
	}



	setting.temperature = expRunningAverage(setting.temperature, (tem_roll_get(CHAMBER_TEMPERATURES_SENSOR)));
	setting.second_temperature = (int16_t)((rtd_get_temperature(AMBIENT_TEMPERATURE_SENSOR)))  + setting.temp_offset;
	//Get bat status
	setting.bat_value = get_bat_value();
	setting.bat_state = get_bat_state();
	//Get Lid state
	setting.lid_state = get_lid_state();

	// Feature logic 27 on/off each 60s
	interval_count.pwr_12v += 1;
	if(interval_count.pwr_12v == SECOND_TO_COUNT(POWER_12V_RESET_INTERVAL))
	{
		pwr_12v_off();
	}

	if(interval_count.pwr_12v >= (SECOND_TO_COUNT(POWER_12V_RESET_INTERVAL) + SECOND_TO_COUNT(2)))
	{
		pwr_12v_on();
		interval_count.pwr_12v = 0;
	}



	//Check exeed temperature setting
	if(setting.op_mode == OPERATION_MODE_FRIDEGE)
	{
		htr_off(); //Heater on in freezer mode,off in refrigerator off

		//Deviation logic control compressor
		if(tem_roll_enough_data(CHAMBER_TEMPERATURES_SENSOR))
		{
			if(setting.temperature <= setting.setpoint_fridge) //Reach setpoint -> Off compressor
			{
				ctl.cmprsr = TURN_OFF;
			}else if(setting.temperature >= (setting.setpoint_fridge + setting.deviation_fridge)) //Reach deviation->Need down temperature
			{
				ctl.cmprsr = TURN_ON;
			}
		}

		if(setting.temperature < (setting.setpoint_fridge - setting.alarm_temperature_deviation))    //Check under min temperature
		{
			//Increase count for delay check later
			alarm_count.under_min_temp += 1;
			alarm_count.over_max_temp = 0;

		}else if(setting.temperature > (setting.setpoint_fridge + setting.alarm_temperature_deviation))//Check over max temperature
		{
			//Increase count for delay check later
			alarm_count.over_max_temp += 1;
			alarm_count.under_min_temp = 0;
		}else
		{
			alarm_count.over_max_temp = 0;
			alarm_count.under_min_temp = 0;
		}
	}else if(setting.op_mode == OPERATION_MODE_FREEZER)
	{
		htr_on(); //Heater on in freezer mode,off in refrigerator mode

		//Deviation logic control compressor
		if(tem_roll_enough_data(CHAMBER_TEMPERATURES_SENSOR))
		{
			if(setting.temperature <= setting.setpoint_freezer) //Reach setpoint -> Off compressor
			{
				ctl.cmprsr = TURN_OFF;
			}else if(setting.temperature >= (setting.setpoint_freezer + setting.deviation_freezer)) //Reach deviation->on compressor
			{
				ctl.cmprsr = TURN_ON;
			}
		}
		if(setting.temperature < (setting.setpoint_freezer - setting.alarm_temperature_deviation))   //Check under min temperature
		{
			//Increase count for delay check later
			alarm_count.under_min_temp += 1;
			alarm_count.over_max_temp = 0;
		}else if (setting.temperature > (setting.setpoint_freezer + setting.alarm_temperature_deviation))   //Check over max temperature
		{
			//Increase count for delay check later
			alarm_count.over_max_temp += 1;
			alarm_count.under_min_temp = 0;
		}else
		{
			alarm_count.over_max_temp = 0;
			alarm_count.under_min_temp = 0;
		}
	}


	//Chamber fan always on: change in BUGID:17 Leave F2 running on all power modes
	ctl.fan2 = TURN_ON;


	//Lid check open
	if(setting.lid_state == LID_OPEN)
	{
		//Increase count for delay check later
		alarm_count.lid_open += 1;
		interval_count.lid = 0;
		//when lid is opened, compressor, condenser fan and chamber fan turned off.
		ctl.cmprsr = TURN_OFF;
		ctl.cmprsr_fan = TURN_OFF;
		ctl.fan2 = TURN_OFF;
	}else //Lid close
	{
		alarm_count.lid_open = 0;
		interval_count.lid ++;
	}



   //All turns back on when lid is closed although we require a compressor on delay of 1-2 mins(settable in service mode)
	if(interval_count.lid < MINUTE_TO_COUNT(LID_CLOSE_DELAY_MINS))
	{
		ctl.cmprsr = TURN_OFF;
		ctl.cmprsr_fan = TURN_OFF;
	}

	//If in Battery mode -> Fan only run when compressor run-> change to BUGID17:Leave F2 running on all power modes
//	if(setting.pwr_mode == POWER_MODE_BAT)
//	{
//		ctl.fan2 = ctl.cmprsr;
//	}


	if(interval_count.cmprsr) //Currently already delay on? -> just turn off cmprsr
	{
		ctl.cmprsr = TURN_OFF;
	}else
	{
		if((ctl_pre.cmprsr != ctl.cmprsr) && (ctl.cmprsr == TURN_OFF)) //Check if compressor change off->on?
		{
			interval_count.cmprsr = MINUTE_TO_COUNT(CMPRSR_DELAY_ON_MINS);    //Reset counter
		}
		//Get power status
		if(get_power_mode() != setting.pwr_mode) //Check switch power source
		{
			interval_count.cmprsr = MINUTE_TO_COUNT(CMPRSR_DELAY_ON_MINS); //Reset counter
		}
	}

	setting.pwr_mode = get_power_mode(); //Need to put this logic after check switch power source
	if(interval_count.cmprsr > 0) interval_count.cmprsr --;

	//Fan1 control tight to compressor
	ctl.fan1 = ctl.cmprsr;

	//Internal fan will off in freezer mode
	if(setting.op_mode == OPERATION_MODE_FREEZER)
	{
		ctl.fan1 = TURN_OFF;
	}

	ctl_pre.cmprsr = ctl.cmprsr;//Save back up
	if(ctl.cmprsr == TURN_ON)
	{
		setting.cmprsr = CMPRSR_STATE_ON;
		cmprsr_power_on();
	}else
	{
		setting.cmprsr = CMPRSR_STATE_OFF;
		cmprsr_power_off();
	}
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
	//Alarm down count
	if(alarm_count.alarm_mute > 0)
	{
		alarm_count.alarm_mute--; //Check alarm mute count
	}
  /*
   * Note lcd_state need to consider carefully for set lcd_state
   * */
	switch((uint8_t)lcd_state)
	{
		case LCD_MAIN_STATE:
				if(lcd.op_mode != setting.op_mode
				  || lcd.pwr_mode != setting.pwr_mode
				  || lcd.bat_state != setting.bat_state
				  || lcd.bat_value != setting.bat_value
				  || lcd.spk_mode != setting.spk_mode
				  || lcd.bat_signal != setting.bat_signal
				  || lcd.onoff != setting.onoff
                  || lcd.temperature != setting.temperature
				  || lcd.cmprsr != setting.cmprsr);
				{
					//Update lcd main frame
					lcd.op_mode = setting.op_mode;
					lcd.pwr_mode = setting.pwr_mode;
					lcd.temperature = setting.temperature;
					lcd.bat_state = setting.bat_state;
					lcd.bat_value = setting.bat_value;
					lcd.spk_mode = setting.spk_mode;
					lcd.bat_signal = setting.bat_signal;
					lcd.cmprsr = setting.cmprsr;
					//Reload main frame
					lcd_interface_show(lcd_state);
				}
				if(alarm_count.alarm_mute == 0)
				{
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
					}
				}
		break;
		case LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE:
		case LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE:
		case LCD_WARNING_TYPE_LID_OPEN_STATE:
			if(setting.warning_type == WARNING_TYPE_NONE)  //Waiting for end warning or alarm mute count reset
			{
				lcd_state = LCD_MAIN_STATE;
				lcd_interface_show(lcd_state);
			}else //Still warning.
			{
				//Need reload temperature to update lcd
				if(interval_count.temper > SECOND_TO_COUNT(TEMPERATURE_SHOW_INTERVAL))
				{
					if(lcd.temperature != setting.temperature)
					{
						lcd.temperature = setting.temperature;
						interval_count.temper = 0;
						lcd_interface_show(lcd_state); //Reload when temperature change
					}
				}
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
				logging_write(LOG_FILE_NAME, &setting);//Log imediataly when warning
				main_state = MAIN_WARNING_WAITING_STATE;//Move to waiting state.
			}else if(setting.warning_type == WARNING_TYPE_OVER_MAX_TEMP)
			{
				logging_write(LOG_FILE_NAME, &setting);//Log immediately when warning
				main_state = MAIN_WARNING_WAITING_STATE;//Move to waiting state.
			}else if(setting.warning_type == WARNING_TYPE_UNDER_MIN_TEMP)
			{
				logging_write(LOG_FILE_NAME, &setting);//Log immediately when warning
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
				alarm_count.alarm_mute = 0;
			}else //Still warning?
			{
				if(alarm_count.alarm_mute == 0)
				{
					setting.spk_mode = SPEAKER_MODE_ON; //Lcd speaker on again
					if(buzzer_state != BUZZER_ON_WATING_STATE)
					{
						buzzer_state = BUZZER_ON_STATE;
					}
				}else
				{
					setting.spk_mode = SPEAKER_MODE_OFF; //Lcd speaker off
					if(buzzer_state != BUZZER_OFF_WAITING_STATE)
					{
						buzzer_state = BUZZER_OFF_STATE;
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
	interval_count.logging++;
	if(interval_count.logging >= MINUTE_TO_COUNT(setting.logging_interval))
	{
		interval_count.logging = 0;
		logging_write(LOG_FILE_NAME, &setting);//Log when reach logging interval in setting.
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
	//Reset temperature to 0
	lcd.temperature = 0;
	//Get power mode
	setting.pwr_mode = get_power_mode();
	//LCD ui
	lcd_ui_init(); //Init lvgl, porting
	lcd_interface_init(); //Init api and show main frame
	event_add(led_task, &led_id, 10);
	event_active(&led_id);
	event_add(main_task, &main_app_id, MAIN_TASK_TICK_MS);
	event_active(&main_app_id);
	rtc_task(); //Update time when start;
	event_add(rtc_task, &rtc_time_id, RTC_TASK_TICK_MS);
	event_active(&rtc_time_id);
}
