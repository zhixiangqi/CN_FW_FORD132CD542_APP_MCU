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

uint8_t u8txBufferFlash[128]={0x00};
uint8_t u8rxBufferFlash[128]={0x00};
uint8_t u8txBufferSizeFlash=0;
uint8_t u8rxBufferSizeFlash=0;
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

void FlashApp_WriteEnable()
{
  u8txBufferFlash[0]=FLASH_WRITE_ENABLE_CMD;

  u8txBufferSizeFlash=0x01U;
  u8rxBufferSizeFlash=0x00U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);
}
void FlashApp_WriteDisable()
{
  u8txBufferFlash[0]=FLASH_WRITE_DISABLE_CMD;

  u8txBufferSizeFlash=0x01U;
  u8rxBufferSizeFlash=0x00U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);
}
uint8_t FlashApp_ReadStatusRegister(uint8_t u8RDSR)
{
  if (u8RDSR==FLASH_READ_SR1_CMD)
  {
    u8txBufferFlash[0]=FLASH_READ_SR1_CMD;
  }
  if (u8RDSR==FLASH_READ_SR2_CMD)
  {
    u8txBufferFlash[0]=FLASH_READ_SR2_CMD;
  }
  u8txBufferSizeFlash=0x01U;
  u8rxBufferSizeFlash=0x01U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

  return u8rxBufferFlash[0];
}
void FlashApp_CheckBusy()
{
  uint8_t u8Rdsr;
  u8Rdsr = FlashApp_ReadStatusRegister(0x05);

  while((u8Rdsr & 0x01)==0x01); 
 }
void FlashApp_WriteStatusRegister(uint8_t dataByte1, uint8_t dataByte2)
{
  FlashApp_WriteEnable();

  u8txBufferFlash[0]=FLASH_WRITE_SR_CMD;
  u8txBufferFlash[1]=dataByte1;
  u8txBufferFlash[2]=dataByte2;

  u8txBufferSizeFlash=0x03U;
  u8rxBufferSizeFlash=0x00U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

}
void FlashApp_ReadDataBytes(uint32_t u32SelectAddr, uint16_t u16length)
{
  u8txBufferFlash[0]= FLASH_READ_DATA_CMD;
  u8txBufferFlash[1]=(uint8_t)(u32SelectAddr >> 16);
  u8txBufferFlash[2]=(uint8_t)(u32SelectAddr >> 8);
  u8txBufferFlash[3]=(uint8_t)(u32SelectAddr >> 0);

  u8txBufferSizeFlash=0x04U;
  u8rxBufferSizeFlash=u16length;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

}
void FlashApp_SectorErase(uint32_t u32SelectAddr)
{
  FlashApp_WriteEnable();

  u8txBufferFlash[0]=FLASH_SECTOR_ERASE_CMD;
  u8txBufferFlash[1]=(uint8_t)(u32SelectAddr >> 16);
  u8txBufferFlash[2]=(uint8_t)(u32SelectAddr >> 8);
  u8txBufferFlash[3]=(uint8_t)(u32SelectAddr >> 0);

  u8txBufferSizeFlash=0x04U;
  u8rxBufferSizeFlash=0x00U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

}
void FlashApp_EraseBlock(uint8_t u8BlockSize, uint32_t u32SelectAddr)
{
  FlashApp_WriteEnable();

  if (u8BlockSize==32U)
  {
    u8txBufferFlash[0]=FLASH_BLOCK_ERASE_32k_CMD;
  }
  if (u8BlockSize==64U)
  {
    u8txBufferFlash[0]=FLASH_BLOCK_ERASE_64k_CMD;
  }
  u8txBufferFlash[1]=(uint8_t)(u32SelectAddr >> 16);
  u8txBufferFlash[2]=(uint8_t)(u32SelectAddr >> 8);
  u8txBufferFlash[3]=(uint8_t)(u32SelectAddr >> 0);

  u8txBufferSizeFlash=0x04U;
  u8rxBufferSizeFlash=0x00U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

}
void FlashApp_EraseChip(uint32_t u32SelectAddr, uint16_t u16UseByte)
{
  FlashApp_WriteEnable();

  u8txBufferFlash[0]=FLASH_ERASE_CHIP_CMD;
  u8txBufferFlash[1]=(uint8_t)(u32SelectAddr >> 16);
  u8txBufferFlash[2]=(uint8_t)(u32SelectAddr >> 8);
  u8txBufferFlash[3]=(uint8_t)(u32SelectAddr >> 0);

  u8txBufferSizeFlash=0x04U;
  u8rxBufferSizeFlash=u16UseByte;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

}
void Flash_WritePage(uint32_t u32SelectAddr, uint8_t *u8sendBuffer, uint16_t u16length)
{
  FlashApp_WriteEnable();

  u8txBufferFlash[0]=FLASH_PAGE_PROGRAM_CMD;
  u8txBufferFlash[1]=(uint8_t)(u32SelectAddr >> 16);
  u8txBufferFlash[2]=(uint8_t)(u32SelectAddr >> 8);
  u8txBufferFlash[3]=(uint8_t)(u32SelectAddr >> 0);

  u8txBufferSizeFlash=0x04+u16length;
  u8rxBufferSizeFlash=0x00U;
  for (uint8_t i = 0; i < u16length; i++)
  {
    u8txBufferFlash[i+0x04] = u8sendBuffer[i];
  }
  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);
  
}
uint16_t Flash_ReadDeviceId()
{
  uint16_t u16Id=0;

  u8txBufferFlash[0]=FLASH_READ_DEVICE_ID_CMD;
  u8txBufferFlash[1]=0x00U;
  u8txBufferFlash[2]=0x00U;
  u8txBufferFlash[3]=0x00U;

  u8txBufferSizeFlash=0x04U;
  u8rxBufferSizeFlash=0x02U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);
  u16Id = (u8rxBufferFlash[1] << 8) | u8rxBufferFlash[0];

  return u16Id;
}
uint32_t Flash_ReadJedecId()
{
  uint32_t u32Id=0;

  u8txBufferFlash[0]=FLASH_READ_JEDEC_ID_CMD;

  u8txBufferSizeFlash=0x01U;
  u8rxBufferSizeFlash=0x03U;

  SPIMDriver_Transfer(u8txBufferFlash,u8rxBufferFlash,u8txBufferSizeFlash,u8rxBufferSizeFlash);

  u32Id = (u8rxBufferFlash[2] <<16) | (u8rxBufferFlash[1] <<8) | (u8rxBufferFlash[0]);

  return u32Id;
}