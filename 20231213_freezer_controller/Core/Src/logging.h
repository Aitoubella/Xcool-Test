#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_
#include "fatfs.h"
#include "lcd_interface.h"
#define LOG_FILE_NAME        "log.csv"
FRESULT logging_init(void);
FRESULT logging_write(char* file, lcd_inter_t* dat);

#endif /* SRC_LOGGING_H_ */
