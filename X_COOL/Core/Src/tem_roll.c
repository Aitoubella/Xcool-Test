/*
 * tem_roll.c
 *
 *  Created on: Jan 30, 2024
 *      Author: Loc
 */




#include "tem_roll.h"

tem_roll_t tem_roll[RTD_MAX_CHANNEL];

void tem_roll_put(rtd_t channel, double cur_tem)
{
	tem_roll[channel].sample[tem_roll[channel].index % MAX_SAMPLE_TEM] = cur_tem;
	tem_roll[channel].index ++;
	if(tem_roll[channel].sample_count < MAX_SAMPLE_TEM)
	tem_roll[channel].sample_count += 1;

}


double tem_roll_get(rtd_t channel)
{
	if(tem_roll[channel].sample_count == 0) return 0; //No temperature put yet->return 0
	//Get total value
	tem_roll[channel].total = 0;
	for(uint8_t i = 0; i < tem_roll[channel].sample_count; i++)
	{
		tem_roll[channel].total += tem_roll[channel].sample[i];
	}
	//Return average of total sample
	return tem_roll[channel].total/tem_roll[channel].sample_count;
}


uint8_t tem_roll_enough_data(rtd_t channel)
{
	return (tem_roll[channel].sample_count == MAX_SAMPLE_TEM);
}
