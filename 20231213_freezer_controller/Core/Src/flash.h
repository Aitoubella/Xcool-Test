/*
 * flash.h
 *
 *  Created on: Jul 5, 2023
 *      Author: Loc
 */

#ifndef SRC_FLASH_H_
#define SRC_FLASH_H_
#include <stdint.h>
#include "flash_page.h"
#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_250   /* Start @ of user Flash area */
#define USER_FLASH_SIZE         0x800               /*1 page*/




HAL_StatusTypeDef flash_mgt_save(uint32_t *data, uint32_t length);
void flash_mgt_read(uint32_t* data, uint32_t length);
HAL_StatusTypeDef flash_erase_page(uint32_t addr);
#endif /* SRC_FLASH_H_ */
