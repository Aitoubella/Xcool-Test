/*
 * app_i2c.h
 *
 *  Created on: Dec 21, 2023
 *      Author: Loc
 */

#ifndef SRC_APP_I2C_H_
#define SRC_APP_I2C_H_
#include "i2c.h"


HAL_StatusTypeDef I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,uint16_t Size, uint32_t Timeout);

#endif /* SRC_APP_I2C_H_ */
