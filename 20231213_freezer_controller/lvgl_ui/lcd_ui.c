/*
 * lcd_ui.c
 *
 *  Created on: Oct 18, 2023
 *      Author: Loc
 */


#include "lcd_ui.h"
#include "printf.h"
#include "event.h"
#include "lvgl.h"
#include "LCDController.h"
#include <stdbool.h>

#define WHITE_COLOR             0xFFFFFF
#define BLACK_COLOR             0x000000
#define RED_COLOR               0xFF0000
#define YELLOW_COLOR            0xFFFF00
#define COLOR_WARNING           0xFF8F00
#define MAX_LABEL              10
#define MAX_TEMP_CHAR          30
static char temp_buff[MAX_TEMP_CHAR] = {0};

lv_obj_t* screen;
lv_obj_t* label_list[MAX_LABEL];
lv_obj_t* btn_bat;
uint8_t label_count;

event_id lcd_tick_id;

lv_obj_t* get_label(void)
{
	label_count ++;
	if(label_count >= MAX_LABEL) label_count = 0;
	return label_list[label_count];
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
//{
//	lv_timer_handler();
//}
void lcd_lgvl_tick_handler(void)
{
	lv_timer_handler();
}

void lcd_ui_init(void)
{
	//Init tick for lvgl run


	//Init lvgl library
	lv_init();
	//Init brigde port use SPI
	lv_port_disp_init();

	screen = lv_obj_create(NULL);
	lv_obj_clear_flag( screen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	//Create label in to main
	btn_bat = lv_btn_create(screen);
	for(uint8_t i = 0; i < MAX_LABEL; i ++)
	{
		label_list[i] = lv_label_create(screen);
	}



	event_add(lcd_lgvl_tick_handler, &lcd_tick_id, 5);
	event_active(&lcd_tick_id);
}

void lcd_set_background_color(lv_obj_t* obj,uint32_t color)
{
	lv_obj_set_style_bg_color(obj, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT );
}

void lcd_ui_clear(void)
{
	lv_obj_add_flag(btn_bat,LV_OBJ_FLAG_HIDDEN);
	for(uint8_t i = 0; i < MAX_LABEL; i ++)
	{
		lv_obj_add_flag(label_list[i],LV_OBJ_FLAG_HIDDEN);
	}

	label_count = 0;
}
void lcd_ui_load_screen(void)
{
	lv_disp_load_scr(screen);
}

void lcd_ui_refresh(void)
{
	lv_refr_now(NULL);
	label_count = 0;
}

void lcd_hide_label(lv_obj_t*  obj)
{
	lv_obj_add_flag(obj,LV_OBJ_FLAG_HIDDEN);
}

void lcd_label_set_param(lv_obj_t* label, const char* txt, lv_coord_t x, lv_coord_t y, const lv_font_t * font, uint32_t color)
{
	lv_obj_set_width( label, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height( label, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x( label, x );
	lv_obj_set_y( label, y );
	lv_obj_set_align( label, LV_ALIGN_CENTER );
	lv_label_set_text(label, txt);
	lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(label, font, LV_PART_MAIN| LV_STATE_DEFAULT);
}

void lcd_main_screen_screen(speaker_mode_t sp_mode, int16_t temperature, power_mode_t pwr_mode, operation_mode_t op_mode, uint8_t bat_value, battery_state_t bat_st, battery_signal_t bat_signal)
{

	lcd_ui_t speaker_symbol = FONT_VERDENA_24;
	lcd_ui_t temper = FONT_VERDENA_24;
	lcd_ui_t operation_mode = FONT_VERDENA_40;
	lcd_ui_t power_mode = FONT_VERDENA_40;
	lcd_ui_t bat_state = FONT_VERDENA_36;
	lcd_ui_t bat_symbol = FONT_VERDENA_120;

	lcd_set_background_color(screen, BLACK_COLOR);

	if(sp_mode ==  SPEAKER_MODE_ON)
	{
		lcd_label_set_param(speaker_symbol.obj, LV_SYMBOL_VOLUME_MAX, -126, -89, speaker_symbol.font, WHITE_COLOR);
	}else
	{
		lcd_label_set_param(speaker_symbol.obj, LV_SYMBOL_MUTE, -126, -89, speaker_symbol.font, WHITE_COLOR);
	}

	snprintf(temp_buff,MAX_TEMP_CHAR, "%d°C",temperature);
	lcd_label_set_param(temper.obj, temp_buff, 103, -89, temper.font, WHITE_COLOR);

	if(op_mode == OPERATION_MODE_FREEZER)
	{
		lcd_label_set_param(operation_mode.obj, "Freezer", -2, -18, operation_mode.font, WHITE_COLOR);
	}else
	{
		lcd_label_set_param(operation_mode.obj, "Fridge", -2, -18, operation_mode.font, WHITE_COLOR);
	}

	if(pwr_mode == POWER_MODE_DC)
	{
		lcd_label_set_param(power_mode.obj, "DC", -104, 63,power_mode.font, WHITE_COLOR);
	}else if(pwr_mode == POWER_MODE_AC)
	{
		lcd_label_set_param(power_mode.obj, "AC", -104, 63,power_mode.font, WHITE_COLOR);
	}else
	{
		lcd_label_set_param(power_mode.obj, LV_SYMBOL_BATTERY_FULL, -104, 63, power_mode.font, WHITE_COLOR);
	}


	//Bat color
	lv_obj_set_width( btn_bat, 120);
	lv_obj_set_height( btn_bat, 70);
	lv_obj_set_x( btn_bat, 55 );
	lv_obj_set_y( btn_bat, 62 );
	lv_obj_set_align( btn_bat, LV_ALIGN_CENTER );
	lv_obj_add_flag( btn_bat, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
	lv_obj_clear_flag( btn_bat, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_radius(btn_bat, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
    if(bat_signal == BATTERY_NORMAL)
    {
    	lv_obj_set_style_bg_color(btn_bat, lv_color_hex(BLACK_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT );
    }else if(bat_signal == BATTER_WARNING_LOW)
    {
    	lv_obj_set_style_bg_color(btn_bat, lv_color_hex(COLOR_WARNING), LV_PART_MAIN | LV_STATE_DEFAULT );
    }else if(bat_signal == BATTERY_OUT_OF_BAT)
    {
    	lv_obj_set_style_bg_color(btn_bat, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
    }

	lv_obj_set_style_bg_opa(btn_bat, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_clear_flag(btn_bat, LV_OBJ_FLAG_HIDDEN);


	lcd_label_set_param(bat_symbol.obj,  LV_SYMBOL_BATTERY_EMPTY, 60, 62, bat_symbol.font, WHITE_COLOR);
	lcd_set_background_color(bat_symbol.obj, YELLOW_COLOR);
	if(bat_st == BATTERY_STATE_CHARGING)
	{
		snprintf(temp_buff, MAX_TEMP_CHAR, "%d%s",bat_value,LV_SYMBOL_CHARGE);

	}else
	{
		snprintf(temp_buff, MAX_TEMP_CHAR, "%d%%",bat_value);
	}
	lcd_label_set_param(bat_state.obj,temp_buff , 58, 62, bat_state.font, WHITE_COLOR);
	lcd_set_background_color(bat_symbol.obj, YELLOW_COLOR);
}

void lcd_operation_mode_screen(operation_mode_t index)
{
	lcd_ui_t operation_mode = FONT_VERDENA_36;
	lcd_ui_t fridge = FONT_VERDENA_24;
	lcd_ui_t freezer = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	lcd_set_background_color(screen,BLACK_COLOR);

	if(index == OPERATION_MODE_FREEZER)
	{
		freezer.font = &ui_font_verdana364;
	}else if(index == OPERATION_MODE_FRIDEGE)
	{
		fridge.font = &ui_font_verdana364;
	}else if(index == OPERATION_MODE_BACK)
	{
		back.font = &ui_font_verdana364;
	}

	lcd_label_set_param(operation_mode.obj, "Operation Mode", 0, -96, operation_mode.font, WHITE_COLOR);
	lcd_label_set_param(fridge.obj, "Fridge", 0, -28, fridge.font, WHITE_COLOR);
	lcd_label_set_param(freezer.obj, "Freezer", 0, 10, freezer.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 51, back.font, WHITE_COLOR);

}

void lcd_turn_off_unit(display_unit_t value)
{
	lv_obj_t* turn_unit_obj = get_label();
	lcd_label_set_param(turn_unit_obj, "Turn off unit?", 0, -96, &ui_font_verdana364, WHITE_COLOR);
	lv_obj_t* yes_obj = get_label();
	lv_obj_t* no_obj = get_label();
	lcd_set_background_color(screen,BLACK_COLOR);
	if(value == DISPLAY_UINIT_YES)
	{
		lcd_label_set_param(yes_obj, "YES", 0, -11, &ui_font_verdana404, WHITE_COLOR);
		lcd_label_set_param(no_obj, "NO", 0, 28, &ui_font_verdana244, WHITE_COLOR);
	}else
	{
		lcd_label_set_param(yes_obj, "YES", 0, -11, &ui_font_verdana244, WHITE_COLOR);
		lcd_label_set_param(no_obj, "NO", 0, 28, &ui_font_verdana404, WHITE_COLOR);
	}

}

void lcd_setting(setting_t st)
{
	lcd_ui_t setting = FONT_VERDENA_36;
	lcd_ui_t datetime = FONT_VERDENA_24;
	lcd_ui_t download_data = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	lcd_set_background_color(screen,BLACK_COLOR);
	if(st == SETTING_DATETIME)
	{
		datetime.font = &ui_font_verdana404;
	}else if(st == SETTING_DOWNLOAD_DATA)
	{
		download_data.font = &ui_font_verdana404;
	}else if(st == SETTING_BACK)
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(setting.obj, "Setting", 0, -96, setting.font, WHITE_COLOR);
	lcd_label_set_param(datetime.obj, "Date/Time", 0, -34, datetime.font, WHITE_COLOR);
	lcd_label_set_param(download_data.obj, "Download Data", 0, 0, download_data.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 52, back.font, WHITE_COLOR);
}



void lcd_setting_datetime(setting_datetime_t index, datetime_t* time)
{
	lv_obj_t* year_obj = get_label();
	lv_obj_t* month_obj = get_label();
	lv_obj_t* day_obj = get_label();
	lv_obj_t* hour_obj = get_label();
	lv_obj_t* minute_obj = get_label();
	lv_obj_t* back_obj = get_label();
	const lv_font_t* year_font = &ui_font_verdana244;
	const lv_font_t* month_font = &ui_font_verdana244;
	const lv_font_t* day_font = &ui_font_verdana244;
	const lv_font_t* hour_font = &ui_font_verdana244;
	const lv_font_t* minute_font = &ui_font_verdana244;
	const lv_font_t* back_font = &ui_font_verdana244;


	if(index == SETTING_DATETIME_YEAR)
	{
		year_font = &ui_font_verdana404;
	}else if(index == SETTING_DATETIME_MONTH)
	{
		month_font = &ui_font_verdana404;
	}else if(index == SETTING_DATETIME_DAY)
	{
		day_font = &ui_font_verdana404;
	}else if(index == SETTING_DATETIME_HOUR)
	{
		hour_font = &ui_font_verdana404;
	}else if(index == SETTING_DATETIME_MINUTE)
	{
		minute_font = &ui_font_verdana404;
	}else if(index == SETTING_DATETIME_BACK)
	{
		back_font = &ui_font_verdana404;
	}


	snprintf(temp_buff,MAX_TEMP_CHAR,"Set Year: %2d", time->year);
	lcd_label_set_param(year_obj, temp_buff, 0, -85, year_font, WHITE_COLOR);
	snprintf(temp_buff,MAX_TEMP_CHAR,"Set Month: %2d", time->month);
	lcd_label_set_param(month_obj, temp_buff, 0, -46, month_font, WHITE_COLOR);
	snprintf(temp_buff,MAX_TEMP_CHAR,"Set Day: %2d", time->day);
	lcd_label_set_param(day_obj, temp_buff, 0, -13, day_font, WHITE_COLOR);
	snprintf(temp_buff,MAX_TEMP_CHAR,"Set Hour: %2d", time->hour);
	lcd_label_set_param(hour_obj, temp_buff, 0, 19, hour_font, WHITE_COLOR);
	snprintf(temp_buff,MAX_TEMP_CHAR,"Set Minute: %2d", time->minute);
	lcd_label_set_param(minute_obj, temp_buff, 0, 52, minute_font, WHITE_COLOR);
	lcd_label_set_param(back_obj, "Back", 0, 84, back_font, WHITE_COLOR);
}


void lcd_setting_datetime_year_set(uint16_t year)
{
	lcd_ui_t set_year = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(set_year.obj, "Set Year", 0, -78, set_year.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",year + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, -10, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",year);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 30, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",year - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 68, time_3.font, WHITE_COLOR);

}

void lcd_setting_datetime_month_set(uint8_t month)
{
	lcd_ui_t set_month = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(set_month.obj, "Set Month", 0, -78, set_month.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",month + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, -10, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",month);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 30, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",month - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 68, time_3.font, WHITE_COLOR);
	if(month == 12)
	{
		lcd_hide_label(time_1.obj);
	}
	if(month == 1)
	{
		lcd_hide_label(time_3.obj);
	}
}

void lcd_setting_datetime_day_set(uint8_t day)
{
	lcd_ui_t set_day = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(set_day.obj, "Set Day", 0, -78, set_day.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",day + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, -10, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",day);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 30, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",day - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 68, time_3.font, WHITE_COLOR);
	if(day == 31)
	{
		lcd_hide_label(time_1.obj);
	}
	if(day == 1)
	{
		lcd_hide_label(time_3.obj);
	}
}

void lcd_setting_datetime_hour_set(uint8_t hour)
{
	lcd_ui_t set_hour = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(set_hour.obj, "Set Hour", 0, -78, set_hour.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",hour + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, -10, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",hour);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 30, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",hour - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 68, time_3.font, WHITE_COLOR);
	if(hour == 23)
	{
		lcd_hide_label(time_1.obj);
	}
	if(hour == 0)
	{
		lcd_hide_label(time_3.obj);
	}
}

void lcd_setting_datetime_min_set(uint8_t min)
{
	lcd_ui_t set_min = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(set_min.obj, "Set Minute", 0, -78, set_min.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",min + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, -10, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",min);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 30, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d",min - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 68, time_3.font, WHITE_COLOR);

	if(min == 59)
	{
		lcd_hide_label(time_1.obj);
	}
	if(min == 0)
	{
		lcd_hide_label(time_3.obj);
	}

}


void lcd_setting_download_data(setting_download_data_t index)
{
	lcd_ui_t download_data = FONT_VERDENA_36;
	lcd_ui_t download_data_to_usb = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SETTING_DOWNLOAD_DATA_TO_USB)
	{
		download_data_to_usb.font = &ui_font_verdana364;
	}else
	{
		back.font = &ui_font_verdana364;
	}

	lcd_label_set_param(download_data.obj, "Download Data", 0, -96, download_data.font, WHITE_COLOR);
	lcd_label_set_param(download_data_to_usb.obj, "Download to USB", 0, 0, download_data_to_usb.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 47, back.font, WHITE_COLOR);
}

void lcd_setting_download_data_insert(setting_download_data_insert_t index)
{
	lcd_ui_t insert = FONT_VERDENA_36;
	lcd_ui_t continue_usb = FONT_VERDENA_24;
	lcd_ui_t cancel = FONT_VERDENA_24;

	if(index == SETTING_DOWNLOAD_DATA_CONTINUE)
	{
		continue_usb.font = &ui_font_verdana404;
	}else
	{
		cancel.font = &ui_font_verdana404;
	}

	lcd_label_set_param(insert.obj, "Insert USB drive\nto download CSV", 0, -70, insert.font, WHITE_COLOR);
	lcd_label_set_param(continue_usb.obj, "Continue", 0, 22, continue_usb.font, WHITE_COLOR);
	lcd_label_set_param(cancel.obj, "Cancel", 0, 63, cancel.font, WHITE_COLOR);
}

void lcd_setting_download_data_complete(void)
{
	lcd_ui_t complete = FONT_VERDENA_36;
	lcd_ui_t safe_remove = FONT_VERDENA_36;
	lcd_ui_t continue_label = FONT_VERDENA_24;

	lcd_label_set_param(complete.obj, "Download\ncomplete", 0, -70, complete.font, WHITE_COLOR);
	lcd_label_set_param(safe_remove.obj, "Safe to remove\nUSB Device", 0, 22, safe_remove.font, WHITE_COLOR);
	lcd_label_set_param(continue_label.obj, "Continue", 0, 81, continue_label.font, WHITE_COLOR);
}

void lcd_service(service_t index)
{
	lcd_ui_t service = FONT_VERDENA_36;
	lcd_ui_t temperature = FONT_VERDENA_24;
	lcd_ui_t alarm = FONT_VERDENA_24;
	lcd_ui_t data_logging = FONT_VERDENA_24;
	lcd_ui_t calibration = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	lcd_set_background_color(screen,BLACK_COLOR);

	if(index == SERVICE_TEMPERATURE)
	{
		temperature.font = &ui_font_verdana404;
	}else if(index == SERVICE_ALARM)
	{
		alarm.font = &ui_font_verdana404;
	}else if(index == SERVICE_DATA_LOGGING)
	{
		data_logging.font = &ui_font_verdana404;
	}else if(index == SERVICE_CALIBRATION)
	{
		calibration.font = &ui_font_verdana404;
	}
	else if(index == SERVICE_BACK)
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(service.obj, "Service", 0, -96, service.font, WHITE_COLOR);
	lcd_label_set_param(temperature.obj, "Temperature", 0, -49, temperature.font, WHITE_COLOR);
	lcd_label_set_param(alarm.obj, "Alarms", 0, -17, alarm.font, WHITE_COLOR);
	lcd_label_set_param(data_logging.obj, "Data Logging", 0, 13, data_logging.font, WHITE_COLOR);
	lcd_label_set_param(calibration.obj, "Calibration", 0, 52, calibration.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 88, back.font, WHITE_COLOR);
}

void lcd_service_temperature(service_temperature_t index)
{
	lcd_ui_t temperature = FONT_VERDENA_36;
	lcd_ui_t fridge = FONT_VERDENA_24;
	lcd_ui_t freezer = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_TEMPERATURE_FRIDGE)
	{
		fridge.font = &ui_font_verdana404;
	}else if(index == SERVICE_TEMPERATURE_FREEZER)
	{
		freezer.font = &ui_font_verdana404;
	}else if(index == SERVICE_TEMPERATURE_BACK)
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(temperature.obj, "Temperature", 0, -96, temperature.font, WHITE_COLOR);
	lcd_label_set_param(fridge.obj, "Fridge", 0, -29, fridge.font, WHITE_COLOR);
	lcd_label_set_param(freezer.obj, "Freezer", 0, 10, freezer.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 51, back.font, WHITE_COLOR);
}

void lcd_service_alarms(service_alarm_t index)
{
	lcd_ui_t alarm = FONT_VERDENA_36;
	lcd_ui_t temperature = FONT_VERDENA_24;
	lcd_ui_t bat = FONT_VERDENA_24;
	lcd_ui_t lid = FONT_VERDENA_24;
	lcd_ui_t mute = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_ALARMS_TEMPERATURE)
	{
		temperature.font = &ui_font_verdana404;
	}else if(index == SERVICE_ALARMS_BATTERY)
	{
		bat.font = &ui_font_verdana404;
	}else if(index == SERVICE_ALARMS_LID)
	{
		lid.font = &ui_font_verdana404;
	}else if(index == SERVICE_ALARMS_MUTE_AlARMS)
	{
		mute.font = &ui_font_verdana404;
	}else
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(alarm.obj, "Alarms", 0, -96, alarm.font, WHITE_COLOR);
	lcd_label_set_param(temperature.obj, "Temperature", 0, -54, temperature.font, WHITE_COLOR);
	lcd_label_set_param(bat.obj, "Battery", 0, -15, bat.font, WHITE_COLOR);
	lcd_label_set_param(lid.obj, "Lid", 0, 23, lid.font, WHITE_COLOR);
	lcd_label_set_param(mute.obj, "Alarms Duration", 0, 61, mute.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 96, back.font, WHITE_COLOR);
}

void lcd_service_data_logging(service_data_logging_t index)
{
	lcd_ui_t data_logging = FONT_VERDENA_36;
	lcd_ui_t logging_interval = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;


	if(index == SERVICE_DATA_LOGGING_INTERVAL)
	{
		logging_interval.font = &ui_font_verdana364;
	}else if(index == SERVICE_DATA_LOGGING_BACK)
	{
		back.font = &ui_font_verdana364;
	}

	lcd_label_set_param(data_logging.obj, "Data Logging", 0, -96, data_logging.font, WHITE_COLOR);
	lcd_label_set_param(logging_interval.obj, "Logging Interval", 0, 0, logging_interval.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 38, back.font, WHITE_COLOR);
}

void lcd_service_data_logging_set(uint8_t value)
{
	lcd_ui_t logging_interval = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;

	lcd_label_set_param(logging_interval.obj, "Logging Interval", 0, -70, logging_interval.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, 0, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 37, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 74, time_3.font, WHITE_COLOR);
	if(value == 1)
	{
		lv_obj_add_flag(time_3.obj,LV_OBJ_FLAG_HIDDEN);
	}
}

void lcd_service_calibration(service_calibration_t index)
{
	lcd_ui_t calibration = FONT_VERDENA_36;
	lcd_ui_t temp_offset = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;


	if(index == SERVICE_CALIBRATION_TEMP_OFFSET)
	{
		temp_offset.font = &ui_font_verdana404;
	}else if(index == SERVICE_CALIBRATION_BACK)
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(calibration.obj, "Calibration", 0, -96, calibration.font, WHITE_COLOR);
	lcd_label_set_param(temp_offset.obj, "Temp Offset", 0, 0, temp_offset.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 38, back.font, WHITE_COLOR);
}

void lcd_service_calibration_set(int8_t value)
{
	lcd_ui_t temp_offset = FONT_VERDENA_36;
	lcd_ui_t temper_1 = FONT_VERDENA_24;
	lcd_ui_t temper_2 = FONT_VERDENA_40;
	lcd_ui_t temper_3 = FONT_VERDENA_24;
	lcd_label_set_param(temp_offset.obj, "Temp Offset", 0, -70, temp_offset.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value + 1);
	lcd_label_set_param(temper_1.obj, temp_buff, 0, 0, temper_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper_2.obj, temp_buff, 0, 37, temper_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value - 1);
	lcd_label_set_param(temper_3.obj, temp_buff, 0, 74, temper_3.font, WHITE_COLOR);
}

void lcd_service_temperature_fridge(service_temperature_fridge_t index, int8_t value)
{
	lcd_ui_t fridge = FONT_VERDENA_36;
	lcd_ui_t set_point = FONT_VERDENA_36;
	lcd_ui_t temper = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_TEMPERATURE_FRIDGE_VALUE)
	{
		temper.font = &ui_font_verdana364;
	}else
	{
		back.font = &ui_font_verdana364;
	}

	lcd_label_set_param(fridge.obj, "Fridge", 0, -96, fridge.font, WHITE_COLOR);
	lcd_label_set_param(set_point.obj, "Set Point", 0, -48, set_point.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper.obj,temp_buff, 0, 8, temper.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 51, back.font, WHITE_COLOR);
}

void lcd_service_temperature_freezer(service_temperature_freezer_t index, int8_t value)
{
	lcd_ui_t freezer = FONT_VERDENA_36;
	lcd_ui_t set_point = FONT_VERDENA_36;
	lcd_ui_t temper = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_TEMPERATURE_FREEZER_VALUE)
	{
		temper.font = &ui_font_verdana364;
	}else
	{
		back.font = &ui_font_verdana364;
	}

	lcd_label_set_param(freezer.obj, "Freezer", 0, -96, freezer.font, WHITE_COLOR);
	lcd_label_set_param(set_point.obj, "Set Point", 0, -48, set_point.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper.obj, temp_buff, 0, 8, temper.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 51, back.font, WHITE_COLOR);
}

void lcd_service_temperature_fridge_set(int8_t value)
{
	lcd_ui_t mode = FONT_VERDENA_36;
	lcd_ui_t set_point = FONT_VERDENA_36;
	lcd_ui_t temper_1 = FONT_VERDENA_24;
	lcd_ui_t temper_2 = FONT_VERDENA_40;
	lcd_ui_t temper_3 = FONT_VERDENA_24;


	lcd_label_set_param(mode.obj, "Fridge", 0, -96, mode.font, WHITE_COLOR);

	lcd_label_set_param(set_point.obj, "Set Point", 0, -48, set_point.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value + 1);
	lcd_label_set_param(temper_1.obj, temp_buff, 0, 0, temper_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper_2.obj, temp_buff, 0, 37, temper_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value - 1);
	lcd_label_set_param(temper_3.obj, temp_buff, 0, 74, temper_3.font, WHITE_COLOR);
}

void lcd_service_temperature_freezer_set(int8_t value)
{
	lcd_ui_t mode = FONT_VERDENA_36;
	lcd_ui_t set_point = FONT_VERDENA_36;
	lcd_ui_t temper_1 = FONT_VERDENA_24;
	lcd_ui_t temper_2 = FONT_VERDENA_40;
	lcd_ui_t temper_3 = FONT_VERDENA_24;


	lcd_label_set_param(mode.obj, "Freezer", 0, -96, mode.font, WHITE_COLOR);

	lcd_label_set_param(set_point.obj, "Set Point", 0, -48, set_point.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value + 1);
	lcd_label_set_param(temper_1.obj, temp_buff, 0, 0, temper_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper_2.obj, temp_buff, 0, 37, temper_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value - 1);
	lcd_label_set_param(temper_3.obj, temp_buff, 0, 74, temper_3.font, WHITE_COLOR);
}

void lcd_service_alarm_temperature(service_alarms_temperature_t index)
{
	lcd_ui_t temp_deviation = FONT_VERDENA_24;
	lcd_ui_t alarm_delay = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;
	if(index == SERVICE_ALARM_TEMP_TEMP_DEVIATION)
	{
		temp_deviation.font = &ui_font_verdana404;
	}else if(index == SERVICE_ALARM_TEMP_ALARM_DELAY)
	{
		alarm_delay.font = &ui_font_verdana404;
	}else
	{
		back.font = &ui_font_verdana404;
	}

	lcd_label_set_param(temp_deviation.obj, "Temp Deviation", 0, -60, temp_deviation.font, WHITE_COLOR);
	lcd_label_set_param(alarm_delay.obj, "Alarm Delay", 0, 0, alarm_delay.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 60, back.font, WHITE_COLOR);
}

void lcd_service_alarm_temperature_temp_deviation_set(int8_t value)
{
	lcd_ui_t temp_deviation = FONT_VERDENA_36;

	lcd_ui_t temper_1 = FONT_VERDENA_24;
	lcd_ui_t temper_2 = FONT_VERDENA_40;
	lcd_ui_t temper_3 = FONT_VERDENA_24;
	lcd_label_set_param(temp_deviation.obj, "Temp Deviation", 0, -70, temp_deviation.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value + 1);
	lcd_label_set_param(temper_1.obj, temp_buff, 0, 0, temper_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value);
	lcd_label_set_param(temper_2.obj, temp_buff, 0, 37, temper_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d°C",value - 1);
	lcd_label_set_param(temper_3.obj, temp_buff, 0, 74, temper_3.font, WHITE_COLOR);
}

void lcd_service_alarm_temperature_alarm_delay_set(uint8_t value)
{
	lcd_ui_t temp_deviation = FONT_VERDENA_36;

	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(temp_deviation.obj, "Alarm Delay", 0, -70, temp_deviation.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, 0, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 37, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 74, time_3.font, WHITE_COLOR);

	if(value == 0)
	{
		lcd_hide_label(time_3.obj);
	}
}

void lcd_service_alarm_bat(service_alarms_bat_t index, uint8_t value)
{
	lcd_ui_t alarm_bat = FONT_VERDENA_36;
	lcd_ui_t temp_value = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_ALARM_BAT_VALUE)
	{
		temp_value.font = &ui_font_verdana404;
	}else
	{
		back.font = &ui_font_verdana404;
	}
	lcd_label_set_param(alarm_bat.obj, "Battery Alarm", 0, -78, alarm_bat.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d%%",value);
	lcd_label_set_param(temp_value.obj, temp_buff, 0, 0, temp_value.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 44, back.font, WHITE_COLOR);
}

void lcd_service_alarm_bat_set(uint8_t value)
{
	lcd_ui_t bat_alarm = FONT_VERDENA_36;

	lcd_ui_t percent_1 = FONT_VERDENA_24;
	lcd_ui_t percent_2 = FONT_VERDENA_40;
	lcd_ui_t percent_3 = FONT_VERDENA_24;
	lcd_label_set_param(bat_alarm.obj, "Battery Alarm", 0, -70, bat_alarm.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d %%",value + 1);
	lcd_label_set_param(percent_1.obj, temp_buff, 0, 0, percent_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d %%",value);
	lcd_label_set_param(percent_2.obj, temp_buff, 0, 37, percent_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d %%",value - 1);
	lcd_label_set_param(percent_3.obj, temp_buff, 0, 74, percent_3.font, WHITE_COLOR);
	if(value == 1)
	{
		lcd_hide_label(percent_3.obj);
	}
	if(value >= 99)
	{
		lcd_hide_label(percent_1.obj);
	}
}

void lcd_service_alarm_lid(service_alarms_lid_t index, uint8_t value)
{
	lcd_ui_t alarm_lid = FONT_VERDENA_36;
	lcd_ui_t time = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_ALARM_LID_VALUE)
	{
		time.font = &ui_font_verdana404;
	}else
	{
		back.font = &ui_font_verdana404;
	}
	lcd_label_set_param(alarm_lid.obj, "Lid Alarm", 0, -78, alarm_lid.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time.obj, temp_buff, 0, 0, time.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 44, back.font, WHITE_COLOR);
}

void lcd_service_alarm_lid_set(uint8_t value)
{
	lcd_ui_t lid_alarm = FONT_VERDENA_36;

	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(lid_alarm.obj, "Lid Alarm", 0, -70, lid_alarm.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, 0, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 37, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 74, time_3.font, WHITE_COLOR);
	if(value == 0)
	{
		lcd_hide_label(time_3.obj);
	}
}


void lcd_service_alarms_mute_duration(service_alarms_mute_duration_t index, uint8_t value)
{
	lcd_ui_t mute_duration = FONT_VERDENA_36;
	lcd_ui_t time = FONT_VERDENA_24;
	lcd_ui_t back = FONT_VERDENA_24;

	if(index == SERVICE_ALARM_MUTE_DURATION_VALUE)
	{
		time.font = &ui_font_verdana404;
	}else
	{
		back.font = &ui_font_verdana404;
	}
	lcd_label_set_param(mute_duration.obj, "Mute Duration", 0, -78, mute_duration.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time.obj, temp_buff, 0, 0, time.font, WHITE_COLOR);
	lcd_label_set_param(back.obj, "Back", 0, 44, back.font, WHITE_COLOR);
}

void lcd_service_alarms_mute_duration_set(uint8_t value)
{
	lcd_ui_t mute_duration = FONT_VERDENA_36;
	lcd_ui_t time_1 = FONT_VERDENA_24;
	lcd_ui_t time_2 = FONT_VERDENA_40;
	lcd_ui_t time_3 = FONT_VERDENA_24;
	lcd_label_set_param(mute_duration.obj, "Mute Duration", 0, -70, mute_duration.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value + 1);
	lcd_label_set_param(time_1.obj, temp_buff, 0, 0, time_1.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value);
	lcd_label_set_param(time_2.obj, temp_buff, 0, 37, time_2.font, WHITE_COLOR);
	snprintf(temp_buff, MAX_TEMP_CHAR, "%d mins",value - 1);
	lcd_label_set_param(time_3.obj, temp_buff, 0, 74, time_3.font, WHITE_COLOR);
	if(value == 0)
	{
		lcd_hide_label(time_3.obj);
	}
}


void lcd_service_alarms_warning(operation_mode_t mode, warning_type_t type)
{

	lcd_ui_t mode_obj = FONT_VERDENA_40;
	lcd_ui_t type_obj = FONT_VERDENA_36;
	lcd_ui_t warning_symbol = FONT_VERDENA_40;
	lcd_set_background_color(screen,RED_COLOR);
	if(mode == OPERATION_MODE_FRIDEGE)
	{
		lcd_label_set_param(mode_obj.obj, "Fridge", 0, -70, mode_obj.font, WHITE_COLOR);
	}else
	{
		lcd_label_set_param(mode_obj.obj, "Freezer", 0, -70, mode_obj.font, WHITE_COLOR);
	}

	if(type == WARNING_TYPE_UNDER_MIN_TEMP)
	{
		lcd_label_set_param(type_obj.obj, "Under Min\nTemp", 40, 45, type_obj.font, WHITE_COLOR);
	}else if(type == WARNING_TYPE_OVER_MAX_TEMP)
	{
		lcd_label_set_param(type_obj.obj, "Over Max\nTemp", 40, 45, type_obj.font, WHITE_COLOR);
	}else if(type == WARNING_TYPE_LID_OPEN)
	{
		lcd_label_set_param(type_obj.obj, "Lid Open", 40, 45, type_obj.font, WHITE_COLOR);
	}

	lcd_label_set_param(warning_symbol.obj, LV_SYMBOL_WARNING, -108, 32, warning_symbol.font, WHITE_COLOR);
}


