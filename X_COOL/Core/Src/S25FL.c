/********************(C)COPYRIGHT 2023 lochoangvan@gmail.com********************/

#include "stm32l4xx.h"
#include "S25FL.h"
#include "spi.h"
#define Sector_Size  4096       //256Byte
#define Page_Size    256        //byte
#define Total_Sector 32        //Total sector of M25P16
extern SPI_HandleTypeDef hspi1;
#define S25FL_SPI        hspi1


/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : tx : byte to send.
* Output         : None
* Return         : Status transmit
*******************************************************************************/
HAL_StatusTypeDef SPI_Flash_SendByte(uint8_t tx)
{
	return HAL_SPI_Transmit(&S25FL_SPI, &tx, 1, 100);
}

/*******************************************************************************
* Function Name  : SPI_Flash_SendRecv
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : tx : byte to send.
* Output         : recv: byte to receive
* Return         : Status transmit
*******************************************************************************/
HAL_StatusTypeDef SPI_Flash_SendRecv(uint8_t tx, uint8_t* rx)
{
	return HAL_SPI_TransmitReceive(&S25FL_SPI, &tx, rx, 1, 100);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
HAL_StatusTypeDef SPI_Flash_ReadByte(uint8_t* rx)
{
  return SPI_Flash_SendRecv(Dummy_Byte, rx);
}
/*******************************************************************************
* Function Name  : FlashReadID
* Description    : Reads a ID from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte ID from the SPI Flash.
*******************************************************************************/
HAL_StatusTypeDef FlashReadID(uint8_t *Data)
{
	HAL_StatusTypeDef status = HAL_OK;
	Select_Flash();	
  	SPI_Flash_SendByte(RDID);
  	for(uint8_t i = 0; i < 4; i++)
  	{
  		status = SPI_Flash_ReadByte(&Data[i]);
  		if(status != HAL_OK) return status;
  	}
  NotSelect_Flash();	
  return status;
}
/*******************************************************************************
* Function Name  : FlashWaitBusy
* Description    : Check flash busy
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
HAL_StatusTypeDef FlashWaitBusy(void)
{
  uint8_t FLASH_Status = 0;
  HAL_StatusTypeDef status = HAL_OK;
  uint16_t err_count = 0;
  Select_Flash();	
  SPI_Flash_SendByte(S25FL_RDSR);
  do
  {
	  status = SPI_Flash_SendRecv(Dummy_Byte, &FLASH_Status);
	  if(status != HAL_OK) return status;
	  err_count ++;
//	  if(err_count >  100) return HAL_BUSY;
  }
  while (((FLASH_Status & WIP_Flag) == SET));

  NotSelect_Flash();
  return status;
}





/*******************************************************************************
* Function Name  : FlashSectorEarse
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : tx : byte to send.
* Output         : recv: byte to receive
* Return         : Status transmit
*******************************************************************************/

HAL_StatusTypeDef FlashSectorEarse(uint32_t page)
{	
    HAL_StatusTypeDef status = HAL_OK;
    static uint8_t adress[4];
	FlashWaitBusy();
	Select_Flash();
	status = SPI_Flash_SendByte(S25FL_WREN);
	if(status != HAL_OK) return status;
	NotSelect_Flash();
	FlashWaitBusy();

	Select_Flash();
	adress[0] = S25FL_SECTOR_ERASE;
	adress[1] = (page >> 16) & 0xFF;
	adress[2] = (page >> 8) & 0xFF;
	adress[3] = page & 0xFF;
	status = HAL_SPI_Transmit(&S25FL_SPI, adress, 4, 100);
	NotSelect_Flash();
	return status;
}

/*******************************************************************************
* Function Name  : Flash_Read_Array
* Description    : Read array of byte from flash
* Input          : start_address: address start to read
*                : length: Length of array data to read
* Output         : buffer: contain data
* Return         : None
*******************************************************************************/
HAL_StatusTypeDef Flash_Read_Array(uint32_t start_address, uint8_t buffer[], uint16_t length)
{
	HAL_StatusTypeDef status;
	static uint8_t adress[4];
	FlashWaitBusy(); 
	Select_Flash();
	adress[0] = S25FL_READ;
	adress[1] = (start_address >> 16) & 0xFF;
	adress[2] = (start_address >> 8) & 0xFF;
	adress[3] = start_address & 0xFF;

	status = HAL_SPI_Transmit(&S25FL_SPI, adress, 4, 100);
	if(status != HAL_OK) return status;
	status = HAL_SPI_Receive(&S25FL_SPI, buffer, length, 100);
	NotSelect_Flash();
	return status;
}

/*******************************************************************************
* Function Name  : Flash_Read_Array
* Description    : Write array of byte to flash
* Input          : start_address: address start to write
*                : length: Length of array data to write
* Output         : buffer: contain data
* Return         : None
*******************************************************************************/
HAL_StatusTypeDef Flash_Write_Array(uint32_t start_address,uint8_t buffer[],uint16_t length)
{
	HAL_StatusTypeDef status;
	static uint8_t adress[4];
	if(start_address%0x1000 == 0)
	{
		status = FlashSectorEarse(start_address);
		if(status != HAL_OK) return status;
	}
	status = FlashWaitBusy();
	if(status != HAL_OK) return status;
	Select_Flash();
	status = SPI_Flash_SendByte(S25FL_WREN);
	if(status != HAL_OK) return status;
	NotSelect_Flash();
	status = FlashWaitBusy();
	if(status != HAL_OK) return status;
	Select_Flash();
	adress[0] = S25FL_WRITE;
	adress[1] = (start_address >> 16) & 0xFF;
	adress[2] = (start_address >> 8) & 0xFF;
	adress[3] = start_address & 0xFF;
	status = HAL_SPI_Transmit(&S25FL_SPI, adress, 4, 100);
	if(status != HAL_OK) return status;
	status = HAL_SPI_Transmit(&S25FL_SPI, adress, 4, 100);
	NotSelect_Flash();
	return status;
}

