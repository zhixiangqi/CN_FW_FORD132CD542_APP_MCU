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
#include "app/inc/TC0App.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/WdtApp.h"
#include "driver/inc/NVMDriver.h"
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/UartDriver.h"

uint8_t dataSend[64] = {0xFF};
uint8_t dataRecv[64] = {0xFF};

uint8_t u8SendBuffLen = sizeof(dataSend);
uint8_t u8RecvBuffLen = sizeof(dataRecv);

uint8_t u8EraseFlag = 0;
uint8_t u8SectorFlag = 0;
uint8_t u8SectorNum = 0;
uint8_t u8PageNum = 0;
uint32_t u32Timer = 0;
uint8_t u8WriteCount = 0;
uint8_t u8ReadCount = 0;

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length)
{
    uint8_t dataSend[256] = {0};
    (void)memcpy((void *)dataSend, (void *) FLASH_ROW_ADDRESS( address ), SIZE_ROW);
    uint8_t start_addr = (uint8_t)(address & 0xFF);
    for(uint16_t addr = 0U; addr < length; addr ++){
        if((start_addr + addr) <= 0xFF){
            dataSend[start_addr + addr] = data[addr];
        }else{
            /* error write*/
        }
    }
    NVMDriver_PageWrite(dataSend,FLASH_ROW_ADDRESS(address));
}

void FlashApp_CheckNorFlash()
{
  /*Check Erase Flag*/
  u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0));
  if (u8EraseFlag != 0xC7U)
  {
    GD25Q_SPIFLASH_EraseChip();//Erase Chip
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
    
    /*Initial Value*/
    //Set Erase Flag
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0),0xC7U);
    //Set Sector Flag
    // GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0)+1,0U);
    //Set Sector Number
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(1),0U);
    //Set Page Number
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(2)+u8PageNum,0U);
    //Set Timer
    GD25Q_SPIFLASH_SetWord(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(3)+4*u8WriteCount,0U);
    //Set Write Counter
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8WriteCount,0U);
    //Set Read Counter
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(5)+u8ReadCount,0U);
  }else{
    //Get Sector Flag
    u8SectorFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0)+1);
    //Get Sector Number
    u8SectorNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(1));
    //Get Page Number
    u8PageNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(2)+u8PageNum);
    //Get Timer
    u32Timer = GD25Q_SPIFLASH_GetWord(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(3)+4*u8WriteCount);
    //Get Write Counter
    u8WriteCount = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8WriteCount);
    //Get Read Counter
    u8ReadCount = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8ReadCount);
  }
  
  /*Check Block protection state*/
  uint8_t u8RegsState1,u8RegsState2;
  u8RegsState1 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
  u8RegsState2 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg2);
  if ((u8RegsState1|u8RegsState2) !=0U)
  {
    GD25Q_SPIFLASH_WriteStatusRegister(0U,0U);
  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver is Already Disabel Block Protection\r\n");
  }
}

void FlashApp_WriteNorFlash()
{
  /*Header file,3 Pages*/
  //Erase Flag
  // GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0),u8EraseFlag);
  //Sector Number
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(1),u8SectorNum);
  //Page Number
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(2)+u8PageNum,u8PageNum);
  //Timer
  GD25Q_SPIFLASH_SetWord(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(3)+4*u8WriteCount,TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT));
  //Write Counter
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8WriteCount,u8WriteCount);
  //Read Counter
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(5)+u8ReadCount,u8ReadCount);

  //Write DTC Data,cycle 64 bytes every time
  memset(dataSend,0xFFU,sizeof(dataSend));//Clear Buffer
  memset(dataRecv,0xFFU,sizeof(dataRecv));//Clear Buffer
  
  // Get DTC Information, all 64 bytes
  dataSend[0] = 0x0A;  //Function type
  dataSend[1] = CMD_DISP_STATUS; //Error Code
  dataSend[2] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
  dataSend[3] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1);

  dataSend[4] = 0x0B;  //Function type
  dataSend[5] = CMD_DTC; //Error Code
  for (uint8_t i = 0; i < 41; i++)
  {
    dataSend[6+i] = RegisterApp_DHU_Read(CMD_DTC,CMD_DATA_POS+i);
  }

  // Write DTC information,length cycle 64 byte
  GD25Q_SPIFLASH_WriteBuffer(dataSend, GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(6)+u8WriteCount*u8SendBuffLen,u8SendBuffLen);

  // Read DTC information,length cycle 64 byte
  GD25Q_SPIFLASH_ReadBuffer(dataRecv, GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(6)+u8WriteCount*u8SendBuffLen,u8RecvBuffLen);

  /*Check Head File*/
  //Get Sector Flag
  u8SectorFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0)+1);
  //Get Sector Number
  u8SectorNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(1));
  //Get Page Number
  u8PageNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(2)+u8PageNum);

  /*Check Index File*/
  //Get Timer
  u32Timer = GD25Q_SPIFLASH_GetWord(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(3)+4*u8WriteCount);
  //Get Write Counter
  u8WriteCount = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8WriteCount);
  //Get Read Counter
  u8ReadCount = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(4)+u8ReadCount);

  /*Judge Current Page and Sector Number*/
  if (u8WriteCount % 4 == 0U)
  {
    u8PageNum++;
  }
  if (u8PageNum == 16U)
  {
    u8PageNum = 0;
    u8SectorNum++;
  }
  if (u8SectorNum == 128U)
  {
    u8SectorNum = 0;
    //Set Sector Flag
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorNum)+GD25Q80_PAGE_ADDRESS(0)+1,1U);
  }
  /*Judge if 128 Sector complete,start Erase Sector*/
  if (u8SectorFlag == 1U)
  {
    GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8SectorNum));
  }
  u8WriteCount++;
  u8ReadCount++;
}
