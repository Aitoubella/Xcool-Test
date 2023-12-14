/*
 * board.h
 *
 *  Created on: Oct 23, 2023
 *      Author: Loc
 */

#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_
#include "main.h"
//Chamber fan and compressor fan
#define FAN1_ON()     HAL_GPIO_WritePin(FAN_CTL1_GPIO_Port,FAN_CTL1_Pin, GPIO_PIN_SET)
#define FAN1_OFF()    HAL_GPIO_WritePin(FAN_CTL1_GPIO_Port,FAN_CTL1_Pin, GPIO_PIN_RESET)

#define FAN2_ON()     HAL_GPIO_WritePin(FAN_CTL2_GPIO_Port,FAN_CTL2_Pin, GPIO_PIN_SET)
#define FAN2_OFF()    HAL_GPIO_WritePin(FAN_CTL2_GPIO_Port,FAN_CTL2_Pin, GPIO_PIN_RESET)



#define CMPRSR_ENABLE()   HAL_GPIO_WritePin(CMPRSR_EN_GPIO_Port,CMPRSR_EN_Pin, GPIO_PIN_SET)
#define CMPRSR_DISABLE()   HAL_GPIO_WritePin(CMPRSR_EN_GPIO_Port,CMPRSR_EN_Pin, GPIO_PIN_RESET)

#define CMPRSR_FAN_VCC_ON()  HAL_GPIO_WritePin(CMPRSR_FAN_VCC_GPIO_Port,CMPRSR_FAN_VCC_Pin, GPIO_PIN_SET)
#define CMPRSR_FAN_VCC_OFF() HAL_GPIO_WritePin(CMPRSR_FAN_VCC_GPIO_Port,CMPRSR_FAN_VCC_Pin, GPIO_PIN_RESET)
#define CMPRSR_FAN_GND_ON()  HAL_GPIO_WritePin(CMPRSR_FAN_GND_GPIO_Port,CMPRSR_FAN_GND_Pin, GPIO_PIN_SET)
#define CMPRSR_FAN_GND_OFF() HAL_GPIO_WritePin(CMPRSR_FAN_GND_GPIO_Port,CMPRSR_FAN_GND_Pin, GPIO_PIN_RESET)


#define CMPRSR_SPD_ON()  HAL_GPIO_WritePin(CMPRSR_SPD_GPIO_Port,CMPRSR_SPD_Pin, GPIO_PIN_SET)
#define CMPRSR_SPD_OFF() HAL_GPIO_WritePin(CMPRSR_SPD_GPIO_Port,CMPRSR_SPD_Pin, GPIO_PIN_RESET)




#define HTR_CTL_ON()  HAL_GPIO_WritePin(CHRG_OK_GPIO_Port,CHRG_OK_Pin, GPIO_PIN_SET) //Heater on
#define HTR_CTL_OFF() HAL_GPIO_WritePin(CHRG_OK_GPIO_Port,CHRG_OK_Pin, GPIO_PIN_RESET) //Heater off


typedef enum
{
	CHARGE_STATUS_FALT = 0,
	CHARGE_STATUS_NORMAL,
}charge_status_t;

typedef enum
{
	CMPRSR_STATUS_FAILT = 0,
	CMPRSR_STATUS_NORMAL,
}cmprsr_status_t;

charge_status_t get_charge_status(void);


void pwr_ctrl_on(void);
void pwr_ctrl_off(void);
#endif /* SRC_BOARD_H_ */
