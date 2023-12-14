/*
 * BQ25731.c
 *
 *  Created on: Nov 13, 2023
 *      Author: Loc
 */


#include "BQ25731.h"
#include "i2c.h"
#include "main.h"
#include <string.h>
#define BQ25731_I2C     hi2c1
#define BQ25731_ADDR    0x6B


#define BIT_TO_DEC(x,y) ((x##y) << y)




HAL_StatusTypeDef bq25731_read_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Master_Transmit(&BQ25731_I2C, (BQ25731_ADDR << 1) | 0x01, &reg, 1, 100); //Send write operation to move to reg
	if(status != HAL_OK) return status;
	return HAL_I2C_Master_Receive(&BQ25731_I2C, (BQ25731_ADDR << 1), data, len, 100); //Read 1 data from current reg
}
HAL_StatusTypeDef bq25731_write_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
#define MAX_LEN_WRITE            5
	if(len >= 5) return HAL_ERROR;
	uint8_t data_temp[MAX_LEN_WRITE] = {reg};
	memcpy(&data_temp[1], data, len);
	return HAL_I2C_Master_Transmit(&BQ25731_I2C, (BQ25731_ADDR << 1), data_temp, len + 1, 100);
}

HAL_StatusTypeDef bq25731_set_bit_reg(uint8_t reg, uint8_t lsb, uint8_t msb)
{
	uint8_t data[2] = {0};
	HAL_StatusTypeDef status = bq25731_read_reg(reg, data, 2);
	if(status != HAL_OK) return status;
	data[0] |= lsb;
	data[1] |= msb;
	return bq25731_write_reg(reg, data, 2);
}
HAL_StatusTypeDef bq25731_clear_bit_reg(uint8_t reg, uint8_t lsb, uint8_t msb)
{
	uint8_t data[2] = {0};
	HAL_StatusTypeDef status = bq25731_read_reg(reg, data, 2);
	if(status != HAL_OK) return status;
	data[0] &= ~lsb;
	data[1] &= ~msb;
	return bq25731_write_reg(reg, data, 2);
}



HAL_StatusTypeDef bq25731_get_device_id(uint8_t* dev_id)
{
	return bq25731_read_reg(DEVICE_ID_REG, dev_id, 1);
}

HAL_StatusTypeDef bq25731_get_manufacture_id(uint8_t* man_id)
{
	return bq25731_read_reg(MANUFATURER_ID_REG, man_id, 1);
}

HAL_StatusTypeDef bq25731_set_charge_voltage(uint16_t mv)
{
	uint8_t data[2] = {0};
	mv /= 8; //Each bit is 8mV

	data[0] = (mv & 0b11111) << 3; //Get bit 0->4  and fit bit 3->7 reg
	data[1] = mv >> 5; //Get bit 5->11 fit 0->6 reg


	return bq25731_write_reg(CHARGE_VOLTAGE_REG, data, 2);
}


HAL_StatusTypeDef bq25731_set_charge_current(uint16_t mA)
{
	mA /= 128;
	uint8_t data[2] = {0};
	data[0] = (mA & 0b11) << 6; //Get bit 0,1  to fit bit 6,7
	data[1] = mA >> 2; //Get bit 2->6 fit bit 0->5
	return bq25731_write_reg(CHARGE_CURRENT_REG, data, 2);
}

HAL_StatusTypeDef bq25731_get_charge_current_reg(uint8_t* data)
{
	return bq25731_read_reg(CHARGE_CURRENT_REG, data, 2);
}

HAL_StatusTypeDef bq25731_get_sys_and_bat_voltage(bq25731_t* bq, uint16_t *vbat, uint16_t *vsys)
{
	HAL_StatusTypeDef result = bq25731_read_reg(ADC_SYS_AND_BAT_VOLTAGE_REG, (uint8_t *)&bq->ADCVSYS_VBAT, 2);
	if(result != HAL_OK) return result;
	uint32_t charge_voltage = 8*(BIT_TO_DEC(bq->ChargeVoltage.BIT,0) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,1) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,2) + BIT_TO_DEC(bq->ChargeVoltage.BIT,3) + \
			  BIT_TO_DEC(bq->ChargeVoltage.BIT,4) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,5) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,6) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,7) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,8) +
			  BIT_TO_DEC(bq->ChargeVoltage.BIT,9) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,10) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,10) +  BIT_TO_DEC(bq->ChargeVoltage.BIT,11));

	if(charge_voltage <= 16800) //4cell bat
	{
		if(bq->ADCVSYS_VBAT.VBAT == 0)
		{
			*vbat = 0;
		}else
		{
			*vbat = bq->ADCVSYS_VBAT.VBAT * 64 + 2880;
		}
		if(bq->ADCVSYS_VBAT.VSYS == 0)
		{
			*vsys = 0;
		}else
		{
			*vsys = bq->ADCVSYS_VBAT.VSYS * 64 + 2880;
		}
	}else
	{ //5cels
		if(bq->ADCVSYS_VBAT.VBAT == 0)
		{
			*vbat = 0;
		}else
		{
			*vbat = bq->ADCVSYS_VBAT.VBAT * 64 + 8160;
		}
		if(bq->ADCVSYS_VBAT.VSYS == 0)
		{
			*vsys = 0;
		}else
		{
			*vsys = bq->ADCVSYS_VBAT.VSYS * 64 + 8160;
		}
	}
	return result;
}

HAL_StatusTypeDef bq25731_get_charge_discharge_current(bq25731_t* bq, uint16_t *charge_current, uint16_t *discharge_current)
{
	HAL_StatusTypeDef result = bq25731_read_reg(ADC_BAT_CHARGE_DISCHARGE_CURRENT_REG,(uint8_t *)&bq->ADCIBAT, 2);
	if(result != HAL_OK) return result;
	if(bq->ChargeOption1.RSNS_RSR) //Rsense = 5 mOhm
	{
		*charge_current = bq->ADCIBAT.Charge_Current*128;
		*discharge_current = bq->ADCIBAT.Discharge_Current*512;
	}else//Rsense = 10 mOhm
	{
		*charge_current = bq->ADCIBAT.Charge_Current*64;
		*discharge_current = bq->ADCIBAT.Discharge_Current*256;
	}

	return result;
}


//Get Bus voltage and power system
HAL_StatusTypeDef bq25731_get_vbus_psys(bq25731_t* bq, uint16_t *vbus, uint16_t *psys)
{
	HAL_StatusTypeDef result = bq25731_read_reg(ADC_VBUS_PSYS_REG, (uint8_t *)&bq->ADCVBUS_PSYS, 2);
	*vbus = bq->ADCVBUS_PSYS.VBUS * 96;
	if(bq->ADCOption.ADC_FULLSCALE)
	{
		*psys =  bq->ADCVBUS_PSYS.PSYS * 12;
	}else
	{
		*psys =  bq->ADCVBUS_PSYS.PSYS * 8;
	}
	return result;
}

HAL_StatusTypeDef bq25731_get_input_current(uint8_t* data)
{
	return bq25731_read_reg(ADC_INPUT_CURRENT_CMPIN_VOLTAGE_REG, data, 2);
}


HAL_StatusTypeDef bq25731_get_charge_status(bq25731_t* bq)
{
	HAL_StatusTypeDef result = bq25731_read_reg(CHARGE_STATUS_REG, (uint8_t *)&bq->ChargerStatus, 2);
	return result;
}

HAL_StatusTypeDef bq25731_get_prochot_status(bq25731_t* bq)
{
	return bq25731_read_reg(PROCHOT_STATUS_REG, (uint8_t *)&bq->ProchotStatus, 2);
}

HAL_StatusTypeDef bq25731_charge_option_0(uint8_t lsb, uint8_t msb)
{
	return bq25731_set_bit_reg(CHARGE_OPTION_0_REG, lsb, msb);
}

HAL_StatusTypeDef bq25731_charge_option_0_clear_bit(uint8_t lsb, uint8_t msb)
{
	return bq25731_clear_bit_reg(CHARGE_OPTION_0_REG, lsb, msb);
}

HAL_StatusTypeDef bq25731_read_charge_option_0(bq25731_t* bq)
{
	return bq25731_read_reg(CHARGE_OPTION_0_REG, (uint8_t*)&bq->ChargeOption0, 2);
}

HAL_StatusTypeDef bq25731_charge_option_1(uint8_t lsb,uint8_t msb)
{
	return bq25731_set_bit_reg(CHARGE_OPTION_1_REG, lsb, msb);
}

HAL_StatusTypeDef bq25731_charge_option_3(uint8_t lsb,uint8_t msb)
{
	return bq25731_set_bit_reg(CHARGE_OPTION_3_REG, lsb, msb);
}

HAL_StatusTypeDef bq25731_read_charge_option_1(bq25731_t* bq)
{
	return bq25731_read_reg(CHARGE_OPTION_1_REG, (uint8_t*)&bq->ChargeOption1, 2);
}
HAL_StatusTypeDef bq25731_read_charge_option_2(bq25731_t* bq)
{
	return bq25731_read_reg(CHARGE_OPTION_2_REG, (uint8_t*)&bq->ChargeOption2, 2);
}

HAL_StatusTypeDef bq25731_read_charge_option_3(bq25731_t* bq)
{
	return bq25731_read_reg(CHARGE_OPTION_3_REG, (uint8_t*)&bq->ChargeOption3, 2);
}

HAL_StatusTypeDef bq25731_set_adc_option(uint8_t lsb, uint8_t msb)
{
	return bq25731_set_bit_reg(ADC_OPTION_REG, lsb, msb);
}

HAL_StatusTypeDef bq25731_read_adc_option(bq25731_t* bq)
{
	return bq25731_read_reg(ADC_OPTION_REG, (uint8_t*)&bq->ADCOption, 2);
}

HAL_StatusTypeDef bq25731_disable_charge(bq25731_t* bq)
{
	return bq25731_clear_bit_reg(CHARGE_OPTION_0_REG, CHRG_INHIBIT_BIT , 0);
}

