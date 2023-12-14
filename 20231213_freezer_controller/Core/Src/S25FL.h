/* Includes ------------------------------------------------------------------*/
#ifndef _S25FL_H_
#define _S25FL_H_
#include <stdint.h>
#include "main.h"
#include "gpio.h"
#define S25FL_WRITE_STATUS  0x01
#define S25FL_WRITE         0x02
#define S25FL_READ          0x03
#define S25FL_WRDI          0x04
#define S25FL_RDSR          0x05
#define S25FL_WREN          0x06
#define S25FL_SECTOR_ERASE  0x20
#define S25FL_CHIP_ERASE    0xC7
#define S25FL_BLOCK_ERASE   0xD8
#define RDID                0x9F
#define WIP_Flag            0x01
#define Dummy_Byte          0x00


#define S25FL_PIN_CS	    FLASH_CS_Pin  //CS Pin
#define S25FL_PORT          FLASH_CS_GPIO_Port


/* Select SPI FLASH: ChipSelect pin low  */
#define Select_Flash()       HAL_GPIO_WritePin (S25FL_PORT, S25FL_PIN_CS, GPIO_PIN_RESET)
/* Deselect SPI FLASH: ChipSelect pin high */
#define NotSelect_Flash()    HAL_GPIO_WritePin(S25FL_PORT, S25FL_PIN_CS, GPIO_PIN_SET)



void SPI_Flash_Init(void);
HAL_StatusTypeDef SPI_Flash_ReadByte(uint8_t* rx);
HAL_StatusTypeDef SPI_Flash_SendByte(uint8_t byte);
HAL_StatusTypeDef FlashSectorEarse(uint32_t page);
HAL_StatusTypeDef FlashPageRead(uint32_t page,uint8_t *Data);
HAL_StatusTypeDef FlashPageWrite(uint32_t page,uint8_t *Data);
HAL_StatusTypeDef FlashWaitBusy(void);
HAL_StatusTypeDef Flash_Read_Array(uint32_t start_address,uint8_t buffer[],uint16_t length);
HAL_StatusTypeDef Flash_Write_Array(uint32_t start_address,uint8_t buffer[],uint16_t length);


#endif
