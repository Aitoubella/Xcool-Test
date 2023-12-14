/*
 * flash.c
 *
 *  Created on: Jul 5, 2023
 *      Author: Loc
 */


#include "flash.h"
#include "stm32l4xx_hal_flash.h"
#include <string.h>
#include <stdlib.h>

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
  	/* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
  	/* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }

  return bank;
}

/* Private typedef -----------------------------------------------------------*/
static HAL_StatusTypeDef flash_write_array(uint32_t start_addr, uint64_t* data, uint32_t length)
{
	uint32_t PAGEError;
	uint32_t i = 0;
	uint32_t addr = start_addr;
	static FLASH_EraseInitTypeDef EraseInitStruct;
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	while(addr < (start_addr + length))
	{
		if((addr - 0x8000000) % FLASH_PAGE_SIZE == 0)
		{
			/* Fill EraseInit structure*/
			EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.Page = GetPage(start_addr);
			EraseInitStruct.Banks = GetBank(start_addr);
			EraseInitStruct.NbPages     = 1; //number of page to erase
			if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
			{
				return HAL_ERROR;
			}
		}
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data[i]) != HAL_OK) //Each element take 4 bytes to write
		{
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
		addr += 8;
		i += 1;
	}
	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return HAL_OK;
}

/**
 * @brief Store data ram to flash
 * @return 0: Write data with no error
 * 		   1: Error in write progress
 * */

HAL_StatusTypeDef flash_mgt_save(uint32_t *data, uint32_t length)
{
	//write current flash
	if(flash_write_array(FLASH_USER_START_ADDR, (uint64_t *)data, length) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}


static void flash_mgt_read_array(uint32_t start_addr, uint32_t* data, uint32_t length)
{
	if(*(__IO uint32_t *)start_addr == 0xFFFFFFFF)
	{
		//Data in flash is empty
		return;
	}
	uint32_t addr = start_addr;
	while(addr < (start_addr + length))
	{
		*(data) = *(__IO uint32_t *)addr;
		addr += sizeof(uint32_t); //Move to next address flash
		data ++;                  //Move to next address ram
	 }
}

/**
 * @brief Read data from flash
 * @param data uint32 pointer read
 * @lenth number of byte data
 * @return None
 * */
void flash_mgt_read(uint32_t* data, uint32_t length)
{
	flash_mgt_read_array(FLASH_USER_START_ADDR, data, length);
}

/**
 * @brief Erase flash page
 * @return HAL_StatusTypeDef
 * */
HAL_StatusTypeDef flash_erase_page(uint32_t addr)
{
	uint32_t PAGEError;
	static FLASH_EraseInitTypeDef EraseInitStruct;
	if((addr - 0x8000000) % FLASH_PAGE_SIZE == 0)
	{
		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Page = GetPage(addr);
		EraseInitStruct.Banks = GetBank(addr);
		EraseInitStruct.NbPages     = 1; //number of page to erase
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			return HAL_ERROR;
		}
	}
	return HAL_OK;
}


