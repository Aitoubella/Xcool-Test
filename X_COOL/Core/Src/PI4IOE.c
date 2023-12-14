/*
 * PI4IOE.c
 *
 *  Created on: Oct 28, 2023
 *      Author: Loc
 */


#include "PI4IOE.h"
#include "i2c.h"
#include "main.h"
//#define PI4IOE_I2C_ADDR   0x21 //ADDR PIN logic 1
#define PI4IOE_I2C_ADDR     0x20 //ADDR PIN logic 0

#define PI4IOE_I2C        hi2c1



#define INPUT_PORT0_REG  0x00
#define INPUT_PORT1_REG  0x01
#define OUTPUT_PORT0_REG  0x02
#define OUTPUT_PORT1_REG  0x03
#define POLARITY_INVERSION_PORT0_REG   0x04
#define POLARITY_INVERSION_PORT1_REG   0x05
#define CONFIG_PORT0_REG               0x06
#define CONFIG_PORT1_REG               0x07
#define OUTPUT_DRIVER_STRENGTH_0_REG   0x40
#define OUTPUT_DRIVER_STRENGTH_01_REG  0x41
#define OUTPUT_DRIVER_STRENGTH_1_REG   0x42
#define OUTPUT_DRIVER_STRENGTH_11_REG  0x43
#define INPUT_LATCH_0_REG              0x44
#define INPUT_LATCH_1_REG              0x45
#define PULL_UP_DOWN_ENABLE_0_REG      0x46
#define PULL_UP_DOWN_ENABLE_1_REG      0x47
#define PULL_UP_DOWN_SELECTION_0_REG   0x48
#define PULL_UP_DOWN_SELECTION_1_REG   0x49
#define INTERRUPT_MASK_0_REG           0x4A
#define INTERRUPT_MASK_1_REG           0x4B
#define INTERRUPT_STATUS_0_REG         0x4C
#define INTERRUPT_STATUS_1_REG         0x4D
#define OUTPUT_PORT_CONFIG_REG         0x4F

#define OUPUT_PORT_CONFIG_ODEN0_BIT   0
#define OUPUT_PORT_CONFIG_ODEN1_BIT   1



#define PI4IO_PORT_0 {INPUT_PORT0_REG, OUTPUT_PORT0_REG, POLARITY_INVERSION_PORT0_REG, CONFIG_PORT0_REG, \
					 OUTPUT_DRIVER_STRENGTH_0_REG, OUTPUT_DRIVER_STRENGTH_01_REG, INPUT_LATCH_0_REG, PULL_UP_DOWN_ENABLE_0_REG, \
	                 PULL_UP_DOWN_SELECTION_0_REG, INTERRUPT_MASK_0_REG, INTERRUPT_STATUS_0_REG, OUTPUT_PORT_CONFIG_REG}

#define PI4IO_PORT_1 {INPUT_PORT1_REG, OUTPUT_PORT1_REG, POLARITY_INVERSION_PORT1_REG, CONFIG_PORT1_REG, \
					 OUTPUT_DRIVER_STRENGTH_1_REG, OUTPUT_DRIVER_STRENGTH_11_REG, INPUT_LATCH_1_REG, PULL_UP_DOWN_ENABLE_1_REG, \
	                 PULL_UP_DOWN_SELECTION_1_REG, INTERRUPT_MASK_1_REG, INTERRUPT_STATUS_1_REG, OUTPUT_PORT_CONFIG_REG}


PI4IO_GPIO_Typedef_t PI4IO_GPIO0 = PI4IO_PORT_0;
PI4IO_GPIO_Typedef_t PI4IO_GPIO1 = PI4IO_PORT_1;

HAL_StatusTypeDef PI4IOE_Write_Reg(uint8_t reg, uint8_t data)
{
	uint8_t data_temp[2] = {reg,data};
	return HAL_I2C_Master_Transmit(&PI4IOE_I2C, (PI4IOE_I2C_ADDR << 1),data_temp, 2, 100);
}

HAL_StatusTypeDef PI4IOE_Read_Reg(uint8_t reg, uint8_t* data)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Master_Transmit(&PI4IOE_I2C, (PI4IOE_I2C_ADDR << 1) | 0x01, &reg, 1, 100); //Send write operation to move to reg
	if(status != HAL_OK) return status;

	return HAL_I2C_Master_Receive(&PI4IOE_I2C, (PI4IOE_I2C_ADDR << 1), data, 1, 100); //Read 1 data from current reg
}

HAL_StatusTypeDef PI4IOE_GPIO_Mode_Input(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->config,&value);
	if(status != HAL_OK) return status;
	value |= pin;       //Enable bit to 1 to config input
	return PI4IOE_Write_Reg(gpio->config, value | pin);
}

HAL_StatusTypeDef PI4IOE_GPIO_Mode_Ouput(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->config,&value);
	if(status != HAL_OK) return status;
	value &= ~ pin;  //Reset bit to 0 to config ouput
	return PI4IOE_Write_Reg(gpio->config, value);
}

HAL_StatusTypeDef PI4IOE_GPIO_Write(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin, PI4IO_State_t state)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->output,&value);
	if(status != HAL_OK) return status;
	if(state == PI4IO_PIN_SET)
	{
		value |= pin;
	}else
	{
		value &= ~ pin;
	}
	return PI4IOE_Write_Reg(gpio->output, value);
}


HAL_StatusTypeDef PI4IOE_GPIO_Gpio_Ouput_Mode(PI4IO_GPIO_Typedef_t* gpio, PI4IO_Gpio_Ouput_Mode_t mode)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->output_port_config,&value);
	if(status != HAL_OK) return status;
	if(gpio == &PI4IO_GPIO0)
	{
		if(mode == GPIO_OUPUT_PUSH_PULL)
		{
			gpio->output_port_config &= ~ OUPUT_PORT_CONFIG_ODEN0_BIT;
		}else
		{
			gpio->output_port_config |= OUPUT_PORT_CONFIG_ODEN0_BIT;
		}
	}else
	{
		if(mode == GPIO_OUPUT_PUSH_PULL)
		{
			gpio->output_port_config &= ~ OUPUT_PORT_CONFIG_ODEN1_BIT;
		}else
		{
			gpio->output_port_config |= OUPUT_PORT_CONFIG_ODEN0_BIT;
		}
	}
	return status;
}

HAL_StatusTypeDef PI4IOE_GPIO_Togle(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->output,&value);
	if(status != HAL_OK) return status;
	value ^= pin;
	return PI4IOE_Write_Reg(gpio->output, value);
}

HAL_StatusTypeDef PI4IOE_GPIO_Set_Input_Invert(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = PI4IOE_Read_Reg(gpio->polarity_inversion,&value);
	if(status != HAL_OK) return status;
	value |= pin; //Set to enable invert input logic
	return PI4IOE_Write_Reg(gpio->polarity_inversion, value);
}

HAL_StatusTypeDef PI4IOE_GPIO_Read_Input(PI4IO_GPIO_Typedef_t* gpio, uint8_t pin, PI4IO_State_t* state)
{
	uint8_t value = 0;
	HAL_StatusTypeDef status = PI4IOE_Read_Reg(gpio->input,&value);
	if(value & pin) return *state = PI4IO_PIN_SET;
	else *state = PI4IO_PIN_RESET;
	return status;
}








