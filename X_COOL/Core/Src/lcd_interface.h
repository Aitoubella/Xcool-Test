/*
 * lcd_interface.h
 *
 *  Created on: Oct 30, 2023
 *      Author: Loc
 */

#ifndef SRC_LCD_INTERFACE_H_
#define SRC_LCD_INTERFACE_H_
#include <stdint.h>
#include "lcd_ui.h"
typedef struct
{
	operation_mode_t op_mode;
	power_mode_t pwr_mode;
	speaker_mode_t spk_mode;
	battery_state_t bat_state;
	battery_signal_t bat_signal;
	datetime_t datetime;
	int16_t temperature;
	uint8_t bat_value;
	int8_t temperature_fridge;
	int8_t temperature_freezer;
	int8_t alarm_temperature_deviation;
	uint8_t alarm_temperature_delay;
	uint8_t alarm_bat;
	uint8_t alarm_lid;
	uint8_t logging_interval;
	int8_t temp_offset;
	uint8_t alarm_mute_duration;
	lid_state_t lid_state;
	warning_type_t warning_type;
}lcd_inter_t;

typedef enum
{
	//level 1
	LCD_MAIN_STATE = 0,
	LCD_OPERATION_MODE_STATE,
	LCD_SETTING_STATE,
	LCD_SERVICE_STATE,
	//level 2
	LCD_OPERATION_MODE_FRIDEGE_STATE,
	LCD_OPERATION_MODE_FREEZER_STATE,
	LCD_OPERATION_MODE_BACK_STATE,

	LCD_SETTING_DEFAULT_STATE,
	LCD_SETTING_DATETIME_STATE,
	LCD_SETTING_DOWNLOAD_DATA_STATE,
	LCD_SETTING_BACK_STATE,

	LCD_SERVICE_DEFAULT_STATE,
	LCD_SERVICE_TEMPERATURE_STATE,
	LCD_SERVICE_ALARM_STATE,
	LCD_SERVICE_DATA_LOGGING_STATE,
	LCD_SERVICE_CALIBRATION_STATE,
	LCD_SERVICE_BACK_STATE,
	//level 3
	LCD_SETTING_DATETIME_YEAR_STATE,
	LCD_SETTING_DATETIME_MONTH_STATE,
	LCD_SETTING_DATETIME_DAY_STATE,
	LCD_SETTING_DATETIME_HOUR_STATE,
	LCD_SETTING_DATETIME_MINUTE_STATE,
	LCD_SETTING_DATETIME_BACK_STATE,

	LCD_SETTING_DOWNLOAD_DATA_TO_USB_STATE,
	LCD_SETTING_DOWNLOAD_DATA_BACK_STATE,

	LCD_SERVICE_TEMPERATURE_FRIDGE_STATE,
	LCD_SERVICE_TEMPERATURE_FREEZER_STATE,
	LCD_SERVICE_TEMPERATURE_BACK_STATE,

	LCD_SERVICE_ALARMS_TEMPERATURE_STATE,
	LCD_SERVICE_ALARMS_BATTERY_STATE,
	LCD_SERVICE_ALARMS_LID_STATE,
	LCD_SERVICE_ALARMS_MUTE_AlARMS_STATE,
	LCD_SERVICE_ALARMS_BACK_STATE,

	LCD_SERVICE_DATA_LOGGING_INTERVAL_STATE,
	LCD_SERVICE_DATA_LOGGING_BACK_STATE,
	LCD_SERVICE_DATA_LOGGING_INTERVAL_SET_STATE,

	LCD_SERVICE_CALIBRATION_TEMP_OFFSET_STATE,
	LCD_SERVICE_CALIBRATION_BACK_STATE,
	LCD_SERVICE_CALIBRATION_TEMP_OFFSET_SET_STATE,


	//Level 4
	LCD_SETTING_DATETIME_YEAR_SET_STATE,
	LCD_SETTING_DATETIME_MONTH_SET_STATE,
	LCD_SETTING_DATETIME_DAY_SET_STATE,
	LCD_SETTING_DATETIME_HOUR_SET_STATE,
	LCD_SETTING_DATETIME_MIN_SET_STATE,

	LCD_SETTING_DOWNLOAD_DATA_CONTINUE_STATE,
	LCD_SETTING_DOWNLOAD_DATA_CANCEL_STATE,

	LCD_SETTING_DOWNLOAD_DATA_COMPLETE_STATE,


	LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_STATE,
	LCD_SERVICE_TEMPERATURE_FRIDGE_BACK_STATE,


	LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_STATE,
	LCD_SERVICE_TEMPERATURE_FREEZER_BACK_STATE,

	LCD_SERVICE_TEMPERATURE_FRIDGE_VALUE_SET_STATE,
	LCD_SERVICE_TEMPERATURE_FREEZER_VALUE_SET_STATE,

	LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_STATE,
	LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_STATE,
	LCD_SERVICE_ALARM_TEMP_BACK_STATE,

	LCD_SERVICE_ALARM_TEMP_TEMP_DEVIATION_SET_STATE,
	LCD_SERVICE_ALARM_TEMP_ALARM_DELAY_SET_STATE,

	LCD_SERVICE_ALARM_BAT_VALUE_STATE,
	LCD_SERVICE_ALARM_BAT_BACK_STATE,

	LCD_SERVICE_ALARM_BAT_VALUE_SET_STATE,

	LCD_SERVICE_ALARM_LID_VALUE_STATE,
	LCD_SERVICE_ALARM_LID_BACK_STATE,

	LCD_SERVICE_ALARM_LID_VALUE_SET_STATE,

	LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_STATE,
	LCD_SERVICE_ALARM_MUTE_DURATION_BACK_STATE,

	LCD_SERVICE_ALARM_MUTE_DURATION_VALUE_SET_STATE,

	//Turn off unit
	LCD_TURN_OFF_UNIT_NO_STATE,
	LCD_TURN_OFF_UNIT_YES_STATE,
	LCD_OFF_DISPLAY_WATING,

	//Warning
	LCD_WARNING_TYPE_UNDER_MIN_TEMP_STATE,
	LCD_WARNING_TYPE_OVER_MAX_TEMP_STATE,
	LCD_WARNING_TYPE_LID_OPEN_STATE,
}lcd_state_t;

typedef enum
{
	LCD_GET_TEMPERATURE_EVT = 0,
	LCD_GET_BAT_VALUE_EVT,
	LCD_SET_OPERATION_MODE_EVT,
	LCD_GET_OPERATION_PWR_MODE_EVT,
	LCD_GET_SPEAKER_MODE_EVT,
	LCD_GET_BAT_STATE_EVT,

	LCD_GET_DATETIME_EVT,
	LCD_SET_DATETIME_EVT,

	LCD_GET_TEMPERATURE_FRIDGE_EVT,
	LCD_SET_TEMPERATURE_FRIDGE_EVT,
	LCD_GET_TEMPERATURE_FREEZER_EVT,
	LCD_SET_TEMPERATURE_FREEZER_EVT,

	LCD_GET_ALARM_TEMPERATURE_DEVIATION_EVT,
	LCD_SET_ALARM_TEMPERATURE_DEVIATION_EVT,

	LCD_GET_ALARM_TEMPERATURE_DELAY_EVT,
	LCD_SET_ALARM_TEMPERATURE_DELAY_EVT,

	LCD_GET_ALARM_BAT_EVT,
	LCD_SET_ALARM_BAT_EVT,

	LCD_GET_ALARM_LID_EVT,
	LCD_SET_ALARM_LID_EVT,

	LCD_GET_LOGGING_INTERVAL_EVT,
	LCD_SET_LOGGING_INTERVAL_EVT,

	LCD_GET_TEMP_OFFSET_EVT,
	LCD_SET_TEMP_OFFSET_EVT,

	LCD_SET_LARM_MUTE_DURATION_EVT,

	LCD_USB_INSERT_DOWNLOAD_EVT,

	LCD_MAIN_FRAME_EVT,

	LCD_POWER_OFF_EVT,
	LCD_PWER_ON_EVT,

	LCD_POWER_SHORT_PRESS_EVT,
}lcd_get_set_evt_t;

void lcd_interface_init(void);
void lcd_interface_show(lcd_state_t state);
uint8_t lcd_get_set_cb(lcd_get_set_evt_t evt, void* value);

lcd_inter_t* lcd_interface_get_param(void);
lcd_state_t lcd_interface_get_state(void);

#endif /* SRC_LCD_INTERFACE_H_ */
