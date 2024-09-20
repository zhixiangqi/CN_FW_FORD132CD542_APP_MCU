/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    FlashApp.c

  @Summary
    Setup Flash relevent function for main function to call.

  @Description
    -Setup a function for flashing MCU memory.
    -Setup a function for External Flash Write/Read.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/FlashApp.h"
#include "driver/inc/NVMDriver.h"
#include "driver/inc/SPIMDriver.h"

uint8_t u8reciveBuffer[256]={0};
uint8_t u8sendBuffer[256]={0};
void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length)
{
    uint8_t dataStr[256] = {0};
    (void)memcpy((void *)dataStr, (void *) FLASH_ROW_ADDRESS( address ), SIZE_ROW);
    uint8_t start_addr = (uint8_t)(address & 0xFF);
    for(uint16_t addr = 0U; addr < length; addr ++){
        if((start_addr + addr) <= 0xFF){
            dataStr[start_addr + addr] = data[addr];
        }else{
            /* error write*/
        }
    }
    NVMDriver_PageWrite(dataStr,FLASH_ROW_ADDRESS(address));
}

uint8_t GD25Q80E_ReadStatusRegister(void)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;

  u8sendBuffer[0] = GD25Q80E_READ_STATUS1;

  u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,u8reciveBuffer,2U);
  if (u8Spistatus == CY_SCB_SPI_SUCCESS)
  {
    u8Value = u8reciveBuffer[0];
  }

	return u8Value;
}

void GD25Q80E_WaitWhileBusy(void)
{
	while (GD25Q80E_ReadStatusRegister() & GD25Q80E_STATUS_BUSY);
}

uint32_t GD25Q80E_ReadJedecId(void)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint32_t u32Value = GD25Q80E_STATUS_SUCCESS;

  GD25Q80E_WaitWhileBusy();

  u8sendBuffer[0] = GD25Q80E_DEVICE_ID;

	u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,u8reciveBuffer,4U);
	if (u8Spistatus == CY_SCB_SPI_SUCCESS)
  {
    u32Value = (u8reciveBuffer[2] <<16) | (u8reciveBuffer[1] <<8) | (u8reciveBuffer[0]);
  }

	return u32Value;
}

uint8_t GD25Q80E_WriteEnable(bool enabled)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;
  GD25Q80E_WaitWhileBusy();
  if (enabled)
  {
    u8sendBuffer[0]=GD25Q80E_WRITE_ENABLE;
  }
  else
  {
    u8sendBuffer[0]=GD25Q80E_WRITE_DISABLE;
  }
  u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,NULL,1U);
  if (u8Spistatus == CY_SCB_SPI_SUCCESS)
  {
    u8Value = u8Spistatus;
  }

  return u8Value;  
}

uint8_t GD25Q80E_WriteStatusRegister(uint8_t u8statusRegister)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Result;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;

  u8Result = GD25Q80E_WriteEnable(TRUE);
  if (u8Result == CY_SCB_SPI_SUCCESS)
  {
    u8sendBuffer[0] = GD25Q80E_WRITE_STATUS;
    u8sendBuffer[1] = u8statusRegister;

    u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,u8reciveBuffer,2U);
    if (u8Spistatus == CY_SCB_SPI_SUCCESS)
    {
      u8Value = u8Spistatus;
    }
  }

	return u8Value;
}

uint8_t GD25Q80E_EraseChip(void)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Result;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;

  u8Result = GD25Q80E_WriteEnable(TRUE);
  if (u8Result == CY_SCB_SPI_SUCCESS)
  {
    u8sendBuffer[0] = GD25Q80E_CHIP_ERASE;
    u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,NULL,1);
    if (u8Spistatus == CY_SCB_SPI_SUCCESS)
    {
      u8Value = u8Spistatus;
    }
  }

  return u8Value; 
}
uint8_t GD25Q80_ReadData(uint32_t u32addr, uint16_t u16len, uint8_t* u8data)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;

  GD25Q80E_WaitWhileBusy();

  u8sendBuffer[0] = GD25Q80E_READ_DATA;
  u8sendBuffer[1] = (u32addr & 0xFF0000) >> 16;
  u8sendBuffer[2] = (u32addr & 0xFF00) >> 8;
  u8sendBuffer[3] = u32addr & 0xFF;

  u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,u8reciveBuffer,4 + u16len);
  if (u8Spistatus == CY_SCB_SPI_SUCCESS)
  {
    for (uint16_t i = 0; i < u16len; i++)
    {
      u8data[i] = u8reciveBuffer[i];
    }
    u8Value = u8Spistatus;
  }

  return u8Value;
}

uint8_t GD25Q80E_PageProgram(uint32_t u32addr, uint16_t u16len, uint8_t* u8data)
{
  memset(u8reciveBuffer,0xFF,sizeof(u8reciveBuffer));
  memset(u8sendBuffer,0xFF,sizeof(u8sendBuffer));
  uint8_t u8Spistatus;
  uint8_t u8Result;
  uint8_t u8Value = GD25Q80E_STATUS_SUCCESS;

	u8Result = GD25Q80E_WriteEnable(TRUE);
  if (u8Result == CY_SCB_SPI_SUCCESS)
  {
    /* do nothing if asked to write beyond a page boundary */
    if (((u32addr & 0xFF) + u16len) > GD25Q80E_PAGE_LEN)
      return GD25Q80E_STATUS_ADDR_OUT_OF_BOUND;

    /* do nothing if we would overflow the flash */
    if (u32addr > (GD25Q80E_NUM_BYTES - u16len))
      return GD25Q80E_STATUS_ADDR_OUT_OF_BOUND;

    u8sendBuffer[0] = GD25Q80E_PAGE_PROGRAM;
    u8sendBuffer[1] = (u32addr & 0xFF0000) >> 16;
    u8sendBuffer[2] = (u32addr & 0xFF00) >> 8;
    u8sendBuffer[3] = u32addr & 0xFF;
    for (uint16_t i = 0; i < u16len; i++)
    {
      u8sendBuffer[i+4] = u8data[i];
    }

    u8Spistatus = SPIMDriver_Transfer(u8sendBuffer,NULL,4 + u16len);
    if (u8Spistatus == CY_SCB_SPI_SUCCESS)
    {
      u8Value = u8Spistatus;
    }
  }

  return u8Value;
}
