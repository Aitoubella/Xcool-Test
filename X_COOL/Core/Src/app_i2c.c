/*
 * app_i2c.c
 *
 *  Created on: Dec 21, 2023
 *      Author: Loc
 */


#include "app_i2c.h"
#include "i2c.h"
#include "main.h"


//#define USE_DELAY_FREERTOS

#define I2C_DEBUG_PRINT
#ifdef I2C_DEBUG_PRINT
#include "printf.h"
#endif
#ifdef USE_DELAY_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

static const uint32_t delay_ms = 1 / portTICK_PERIOD_MS;
#endif


HAL_StatusTypeDef I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,uint16_t Size, uint32_t Timeout)
{
	static HAL_StatusTypeDef status = HAL_OK;
	uint32_t error_count = 0;
	status = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
	if(status != HAL_OK)
	{
#ifdef I2C_DEBUG_PRINT
		printf("\nI2C transmit error: %d",status);
#endif
#ifdef USE_DELAY_FREERTOS
		vTaskDelay(delay_ms);
#endif
		HAL_I2C_DeInit(hi2c);
		HAL_I2C_Init(hi2c);
		return status;
	}
	/*  Before starting a new communication transfer, you need to check the current
	  state of the peripheral; if it�s busy you need to wait for the end of current
	  transfer before starting a new one.
	  For simplicity reasons, this example is just waiting till the end of the
	  transfer, but application may perform other tasks while transfer operation
	  is ongoing. */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{
		if(error_count++ > 1000)   //If reach max error count ->reset I2C
		{
#ifdef I2C_DEBUG_PRINT
		printf("\nI2C not ready error");
#endif
#ifdef USE_DELAY_FREERTOS
		vTaskDelay(delay_ms);
#endif
			HAL_I2C_DeInit(hi2c);
			HAL_I2C_Init(hi2c);
			status = HAL_ERROR;
			break;
		}
	}
	return status;
}

HAL_StatusTypeDef I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,uint16_t Size, uint32_t Timeout)
{
	static HAL_StatusTypeDef status = HAL_OK;
	uint32_t error_count = 0;
	status = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
	if(status != HAL_OK)
	{
#ifdef I2C_DEBUG_PRINT
		printf("\nI2C receive error: %d",status);
#endif
#ifdef USE_DELAY_FREERTOS
		vTaskDelay(delay_ms);
#endif
		HAL_I2C_DeInit(hi2c);
		HAL_I2C_Init(hi2c);
		return status;
	}

	/*  Before starting a new communication transfer, you need to check the current
	  state of the peripheral; if it�s busy you need to wait for the end of current
	  transfer before starting a new one.
	  For simplicity reasons, this example is just waiting till the end of the
	  transfer, but application may perform other tasks while transfer operation
	  is ongoing. */
	while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{
		if(error_count++ > 1000)   //If reach max error count ->reset I2C
		{
#ifdef I2C_DEBUG_PRINT
			printf("\nI2C not ready error");
#endif
#ifdef USE_DELAY_FREERTOS
		vTaskDelay(delay_ms);
#endif
			HAL_I2C_DeInit(hi2c);
			HAL_I2C_Init(hi2c);
			status = HAL_ERROR;
			break;
		}
	}

	return status;
}
