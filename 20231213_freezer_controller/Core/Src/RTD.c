/*
 * RTD.c
 *
 *  Created on: Oct 26, 2023
 *      Author: Loc
 */


#include "RTD.h"
#include "main.h"
#include "adc.h"
#include "event.h"
#include "stm32l4xx_hal.h"
#include "quartic.h"
#include <string.h>

#define ADC_VREF_mV           3359       //Voltage adc in mV
#define RTD_MAX_CHANNEL       6

bool rtd_init_done = false;

#define RTD_ADC_USE_DMA  1

#ifndef RTD_ADC_USE_DMA
#define RTD5_ADC  	 {ADC_CHANNEL_1,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}
#define RTD6_ADC  	 {ADC_CHANNEL_2,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}
#define RTD1_ADC 	 {ADC_CHANNEL_6,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}
#define RTD2_ADC 	 {ADC_CHANNEL_7,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}
#define RTD3_ADC  	 {ADC_CHANNEL_13,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}
#define RTD4_ADC  	 {ADC_CHANNEL_14,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_640CYCLES_5}


ADC_ChannelConfTypeDef RTD_ADC_LIST[] = {RTD5_ADC,RTD6_ADC, RTD1_ADC, RTD2_ADC, RTD3_ADC, RTD4_ADC};
#endif
uint32_t adc_buff[RTD_MAX_CHANNEL];
uint32_t adc_total[RTD_MAX_CHANNEL] = {0};
uint32_t adc_voltage[RTD_MAX_CHANNEL] = {0};
uint32_t adc_average[RTD_MAX_CHANNEL] = {0};
//double temperature_result[RTD_MAX_CHANNEL];
double temperature_result[RTD_MAX_CHANNEL];
event_id rtd_id;
#define RES_OFFSET_CALIB     60
//2118  2852
//      4096
//0.00375 ±0.000029 ohm -> 1 kohm  0c
//

#define SAMPLE_MAX_COUNT    500
uint32_t sample_count = 0;






/*
Constant        1000 Ω              100 Ω
Alpha α (°C-1) 0.00375 ±0.000029    0.003850 ±0.000010
Delta δ (°C)   1.605 ±0.009         1.4999 ±0.007
Beta β  (°C)   0.16                 0.10863
A (°C-1)       3.81 x 10^-3         3.908 x 10-3
B (°C-2)      -6.02 x 10^-7         -5.775 x 10-7
C (°C-4)      -6.0 x 10^-12         -4.183 x 10-12

 * Functional Behavior
RT = R0(1 + A*T + B*T^2 - 100*C*T^3 + C * T^4)
Where:
RT = Resistance (Ω) at temperature T (°C)
R0 = Resistance (Ω) at 0 °C
T = Temperature (°C)
A = (α + αδ)/100    B = -α*δ/100^2      CT<0 = -α*β/100^4


double alpha =   0.00375;
double delta =   1.605;
double beta =    0.16;
double A     =   3.81/10000;
double B     =   -6.02/10000000;
double C     =   -6/1000000000000;
*/
double R0  =     1000; //resistance at 0 degree C


//in quartic


double a = 100;//
double b = 6.02 * 100000/6;//  B/C ;
double c = -3.81*1000000000/6;//A/C;
double d = 0;//(1 -  rtd1_res/R0)*1000000000000/6;




uint32_t adc_to_mV(uint32_t input)
{
	return input * ADC_VREF_mV/4095;
}

uint32_t mV_to_ohm(uint32_t input)
{
	return input/2;
}



#ifndef RTD_ADC_USE_DMA
uint32_t adc_read(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* cfg)
{
	 if (HAL_ADC_ConfigChannel(hadc, cfg) != HAL_OK)
	 {
	   Error_Handler();
	 }
	 HAL_ADC_Start(hadc);
     HAL_ADC_PollForConversion(hadc, 1);
     HAL_ADC_Stop(hadc);
     return HAL_ADC_GetValue(hadc);
}
#endif

void rtd_task(void)
{
#ifndef RTD_ADC_USE_DMA
	//Get ADC sample
	for(uint8_t sample = 0; sample < SAMPLE_MAX_COUNT; sample ++)
	{
		for(uint8_t i = 0; i < RTD_MAX_CHANNEL; i ++)
		{
			adc_total[i] = adc_read(&hadc1,&RTD_ADC_LIST[i]);
		}
	}

	for(uint8_t i = 0; i < RTD_MAX_CHANNEL; i ++)
	{
		adc_average[i] = adc_total[i]/SAMPLE_MAX_COUNT; //Get average
		adc_average[i] = adc_to_mV(adc_average[i]); //Convert to voltage
		adc_average[i] = mV_to_ohm(adc_average[i]) + RES_OFFSET_CALIB; //Convert to ohm
		d =  ((adc_average[i])/R0 - 1)*1000000000000/6;
		DComplex *result = solve_quartic(a,b,c,d);
		temperature_result[i] = creal(result[3]);
		adc_total[i] = 0;
	}
#else
	for(uint8_t i = 0; i < RTD_MAX_CHANNEL; i ++)
	{
		adc_average[i] /= SAMPLE_MAX_COUNT;
		adc_voltage[i] = adc_to_mV(adc_average[i]);
		adc_average[i] = mV_to_ohm(adc_voltage[i]) + RES_OFFSET_CALIB;

		d =  (adc_average[i]/R0 - 1)*1000000000000/6;
		DComplex *result = solve_quartic(a,b,c,d);

		temperature_result[i] = creal(result[3]);
	}
	rtd_init_done = true;
	event_inactive(&rtd_id);
#endif
}

double rtd_get_temperature(rtd_t rtd)
{
	return temperature_result[rtd];
}


uint32_t rtd_get_adc_voltage(rtd_t rtd)
{
	return adc_voltage[rtd];
}

bool is_rtd_started(void)
{
	return rtd_init_done;
}



void rtd_init(void)
{

	event_add(rtd_task, &rtd_id, 1);

#ifdef RTD_ADC_USE_DMA
	HAL_ADC_Start_DMA(&hadc1, adc_buff, RTD_MAX_CHANNEL);

#else
	event_active(&rtd_id);
#endif
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	for(uint8_t i = 0; i < RTD_MAX_CHANNEL; i++)
	{
		adc_total[i] += adc_buff[i];
	}

	sample_count++;
	if(sample_count >= SAMPLE_MAX_COUNT)
	{
		memcpy((uint8_t *)adc_average, (uint8_t *)adc_total,RTD_MAX_CHANNEL * 4);
		event_active(&rtd_id);
		memset((uint8_t *)adc_total, 0, RTD_MAX_CHANNEL * 4);
		sample_count = 0;
	}
}



