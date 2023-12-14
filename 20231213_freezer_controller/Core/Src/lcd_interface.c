/*
 * lcd_interface.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Loc
 */


#include "lcd_interface.h"
#include "lcd_ui.h"
#include "button.h"
#include "buzzer.h"
#include "DS1307.h"
#include "ili9341.h"
extern lcd_inter_t setting;

uint8_t lcd_state = LCD_MAIN_STATE;

lcd_inter_t lcd =
{
	.bat_state = BATTERY_STATE_NOT_CHARGE,
};

void button_cb(uint8_t btn_num, btn_evt_t evt)
{

	static uint8_t has_event = 0;
	switch(btn_num)
	{
	case BTN_ENTER:
		if(evt == BUTTON_SHORT_PRESS)
		{
			has_event = 1;
			switch(lcd_state)
			{
			//lcd turn off unit
			case LCD_TURN_OFF_UNIT_NO_STATE:
				lcd_state = LCD_MAIN_STATE;
				break;
			case LCD_TURN_OFF_UNIT_YES_STATE:
				lcd_get_set_cb(LCD_POWER_OFF_EVT, NULL);

				lcd_state = LCD_OFF_DISPLAY_WATING;
				break;
			case LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE:
			case LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE:
			case LCD_WARNING_TYPE_LID_OPEN_STATE:
				lcd_state = LCD_OPERATION_MODE_STATE; //if in state warning -> press enter to go to next setting
				break;
			//level 1
			case LCD_MAIN_STATE:
				//Load all current param
				memcpy((uint8_t *)&lcd,(uint8_t *)&setting, sizeof(lcd_inter_t));
				//Check operation mode
				if(lcd.op_mode == OPERATION_MODE_FRIDEGE)
				{
					lcd_state = LCD_OPERATION_MODE_FRIDEGE_STATE;
				}else
				{
					lcd_state = LCD_OPERATION_MODE_FREEZER_STATE;
				}
//				lcd_state = LCD_OPERATION_MODE_STATE;
				break;
			case LCD_OPERATION_MODE_STATE:
				lcd_state = LCD_SETTING_STATE;
				break;
			case LCD_SETTING_STATE:
				lcd_state = LCD_SERVICE_STATE;
				break;
			case LCD_SERVICE_STATE:
				lcd_state = LCD_MAIN_STATE;
				lcd_get_set_cb(LCD_MAIN_FRAME_EVT, NULL); //Callback event for save to flash logic in main app
				break;
			//level 2 enter to level 3
			//Operation mode
			case LCD_OPERATION_MODE_FREEZER_STATE:
				operation_mode_t op_mode = OPERATION_MODE_FREEZER;
				lcd_get_set_cb(LCD_SET_OPERATION_MODE_EVT, &op_mode);
				lcd_state = LCD_SETTING_STATE;
				break;
			case LCD_OPERATION_MODE_FRIDEGE_STATE:
				operation_mode_t op_mode_1 = OPERATION_MODE_FRIDEGE;
				lcd_get_set_cb(LCD_SET_OPERATION_MODE_EVT, &op_mode_1);
				lcd_state = LCD_SETTING_STATE;
				break;
			case LCD_OPERATION_MODE_BACK_STATE:
				lcd_state = LCD_SETTING_STATE;
				break;

			//setting date time
			case LCD_SETTING_DATETIME_STATE:
				memcpy((uint8_t *)&lcd.datetime, (uint8_t *)&setting.datetime, sizeof(lcd.datetime));
				lcd_state = LCD_SETTING_DATETIME_YEAR_STATE;
				break;
			//setting download data
			case LCD_SETTING_DOWNLOAD_DATA_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE;
				break;
			case LCD_SETTING_BACK_STATE:
				lcd_state = LCD_SETTING_STATE;
				break;
			//service temperature
			case LCD_SERVICE_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_STATE;
				break;
			//service alarm
			case LCD_SERVICE_ALARM_STATE:
				lcd_state = LCD_SERVICE_ALARMS_TEMPERATURE_STATE;
				break;
			case LCD_SERVICE_ALARMS_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE;
				break;
			case LCD_SERVICE_ALARMS_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_STATE;
				break;
			//service data loging
			case LCD_SERVICE_DATA_LOGGING_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_BACK_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_STATE;
				break;
			//service calibration
			case LCD_SERVICE_CALIBRATION_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_BACK_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_STATE;
				break;
			case LCD_SERVICE_BACK_STATE:
				lcd_state = LCD_SERVICE_STATE;
				break;
			//level 3 enter to level 4
			//date time
			case LCD_SETTING_DATETIME_YEAR_STATE:
				lcd_state = LCD_SETTING_DATETIME_YEAR_SET_STATE;
				break;
			case LCD_SETTING_DATETIME_MONTH_STATE:
				lcd_state = LCD_SETTING_DATETIME_MONTH_SET_STATE;
				break;
			case LCD_SETTING_DATETIME_DAY_STATE:
				lcd_state = LCD_SETTING_DATETIME_DAY_SET_STATE;
				break;
			case LCD_SETTING_DATETIME_HOUR_STATE:
				lcd_state = LCD_SETTING_DATETIME_HOUR_SET_STATE;
				break;
			case LCD_SETTING_DATETIME_MINUTE_STATE:
				lcd_state = LCD_SETTING_DATETIME_MIN_SET_STATE;
				break;
			case LCD_SETTING_DATETIME_BACK_STATE:
				lcd_get_set_cb(LCD_SET_DATETIME_EVT, &lcd.datetime);
				lcd_state = LCD_SETTING_DATETIME_STATE;
				break;
			//Back datetime when set done
			case LCD_SETTING_DATETIME_YEAR_SET_STATE:
				lcd_state = LCD_SETTING_DATETIME_YEAR_STATE;
				break;
			case LCD_SETTING_DATETIME_MONTH_SET_STATE:
				lcd_state = LCD_SETTING_DATETIME_MONTH_STATE;
				break;
			case LCD_SETTING_DATETIME_DAY_SET_STATE:
				lcd_state = LCD_SETTING_DATETIME_DAY_STATE;
				break;
			case LCD_SETTING_DATETIME_HOUR_SET_STATE:
				lcd_state = LCD_SETTING_DATETIME_HOUR_STATE;
				break;
			case LCD_SETTING_DATETIME_MIN_SET_STATE:
				lcd_state = LCD_SETTING_DATETIME_MINUTE_STATE;
				break;
			//Download data
			case LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_BACK_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE:
				//USB host init cb
				lcd_get_set_cb(LCD_USB_INSERT_DOWNLOAD_EVT,NULL);
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_COMPLETE_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_COMPLETE_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE;
				break;
			//Temperature go in set
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE;
				break;
			//Temperature back to previous
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE:
				lcd_get_set_cb(LCD_SET_TEMPERATURE_FRIDGE_EVT, &lcd.temperature_fridge);
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE:
				lcd_get_set_cb(LCD_SET_TEMPERATURE_FREEZER_EVT, &lcd.temperature_freezer);
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_STATE;
				break;
			//Alarm temp
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_TEMPERATURE_STATE;
				break;
			//Alarm temp deviation set
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE:
				lcd_get_set_cb(LCD_SET_ALARM_TEMPERATURE_DEVIATION_EVT, &lcd.alarm_temperature_deviation);
				lcd_state = LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE;
				break;
			//Alarm delay set
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE:
				lcd_get_set_cb(LCD_SET_ALARM_TEMPERATURE_DELAY_EVT, &lcd.alarm_temperature_delay);
				lcd_state = LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE;
				break;
			//Alarm bat
			case LCD_SERVICE_ALARMS_BATTERY_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_BATTERY_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE:
				lcd_get_set_cb(LCD_SET_ALARM_BAT_EVT, &lcd.alarm_bat);
				lcd_state = LCD_SERVICE_ALARM_BAT_VALUE_STATE;
				break;
			//Alarm Lid
			case LCD_SERVICE_ALARMS_LID_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_VALUE_SET_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_LID_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_VALUE_SET_STATE:
				lcd_get_set_cb(LCD_SET_ALARM_LID_EVT, &lcd.alarm_lid);
				lcd_state = LCD_SERVICE_ALARM_LID_VALUE_STATE;
				break;
			//Alarm Mute
			case LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE:
				lcd_get_set_cb(LCD_SET_LARM_MUTE_DURATION_EVT, &lcd.alarm_mute_duration);
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE;
				break;
			//Service data logging interval
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE:
				lcd_get_set_cb(LCD_SET_LOGGING_INTERVAL_EVT, &lcd.logging_interval);
				lcd_state = LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE;
				break;
			//Service calibration temperature offset
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE:
				lcd_get_set_cb(LCD_SET_TEMP_OFFSET_EVT, &lcd.temp_offset);
				lcd_state = LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE;
				break;
			}

		}else if(evt == BUTTON_HOLD_2_SEC)
		{
			if(lcd_state == LCD_MAIN_STATE)
			{
				has_event  = 1;
				lcd_turn_off_unit(DISPLAY_UINIT_NO);
				lcd_state = LCD_TURN_OFF_UNIT_NO_STATE;
			}else if(lcd_state == LCD_OFF_DISPLAY_WATING)
			{
				lcd_state = LCD_MAIN_STATE;
				lcd_get_set_cb(LCD_PWER_ON_EVT, NULL);
			}
		}
		break;
	case BTN_UP:
		if(evt == BUTTON_SHORT_PRESS)
		{
			has_event  = 1;
			switch(lcd_state)
			{
			//lcd turn off unit
			case LCD_TURN_OFF_UNIT_NO_STATE:
				lcd_state = LCD_TURN_OFF_UNIT_YES_STATE;
				break;
			case LCD_TURN_OFF_UNIT_YES_STATE:
				lcd_state = LCD_TURN_OFF_UNIT_NO_STATE;
				break;
			//level 2
			//Operation
			case LCD_OPERATION_MODE_STATE:
				lcd_state = LCD_OPERATION_MODE_FRIDEGE_STATE;
				break;
			case LCD_OPERATION_MODE_FRIDEGE_STATE:
				lcd_state = LCD_OPERATION_MODE_BACK_STATE;
				break;
			case LCD_OPERATION_MODE_BACK_STATE:
				lcd_state = LCD_OPERATION_MODE_FREEZER_STATE;
				break;
			case LCD_OPERATION_MODE_FREEZER_STATE:
				lcd_state = LCD_OPERATION_MODE_FRIDEGE_STATE;
				break;

			//Setting
			case LCD_SETTING_STATE:
				lcd_state = LCD_SETTING_BACK_STATE;
				break;
			case LCD_SETTING_BACK_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_STATE:
				lcd_state = LCD_SETTING_DATETIME_STATE;
				break;
			case LCD_SETTING_DATETIME_STATE:
				lcd_state = LCD_SETTING_BACK_STATE;
				break;
				//Service
			case LCD_SERVICE_STATE:
				lcd_state = LCD_SERVICE_BACK_STATE;
				break;
			case LCD_SERVICE_BACK_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_STATE:
				lcd_state = LCD_SERVICE_ALARM_STATE;
				break;
			case LCD_SERVICE_ALARM_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_BACK_STATE;
				break;
			//level 3
			//setting date time
			case LCD_SETTING_DATETIME_YEAR_STATE:
				lcd_state = LCD_SETTING_DATETIME_BACK_STATE;
				break;
			case LCD_SETTING_DATETIME_BACK_STATE:
				lcd_state = LCD_SETTING_DATETIME_MINUTE_STATE;
				break;
			case LCD_SETTING_DATETIME_MINUTE_STATE:
				lcd_state = LCD_SETTING_DATETIME_HOUR_STATE;
				break;
			case LCD_SETTING_DATETIME_HOUR_STATE:
				lcd_state = LCD_SETTING_DATETIME_DAY_STATE;
				break;
			case LCD_SETTING_DATETIME_DAY_STATE:
				lcd_state = LCD_SETTING_DATETIME_MONTH_STATE;
				break;
			case LCD_SETTING_DATETIME_MONTH_STATE:
				lcd_state = LCD_SETTING_DATETIME_YEAR_STATE;
				break;
			//setting  download data
			case LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_BACK_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_BACK_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE;
				break;
			//service temperature
			case LCD_SERVICE_TEMPERATURE_FRIDGE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_STATE;
				break;
			//service alarms
			case LCD_SERVICE_ALARMS_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_ALARMS_BACK_STATE;
				break;
			case LCD_SERVICE_ALARMS_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE;
				break;
			case LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE:
				lcd_state = LCD_SERVICE_ALARMS_LID_STATE;
				break;
			case LCD_SERVICE_ALARMS_LID_STATE:
				lcd_state = LCD_SERVICE_ALARMS_BATTERY_STATE;
				break;
			case LCD_SERVICE_ALARMS_BATTERY_STATE:
				lcd_state = LCD_SERVICE_ALARMS_TEMPERATURE_STATE;
				break;
			//service data logging
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_BACK_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_BACK_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE:
				lcd.logging_interval ++;
				break;
			//service calibration
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_BACK_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_BACK_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE;
				break;
			//level 4
			//setting date time  year,month,day,hour,min
			case LCD_SETTING_DATETIME_YEAR_SET_STATE:
				lcd.datetime.year ++;
				break;
			case LCD_SETTING_DATETIME_MONTH_SET_STATE:
				lcd.datetime.month ++;
				if(lcd.datetime.month > 12) lcd.datetime.month = 1;
				break;
			case LCD_SETTING_DATETIME_DAY_SET_STATE:
				lcd.datetime.day ++;
				if(lcd.datetime.day > 31) lcd.datetime.day = 1;
				break;
			case LCD_SETTING_DATETIME_HOUR_SET_STATE:
				lcd.datetime.hour ++;
				if(lcd.datetime.hour > 23) lcd.datetime.hour = 0;
				break;
			case LCD_SETTING_DATETIME_MIN_SET_STATE:
				lcd.datetime.minute ++;
				if(lcd.datetime.minute > 59) lcd.datetime.minute = 0;
				break;
			//download data
			case LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE;
				break;
			//Temperature set
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE:
				lcd.temperature_fridge ++;
				break;

			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE;
				break;

			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE:
				lcd.temperature_freezer ++;
				break;
			//Alarm temp deviation set
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE:
				lcd.alarm_temperature_deviation ++;
				break;
			//Alarm delay set
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE:
				lcd.alarm_temperature_delay ++;
				break;
			//Alarm bat
			case LCD_SERVICE_ALARM_BAT_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE:
				lcd.alarm_bat ++;
				if(lcd.alarm_bat > 99) lcd.alarm_bat = 1;
				break;
			//Alarm Lid
			case LCD_SERVICE_ALARM_LID_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_VALUE_SET_STATE:
				lcd.alarm_lid ++;
				break;
			//Alarm mute duration
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE:
				lcd.alarm_mute_duration ++;
				break;
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE:
				lcd.temp_offset ++;
				break;
			}
		}
		break;
	case BTN_DOWN:
		if(evt == BUTTON_SHORT_PRESS)
		{
			has_event  = 1;
			switch(lcd_state)
			{
			//lcd turn off unit
			case LCD_TURN_OFF_UNIT_NO_STATE:
				lcd_state = LCD_TURN_OFF_UNIT_YES_STATE;
				break;
			case LCD_TURN_OFF_UNIT_YES_STATE:
				lcd_state = LCD_TURN_OFF_UNIT_NO_STATE;
				break;
			//level 2
			//Operation
			case LCD_OPERATION_MODE_STATE:
				lcd_state = LCD_OPERATION_MODE_FREEZER_STATE;
				break;
			case LCD_OPERATION_MODE_FREEZER_STATE:
				lcd_state = LCD_OPERATION_MODE_BACK_STATE;
				break;
			case LCD_OPERATION_MODE_BACK_STATE:
				lcd_state = LCD_OPERATION_MODE_FRIDEGE_STATE;
				break;
			case LCD_OPERATION_MODE_FRIDEGE_STATE:
				lcd_state = LCD_OPERATION_MODE_FREEZER_STATE;
				break;
				//Setting
			case LCD_SETTING_STATE:
				lcd_state = LCD_SETTING_DATETIME_STATE;
				break;
			case LCD_SETTING_DATETIME_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_STATE:
				lcd_state = LCD_SETTING_BACK_STATE;
				break;
			case LCD_SETTING_BACK_STATE:
				lcd_state = LCD_SETTING_DATETIME_STATE;
				break;
				//Service
			case LCD_SERVICE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_ALARM_STATE;
				break;
			case LCD_SERVICE_ALARM_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_STATE:
				lcd_state = LCD_SERVICE_BACK_STATE;
				break;
			case LCD_SERVICE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_STATE;
				break;
			//level 3
			//setting date time
			case LCD_SETTING_DATETIME_YEAR_STATE:
				lcd_state = LCD_SETTING_DATETIME_MONTH_STATE;
				break;
			case LCD_SETTING_DATETIME_MONTH_STATE:
				lcd_state = LCD_SETTING_DATETIME_DAY_STATE;
				break;
			case LCD_SETTING_DATETIME_DAY_STATE:
				lcd_state = LCD_SETTING_DATETIME_HOUR_STATE;
				break;
			case LCD_SETTING_DATETIME_HOUR_STATE:
				lcd_state = LCD_SETTING_DATETIME_MINUTE_STATE;
				break;
			case LCD_SETTING_DATETIME_MINUTE_STATE:
				lcd_state = LCD_SETTING_DATETIME_BACK_STATE;
				break;
			case LCD_SETTING_DATETIME_BACK_STATE:
				lcd_state = LCD_SETTING_DATETIME_YEAR_STATE;
				break;
			//setting  download data
			case LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_BACK_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_BACK_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE;
				break;
			//service temperature
			case LCD_SERVICE_TEMPERATURE_FRIDGE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_STATE;


			//service alarms
			case LCD_SERVICE_ALARMS_TEMPERATURE_STATE:
				lcd_state = LCD_SERVICE_ALARMS_BATTERY_STATE;
				break;
			case LCD_SERVICE_ALARMS_BATTERY_STATE:
				lcd_state = LCD_SERVICE_ALARMS_LID_STATE;
				break;
			case LCD_SERVICE_ALARMS_LID_STATE:
				lcd_state = LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE;
				break;
			case LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE:
				lcd_state = LCD_SERVICE_ALARMS_BACK_STATE;
				break;
			case LCD_SERVICE_ALARMS_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARMS_TEMPERATURE_STATE;
				break;
			//service data logging
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_BACK_STATE;
				break;
			case LCD_SERVICE_DATA_LOGGING_BACK_STATE:
				lcd_state = LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE;
				break;
			//service calibration
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_BACK_STATE;
				break;
			case LCD_SERVICE_CALIBRATION_BACK_STATE:
				lcd_state = LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE;
				break;
			//Level 4
			//setting date time  year,month,day,hour,min
			case LCD_SETTING_DATETIME_YEAR_SET_STATE:
				if(lcd.datetime.year < 2023) lcd.datetime.year = 2023;
				else lcd.datetime.year --;
				break;
			case LCD_SETTING_DATETIME_MONTH_SET_STATE:
				if(lcd.datetime.month <= 1) lcd.datetime.month = 12;
				else lcd.datetime.month --;
				break;
			case LCD_SETTING_DATETIME_DAY_SET_STATE:
				if(lcd.datetime.day <= 1) lcd.datetime.day = 31;
				else lcd.datetime.day --;
				break;
			case LCD_SETTING_DATETIME_HOUR_SET_STATE:
				if(lcd.datetime.hour < 1) lcd.datetime.hour = 23;
				else lcd.datetime.hour --;
				break;
			case LCD_SETTING_DATETIME_MIN_SET_STATE:
				if(lcd.datetime.minute < 1) lcd.datetime.minute = 59;
				else lcd.datetime.minute --;
				break;
			//Download data continue or back
			case LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE;
				break;
			case LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE:
				lcd_state = LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE;
				break;
			//Temperature set
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE:
				lcd.temperature_fridge --;
				break;

			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE;
				break;
			case LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE:
				lcd_state = LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE;
				break;

			case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE:
				lcd.temperature_freezer --;
				break;
			//Alarm temp deviation set
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE;
				break;
			case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE:
				lcd.alarm_temperature_deviation -- ;
				break;
			//Alarm delay set
			case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE:
				if(lcd.alarm_temperature_delay > 0) lcd.alarm_temperature_delay -- ;
				break;
			//Alarm bat
			case LCD_SERVICE_ALARM_BAT_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_BAT_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE:
				if(lcd.alarm_bat <= 1) lcd.alarm_bat = 99;
				else lcd.alarm_bat -- ;
				break;
			//Alarm Lid
			case LCD_SERVICE_ALARM_LID_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_LID_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_LID_VALUE_SET_STATE:
				if(lcd.alarm_lid > 0) lcd.alarm_lid -- ;
				break;
			//Alarm mute duration
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE:
				lcd_state = LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE;
				break;
			case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE:
				if(lcd.alarm_mute_duration > 0) lcd.alarm_mute_duration -- ;
				break;
			case LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE:
				if(lcd.logging_interval > 1) lcd.logging_interval --;
				break;
			case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE:
				lcd.temp_offset --;
				break;
			}
		}
	}
	if(has_event)
	{
		lcd_interface_show(lcd_state);
		has_event  = 0;
	}
}


void lcd_interface_show(lcd_state_t state)
{
	lcd_ui_clear();
	switch((uint8_t)state)
	{
	case LCD_TURN_OFF_UNIT_NO_STATE:
		lcd_turn_off_unit(DISPLAY_UINIT_NO);
		break;
	case LCD_TURN_OFF_UNIT_YES_STATE:
		lcd_turn_off_unit(DISPLAY_UINIT_YES);
		break;
	case LCD_MAIN_STATE:
		lcd_main_screen_screen(lcd.spk_mode, lcd.temperature, lcd.pwr_mode, lcd.op_mode, lcd.bat_value, lcd.bat_state,lcd.bat_signal);
		break;
	case LCD_OPERATION_MODE_STATE:
		lcd_operation_mode_screen(lcd.op_mode);
		break;
	case LCD_SETTING_STATE:
		lcd_setting(SETTING_DEFAULT);
		break;
	case LCD_SERVICE_STATE:
		lcd_service(SERVICE_DEFAULT);
		break;
	//Level 2
	case LCD_OPERATION_MODE_FREEZER_STATE:
		lcd_operation_mode_screen(OPERATION_MODE_FREEZER);
		break;
	case LCD_OPERATION_MODE_FRIDEGE_STATE:
		lcd_operation_mode_screen(OPERATION_MODE_FRIDEGE);
		break;
	case LCD_OPERATION_MODE_BACK_STATE:
		lcd_operation_mode_screen(OPERATION_MODE_BACK);
		break;

	case LCD_SETTING_DEFAULT_STATE:
		lcd_setting(SETTING_DEFAULT);
		break;
	case LCD_SETTING_DATETIME_STATE:
		lcd_setting(SETTING_DATETIME);
		break;
	case LCD_SETTING_DOWNLOAD_DATA_STATE:
		lcd_setting(SETTING_DOWNLOAD_DATA);
		break;
	case LCD_SETTING_BACK_STATE:
		lcd_setting(SETTING_BACK);
		break;

	case LCD_SERVICE_DEFAULT_STATE:
		lcd_service(SERVICE_DEFAULT);
		break;
	case LCD_SERVICE_TEMPERATURE_STATE:
		lcd_service(SERVICE_TEMPERATURE);
		break;
	case LCD_SERVICE_ALARM_STATE:
		lcd_service(SERVICE_ALARM);
		break;
	case LCD_SERVICE_DATA_LOGGING_STATE:
		lcd_service(SERVICE_DATA_LOGGING);
		break;
	case LCD_SERVICE_CALIBRATION_STATE:
		lcd_service(SERVICE_CALIBRATION);
		break;
	case LCD_SERVICE_BACK_STATE:
		lcd_service(SERVICE_BACK);
		break;

	//Level 3
	case LCD_SETTING_DATETIME_YEAR_STATE:
		lcd_setting_datetime(SETTING_DATETIME_YEAR, &lcd.datetime);
		break;
	case LCD_SETTING_DATETIME_MONTH_STATE:
		lcd_setting_datetime(SETTING_DATETIME_MONTH, &lcd.datetime);
		break;
	case LCD_SETTING_DATETIME_DAY_STATE:
		lcd_setting_datetime(SETTING_DATETIME_DAY, &lcd.datetime);
		break;
	case LCD_SETTING_DATETIME_HOUR_STATE:
		lcd_setting_datetime(SETTING_DATETIME_HOUR, &lcd.datetime);
		break;
	case LCD_SETTING_DATETIME_MINUTE_STATE:
		lcd_setting_datetime(SETTING_DATETIME_MINUTE, &lcd.datetime);
		break;
	case LCD_SETTING_DATETIME_BACK_STATE:
		lcd_setting_datetime(SETTING_DATETIME_BACK, &lcd.datetime);
		break;

	case LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE:
		lcd_setting_download_data(SETTING_DOWNLOAD_DATA_TO_USB);
		break;
	case LCD_SETTING_DOWNLOAD_DATA_BACK_STATE:
		lcd_setting_download_data(SETTING_DOWNLOAD_DATA_BACK);
		break;

	case LCD_SERVICE_TEMPERATURE_FRIDGE_STATE:
		lcd_service_temperature(SERVICE_TEMPERATURE_FRIDGE);
		break;
	case LCD_SERVICE_TEMPERATURE_FREEZER_STATE:
		lcd_service_temperature(SERVICE_TEMPERATURE_FREEZER);
		break;
	case LCD_SERVICE_TEMPERATURE_BACK_STATE:
		lcd_service_temperature(SERVICE_TEMPERATURE_BACK);
		break;

	case LCD_SERVICE_ALARMS_TEMPERATURE_STATE:
		lcd_service_alarms(SERVICE_ALARMS_TEMPERATURE);
		break;
	case LCD_SERVICE_ALARMS_BATTERY_STATE:
		lcd_service_alarms(SERVICE_ALARMS_BATTERY);
		break;
	case LCD_SERVICE_ALARMS_LID_STATE:
		lcd_service_alarms(SERVICE_ALARMS_LID);
		break;
	case LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE:
		lcd_service_alarms(SERVICE_ALARMS_MUTE_AlARMS);
		break;
	case LCD_SERVICE_ALARMS_BACK_STATE:
		lcd_service_alarms(SERVICE_ALARMS_BACK);
		break;

	case LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE:
		lcd_service_data_logging(SERVICE_DATA_LOGGING_INTERVAL);
		break;
	case LCD_SERVICE_DATA_LOGGING_BACK_STATE:
		lcd_service_data_logging(SERVICE_DATA_LOGGING_BACK);
		break;

	case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE:
		lcd_service_calibration(SERVICE_CALIBRATION_TEMP_OFFSET);
		break;
	case LCD_SERVICE_CALIBRATION_BACK_STATE:
		lcd_service_calibration(SERVICE_CALIBRATION_BACK);
		break;

	//Level 4
	case LCD_SETTING_DATETIME_YEAR_SET_STATE:
		lcd_setting_datetime_year_set(lcd.datetime.year);
		break;
	case LCD_SETTING_DATETIME_MONTH_SET_STATE:
		lcd_setting_datetime_month_set(lcd.datetime.month);
		break;
	case LCD_SETTING_DATETIME_DAY_SET_STATE:
		lcd_setting_datetime_day_set(lcd.datetime.day);
		break;
	case LCD_SETTING_DATETIME_HOUR_SET_STATE:
		lcd_setting_datetime_hour_set(lcd.datetime.hour);
		break;
	case LCD_SETTING_DATETIME_MIN_SET_STATE:
		lcd_setting_datetime_min_set(lcd.datetime.minute);
		break;

	case LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE:
		lcd_setting_download_data_insert(SETTING_DOWNLOAD_DATA_CONTINUE);
		break;
	case LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE:
		lcd_setting_download_data_insert(SETTING_DOWNLOAD_DATA_CANCEL);
		break;
	case LCD_SETTING_DOWNLOAD_DATA_COMPLETE_STATE:
		lcd_setting_download_data_complete();
		break;

	case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE:
		lcd_service_temperature_fridge(SERVICE_TEMPERATURE_FRIDGE_VALUE, lcd.temperature_fridge);
		break;
	case LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE:
		lcd_service_temperature_fridge(SERVICE_TEMPERATURE_FRIDGE_BACK, lcd.temperature_fridge);
		break;
	case LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE:
		lcd_service_temperature_fridge_set(lcd.temperature_fridge);
		break;

	case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE:
		lcd_service_temperature_freezer(SERVICE_TEMPERATURE_FRIDGE_VALUE, lcd.temperature_freezer);
		break;
	case LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE:
		lcd_service_temperature_freezer(SERVICE_TEMPERATURE_FRIDGE_BACK, lcd.temperature_freezer);
		break;
	case LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE:
		lcd_service_temperature_freezer_set(lcd.temperature_freezer);
		break;

	case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE:
		lcd_service_alarm_temperature(SERVICE_ALARM_TEMP_TEMP_DEVIATION);
		break;
	case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE:
		lcd_service_alarm_temperature(SERVICE_ALARM_TEMP_ALARM_DELAY);
		break;
	case LCD_SERVICE_ALARM_TEMP_BACK_STATE:
		lcd_service_alarm_temperature(SERVICE_ALARM_TEMP_BACK);
		break;


	case LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE:
		lcd_service_alarm_temperature_temp_deviation_set(lcd.alarm_temperature_deviation);
		break;
	case LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE:
		lcd_service_alarm_temperature_alarm_delay_set(lcd.alarm_temperature_delay);
		break;

	case LCD_SERVICE_ALARM_BAT_VALUE_STATE:
		lcd_service_alarm_bat(SERVICE_ALARM_BAT_VALUE,lcd.alarm_bat);
		break;
	case LCD_SERVICE_ALARM_BAT_BACK_STATE:
		lcd_service_alarm_bat(SERVICE_ALARM_BAT_BACK,lcd.alarm_bat);
		break;
	case LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE:
		lcd_service_alarm_bat_set(lcd.alarm_bat);
		break;

	case LCD_SERVICE_ALARM_LID_VALUE_STATE:
		lcd_service_alarm_lid(SERVICE_ALARM_LID_VALUE,lcd.alarm_lid);
		break;
	case LCD_SERVICE_ALARM_LID_BACK_STATE:
		lcd_service_alarm_lid(SERVICE_ALARM_LID_BACK, lcd.alarm_lid);
		break;
	case LCD_SERVICE_ALARM_LID_VALUE_SET_STATE:
		lcd_service_alarm_lid_set(lcd.alarm_lid);
		break;

	case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE:
		lcd_service_alarms_mute_duration(SERVICE_ALARM_MUTE_DURATION_VALUE,lcd.alarm_mute_duration);
		break;
	case LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE:
		lcd_service_alarms_mute_duration(SERVICE_ALARM_MUTE_DURATION_BACK,lcd.alarm_mute_duration);
		break;
	case LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE:
		lcd_service_alarms_mute_duration_set(lcd.alarm_mute_duration);
		break;

	case LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE:
		lcd_service_data_logging_set(lcd.logging_interval);
		break;

	case LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE:
		lcd_service_calibration_set(lcd.temp_offset);
		break;
	case LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE:
		lcd_service_alarms_warning(lcd.op_mode, WARNING_TYPE_UNDER_MIN_TEMP);
		break;
	case LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE:
		lcd_service_alarms_warning(lcd.op_mode, WARNING_TYPE_OVER_MAX_TEMP);
		break;
	case LCD_WARNING_TYPE_LID_OPEN_STATE:
		lcd_service_alarms_warning(lcd.op_mode, WARNING_TYPE_LID_OPEN);
		break;
	}
	lcd_ui_refresh();
}

lcd_state_t lcd_interface_get_state(void)
{
	return lcd_state;
}

lcd_inter_t* lcd_interface_get_param(void)
{
	return &lcd;
}

void lcd_interface_init(void)
{
	lcd_ui_clear();
	lcd_interface_show(LCD_MAIN_STATE);
	lcd_ui_load_screen();
	button_init(button_cb);
}
