/*
 * tem_roll.c
 *
 *  Created on: Jan 30, 2024
 *      Author: Loc
 */




#include "tem_roll.h"

tem_roll_t tem_roll = {.sample_count = 1};

void tem_roll_put(double cur_tem)
{
	tem_roll.total += cur_tem;
	tem_roll.total -= tem_roll.last_sample;
	tem_roll.sample_count += 1;
	if(tem_roll.sample_count > MAX_SAMPLE_TEM)
	{
		tem_roll.last_sample = cur_tem;
	}
}


double tem_roll_get(void)
{
	if(tem_roll.sample_count <= MAX_SAMPLE_TEM)
	{
		return tem_roll.total/(tem_roll.sample_count - 1);
	}
	return tem_roll.total/MAX_SAMPLE_TEM;
}


uint8_t tem_roll_enough_data(void)
{
	return (tem_roll.sample_count > MAX_SAMPLE_TEM);
}
