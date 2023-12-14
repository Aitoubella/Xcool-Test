/*
 * lcd_ui.h
 *
 *  Created on: Oct 18, 2023
 *      Author: Loc
 */

#ifndef SRC_LCD_UI_H_
#define SRC_LCD_UI_H_
#include "lvgl.h"

LV_FONT_DECLARE( ui_font_verdana244);
LV_FONT_DECLARE( ui_font_verdana364);
LV_FONT_DECLARE( ui_font_verdana404);
LV_FONT_DECLARE( ui_font_verdana704);
LV_FONT_DECLARE( ui_font_verdana1004);
LV_FONT_DECLARE( ui_font_verdana1204);
LV_FONT_DECLARE( ui_font_verdana1404);


#define FONT_VERDENA_24 {get_label(),&ui_font_verdana244} //Font verdena
#define FONT_VERDENA_36 {get_label(),&ui_font_verdana364} //Font verdena
#define FONT_VERDENA_40 {get_label(),&ui_font_verdana404} //Font verdena
#define FONT_VERDENA_70 {get_label(),&ui_font_verdana704} //Font verdena
#define FONT_VERDENA_100 {get_label(),&ui_font_verdana1004} //Font verdena
#define FONT_VERDENA_120 {get_label(),&ui_font_verdana1204} //Font verdena
#define FONT_VERDENA_140 {get_label(),&ui_font_verdana1404} //Font verdena

typedef enum
{
	OPERATION_MODE_FRIDEGE = 0,
	OPERATION_MODE_FREEZER,
	OPERATION_MODE_BACK,
}operation_mode_t;

typedef enum
{
	POWER_MODE_BAT = 0,
	POWER_MODE_DC,
	POWER_MODE_AC,
}power_mode_t;

typedef enum
{
	SPEAKER_MODE_OFF = 0,
	SPEAKER_MODE_ON
}speaker_mode_t;

typedef enum
{
	BATTERY_STATE_CHARGING = 0,
	BATTERY_STATE_NOT_CHARGE,
}battery_state_t;

typedef enum
{
	BATTERY_NORMAL = 0,
	BATTER_WARNING_LOW,
	BATTERY_OUT_OF_BAT,
}battery_signal_t;

typedef enum
{
	DISPLAY_UINIT_NO = 0,
	DISPLAY_UINIT_YES,
}display_unit_t;

typedef enum
{
	SETTING_DEFAULT = 0,
	SETTING_DATETIME,
	SETTING_DOWNLOAD_DATA,
	SETTING_BACK
}setting_t;

typedef enum
{
	SETTING_DATETIME_YEAR = 0,
	SETTING_DATETIME_MONTH,
	SETTING_DATETIME_DAY,
	SETTING_DATETIME_HOUR,
	SETTING_DATETIME_MINUTE,
	SETTING_DATETIME_BACK
}setting_datetime_t;

typedef enum
{
	SETTING_DOWNLOAD_DATA_TO_USB = 0,
	SETTING_DOWNLOAD_DATA_BACK
}setting_download_data_t;

typedef enum
{
	SETTING_DOWNLOAD_DATA_CONTINUE = 0,
	SETTING_DOWNLOAD_DATA_CANCEL
}setting_download_data_insert_t;


typedef enum
{
	SERVICE_DEFAULT = 0,
	SERVICE_TEMPERATURE,
	SERVICE_ALARM,
	SERVICE_DATA_LOGGING,
	SERVICE_CALIBRATION,
	SERVICE_BACK,
}service_t;

typedef enum
{
	SERVICE_TEMPERATURE_FRIDGE = 0,
	SERVICE_TEMPERATURE_FREEZER,
	SERVICE_TEMPERATURE_BACK,
}service_temperature_t;

typedef enum
{
	SERVICE_TEMPERATURE_FRIDGE_VALUE = 0,
	SERVICE_TEMPERATURE_FRIDGE_BACK,
}service_temperature_fridge_t;

typedef enum
{
	SERVICE_TEMPERATURE_FREEZER_VALUE = 0,
	SERVICE_TEMPERATURE_FREEZER_BACK,
}service_temperature_freezer_t;

typedef enum
{
	SERVICE_ALARMS_TEMPERATURE = 0,
	SERVICE_ALARMS_BATTERY,
	SERVICE_ALARMS_LID,
	SERVICE_ALARMS_MUTE_AlARMS,
	SERVICE_ALARMS_BACK
}service_alarm_t;

typedef enum
{
	SERVICE_DATA_LOGGING_INTERVAL = 0,
	SERVICE_DATA_LOGGING_BACK,
}service_data_logging_t;

typedef enum
{
	SERVICE_CALIBRATION_TEMP_OFFSET = 0,
	SERVICE_CALIBRATION_BACK,
}service_calibration_t;

typedef enum
{
	SERVICE_ALARM_TEMP_TEMP_DEVIATION= 0,
	SERVICE_ALARM_TEMP_ALARM_DELAY,
	SERVICE_ALARM_TEMP_BACK
}service_alarms_temperature_t;

typedef enum
{
	SERVICE_ALARM_BAT_VALUE = 0,
	SERVICE_ALARM_BAT_BACK
}service_alarms_bat_t;

typedef enum
{
	SERVICE_ALARM_LID_VALUE = 0,
	SERVICE_ALARM_LID_BACK
}service_alarms_lid_t;

typedef enum
{
	SERVICE_ALARM_MUTE_DURATION_VALUE = 0,
	SERVICE_ALARM_MUTE_DURATION_BACK
}service_alarms_mute_duration_t;



typedef enum
{
	WARNING_TYPE_NONE = 0,
	WARNING_TYPE_UNDER_MIN_TEMP,
	WARNING_TYPE_OVER_MAX_TEMP,
	WARNING_TYPE_LID_OPEN,
}warning_type_t;

typedef enum
{
	LID_CLOSE = 0,
	LID_OPEN,
}lid_state_t;
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}datetime_t;

typedef struct
{
	lv_obj_t* obj;
	const lv_font_t* font;
}lcd_ui_t;

void lcd_ui_init(void);
void lcd_ui_refresh(void);
void lcd_ui_clear(void);
void lcd_ui_load_screen(void);
void lcd_setting(setting_t setting);
void lcd_main_screen_screen(speaker_mode_t sp_mode, int16_t temperature, power_mode_t pwr_mode, operation_mode_t op_mode, uint8_t bat_value, battery_state_t bat_st, battery_signal_t bat_signal);
void lcd_operation_mode_screen(uint8_t operation_mode);
void lcd_turn_off_unit(display_unit_t value);
void lcd_setting_datetime(setting_datetime_t index, datetime_t* time);
void lcd_setting_datetime_year_set(uint16_t year);
void lcd_setting_datetime_month_set(uint8_t month);
void lcd_setting_datetime_day_set(uint8_t day);
void lcd_setting_datetime_hour_set(uint8_t hour);
void lcd_setting_datetime_min_set(uint8_t min);
void lcd_setting_download_data(setting_download_data_t index);
void lcd_setting_download_data_insert(setting_download_data_insert_t index);
void lcd_setting_download_data_complete(void);
void lcd_service(service_t index);
void lcd_service_temperature(service_temperature_t index);
void lcd_service_alarms(service_alarm_t index);
void lcd_service_data_logging(service_data_logging_t index);
void lcd_service_data_logging_set(uint8_t value);
void lcd_service_calibration(service_calibration_t index);
void lcd_service_calibration_set(int8_t value);
void lcd_service_temperature_fridge(service_temperature_fridge_t index, int8_t value);
void lcd_service_temperature_fridge_set(int8_t value);
void lcd_service_temperature_freezer(service_temperature_freezer_t index, int8_t value);
void lcd_service_temperature_freezer_set(int8_t value);
void lcd_service_alarm_temperature(service_alarms_temperature_t index);
void lcd_service_alarm_temperature_temp_deviation_set(int8_t value);
void lcd_service_alarm_temperature_alarm_delay_set(uint8_t value);
void lcd_service_alarm_bat(service_alarms_bat_t index, uint8_t value);
void lcd_service_alarm_bat_set(uint8_t value);
void lcd_service_alarm_lid(service_alarms_lid_t index, uint8_t value);
void lcd_service_alarm_lid_set(uint8_t value);
void lcd_service_alarms_mute_duration(service_alarms_mute_duration_t index, uint8_t value);
void lcd_service_alarms_mute_duration_set(uint8_t value);
void lcd_service_alarms_warning(operation_mode_t mode, warning_type_t type);


#endif /* SRC_LCD_UI_H_ */
