/*
 * tem_roll.h
 *
 *  Created on: Jan 30, 2024
 *      Author: Loc
 */

#ifndef SRC_TEM_ROLL_H_
#define SRC_TEM_ROLL_H_
#include <stdint.h>
#include "RTD.h"

#define MAX_SAMPLE_TEM           5

typedef struct
{
	double total;
	double sample[MAX_SAMPLE_TEM];
	uint32_t index;
	uint32_t sample_count;
}tem_roll_t;



void tem_roll_put(rtd_t channel, double cur_tem);
double tem_roll_get(rtd_t channel);
uint8_t tem_roll_enough_data(rtd_t channel);

#endif /* SRC_TEM_ROLL_H_ */
