/*
 * power_board.h
 *
 *  Created on: Nov 9, 2023
 *      Author: Loc
 */

#ifndef SRC_POWER_BOARD_H_
#define SRC_POWER_BOARD_H_

#include "PI4IOE.h"
#include "main.h"
#define PWR_EN_PIN 	           PI4IO_GPIO_PIN_0
#define PWR_EN_GPIO            PI4IO_GPIO0
#define CMPRSR_D_I_PIN         PI4IO_GPIO_PIN_1
#define CMPRSR_D_I_GPIO        PI4IO_GPIO0
#define CMPRSR_FAN_PIN         PI4IO_GPIO_PIN_2
#define CMPRSR_FAN_GPIO        PI4IO_GPIO0


#define CMPRSR_EN_PIN          PI4IO_GPIO_PIN_4
#define CMPRSR_EN_GPIO         PI4IO_GPIO0

#define EX_PWR_ON_GPIO         EXT_PWR_ON_Z_GPIO_Port
#define EX_PWR_ON_PIN          EXT_PWR_ON_Z_Pin




HAL_StatusTypeDef power_board_init(void);
HAL_StatusTypeDef cmprsr_fan_on(void);
HAL_StatusTypeDef cmprsr_fan_off(void);
HAL_StatusTypeDef cmprsr_power_on(void);
HAL_StatusTypeDef cmprsr_power_off(void);
void ext_pwr_enable(void);
void ext_pwr_disable(void);
HAL_StatusTypeDef cmprsr_get_fault(PI4IO_State_t* state);

void fan1_on(void);
void fan1_off(void);
void fan2_on(void);
void fan2_off(void);
void htr_on(void);
void htr_off(void);

HAL_StatusTypeDef pwr_12v_on(void);
HAL_StatusTypeDef pwr_12v_off(void);
#endif /* SRC_POWER_BOARD_H_ */
