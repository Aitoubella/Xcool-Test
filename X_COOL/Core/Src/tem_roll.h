/*
 * tem_roll.h
 *
 *  Created on: Jan 30, 2024
 *      Author: Loc
 */

#ifndef SRC_TEM_ROLL_H_
#define SRC_TEM_ROLL_H_
#include <stdint.h>
#include <math.h>


#define MAX_SAMPLE_TEM           5

typedef struct
{
	double total;
	double last_sample;
	uint32_t sample_count;
}tem_roll_t;



void tem_roll_put(double cur_tem);
double tem_roll_get(void);
uint8_t tem_roll_enough_data(void);

#endif /* SRC_TEM_ROLL_H_ */
