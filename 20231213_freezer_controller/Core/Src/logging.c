#include "logging.h"
#include "fatfs.h"
#include <string.h>
#include "printf.h"
#include "freeRTOS.h"
const char* op_mode_str[] = { [OPERATION_MODE_FRIDEGE] = "FRIDGE", [OPERATION_MODE_FREEZER] = "FREEZER" };
const char* power_mode_str[] = {[POWER_MODE_DC] = "DC", [POWER_MODE_AC] = "AC"};
const char* speaker_mode_str[] = {[SPEAKER_MODE_OFF] = "MUTE", [POWER_MODE_AC] = "ON"};
const char* bat_state_str[] = {[BATTERY_STATE_CHARGING] = "CHARGING", [POWER_MODE_AC] = "NOT CHARGE"};
const char* warning_type_str[] = {[WARNING_TYPE_NONE] = "NO WARNING",[WARNING_TYPE_UNDER_MIN_TEMP] = "WARNING UNDER MIN TEMP", [WARNING_TYPE_OVER_MAX_TEMP] = "WARNING OVER MAX TEMP", [WARNING_TYPE_LID_OPEN] = "WARNING LID OPEN"};
const char* lid_state_str[] = {[LID_CLOSE] = "LID CLOSE", [LID_OPEN] = "LID OPEN"};



//some variables for FatFs
extern osMutexId fileMutexHandle;
extern FATFS USERFatFS;    /* File system object for USER logical drive */
extern char USERPath[4];
FIL fil; 		//File handle
extern uint8_t usb_buffer[MAX_USB_BUFFER];
FRESULT logging_init(void)
{
	FRESULT fres = FR_OK; //Result after operations
	//Open the file system
	fres = f_mount(&USERFatFS, USERPath, 1); //1=mount now
	if (fres != FR_OK)
	{
		printf("\nf_mount error (%i)\r\n", fres);
		return fres;
	}

	//Let's get some statistics from the SD card
	DWORD free_clusters, free_sectors, total_sectors;

	FATFS* getFreeFs;

	fres = f_getfree(USERPath, &free_clusters, &getFreeFs);
	if (fres != FR_OK)
	{
		printf("\nf_getfree error (%i)", fres);
		return fres;
	}

	//Formula comes from ChaN's documentation
	total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
	free_sectors = free_clusters * getFreeFs->csize;
	printf("\nSD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

	return fres;
}

FRESULT logging_write(char* file, lcd_inter_t* dat)
{
	FRESULT fres = FR_OK; //Result after operations

	fres = f_open(&fil, file, FA_WRITE | FA_OPEN_APPEND);
	if(fres == FR_OK)
	{
		printf("\r\nOpen %s for writing",file);
	} else
	{
		printf("\r\nf_open error (%i)", fres);
		return fres;
	}
	uint8_t len;

	//Date and timeOperation mode, temperature, bat value , bat state, power mode, lid state, warning,
	//Copy in a string
	len = sprintf((char*)usb_buffer, ",%d/%d/%d %d:%d:%d, %d,%d,%s,%s,%s,%s\r\n",
			                        dat->datetime.year, dat->datetime.month, dat->datetime.day, dat->datetime.hour, dat->datetime.minute, dat->datetime.second,
									dat->temperature, dat->bat_value, bat_state_str[dat->bat_state], power_mode_str[dat->pwr_mode], lid_state_str[dat->lid_state], warning_type_str[dat->warning_type]);
	UINT bytesWrote;
	fres = f_write(&fil, usb_buffer, len, &bytesWrote);
	if(fres == FR_OK)
	{
		printf("\r\nWrote %i bytes to %s!", bytesWrote, file);
	} else
	{
		printf("\r\nf_write error (%i)", fres);
	}

	//Be a tidy kiwi - don't forget to close your file!
	f_close(&fil);
	return fres;
}
