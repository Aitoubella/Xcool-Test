/*
 * RTD.h
 *
 *  Created on: Oct 26, 2023
 *      Author: Loc
 */

#ifndef SRC_RTD_H_
#define SRC_RTD_H_
#include <stdbool.h>
#include <stdint.h>

#define RTD_MAX_CHANNEL       6

typedef enum
{
	RTD5 = 0,
	RTD6,
	RTD1,
	RTD2,
	RTD3,
	RTD4,
}rtd_t;
void rtd_init(void);
double rtd_get_temperature(rtd_t rtd);
uint32_t rtd_get_adc_voltage(rtd_t rtd);
void rtd_add_filter(rtd_t channel);
bool is_rtd_started(void);
#endif /* SRC_RTD_H_ */
