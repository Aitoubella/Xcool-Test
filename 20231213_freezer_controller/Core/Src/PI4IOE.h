/*
 * PI4IOE.h
 *
 *  Created on: Oct 28, 2023
 *      Author: Loc
 */

#ifndef SRC_PI4IOE_H_
#define SRC_PI4IOE_H_
#include <stdint.h>
#include "main.h"

#define PI4IO_GPIO_PIN_0                 ((uint8_t)0x01)  /* Pin 0 selected    */
#define PI4IO_GPIO_PIN_1                 ((uint8_t)0x02)  /* Pin 1 selected    */
#define PI4IO_GPIO_PIN_2                 ((uint8_t)0x04)  /* Pin 2 selected    */
#define PI4IO_GPIO_PIN_3                 ((uint8_t)0x08)  /* Pin 3 selected    */
#define PI4IO_GPIO_PIN_4                 ((uint8_t)0x10)  /* Pin 4 selected    */
#define PI4IO_GPIO_PIN_5                 ((uint8_t)0x20)  /* Pin 5 selected    */
#define PI4IO_GPIO_PIN_6                 ((uint8_t)0x40)  /* Pin 6 selected    */
#define PI4IO_GPIO_PIN_7                 ((uint8_t)0x80)  /* Pin 7 selected    */
#define PI4IO_GPIO_PIN_All               ((uint8_t)0xFF)  /* All pins selected */


typedef struct
{
	uint8_t input;
	uint8_t output;
	uint8_t polarity_inversion;
	uint8_t config;
	uint8_t output_strength_0;
	uint8_t output_strength_1;
	uint8_t input_latch;
	uint8_t pull_up_down_enable;
	uint8_t pull_up_down_selection;
	uint8_t interrupt_mask;
	uint8_t interrupt_status;
	uint8_t output_port_config;
}PI4IO_GPIO_Typedef_t;

typedef enum
{
	PI4IO_PIN_RESET = 0,
	PI4IO_PIN_SET
}PI4IO_State_t;

typedef enum
{
	GPIO_OUPUT_PUSH_PULL = 0,
	GPIO_OPEN_DRAIN
}PI4IO_Gpio_Ouput_Mode_t;



extern PI4IO_GPIO_Typedef_t PI4IO_GPIO0;
extern PI4IO_GPIO_Typedef_t PI4IO_GPIO1;

HAL_StatusTypeDef PI4IOE_GPIO_Mode_Input(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin);
HAL_StatusTypeDef PI4IOE_GPIO_Mode_Ouput(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin);
HAL_StatusTypeDef PI4IOE_GPIO_Write(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin, PI4IO_State_t state);
HAL_StatusTypeDef PI4IOE_GPIO_Gpio_Ouput_Mode(PI4IO_GPIO_Typedef_t* gpio, PI4IO_Gpio_Ouput_Mode_t mode);
HAL_StatusTypeDef PI4IOE_GPIO_Togle(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin);
HAL_StatusTypeDef PI4IOE_GPIO_Set_Input_Invert(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin);
HAL_StatusTypeDef PI4IOE_GPIO_Read_Input(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin, PI4IO_State_t* state);



#endif /* SRC_PI4IOE_H_ */
