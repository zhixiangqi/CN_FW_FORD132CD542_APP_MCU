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

uint8_t u8SendBuffLen = 64U;
uint8_t u8RecvBuffLen = 64U;

uint8_t u8SectorSerialNum;
uint8_t u8EraseFlag;
uint8_t u8SectorWriteCycleFlag;
uint8_t u8SectorWriteFlag;
uint8_t u8PageSerialNum;
uint8_t u8SWriteSerialNum;
uint8_t u8SReadSerialNum;
uint8_t u8RegsState1,u8RegsState2;

uint8_t u8testConunt = 0;
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
//  uint8_t u8TxBuffer[60] = {0};
  /*Check Chip if Erase*/
  u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
  if (u8EraseFlag != CHIP_ERASE_FALG)
  {
    GD25Q_SPIFLASH_EraseChip();//Erase Chip
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
    
    /*Initial every sector Index information*/
    for (uint8_t i = 0; i < 128; i++)
    {
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0),i);//Sector Serial Number
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1),CHIP_ERASE_FALG);//Erase Chip Flag
    }

    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(0),0U);//Page Serial Number
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(0),0U);//Write Serial Number
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(0),0U);//Read Serial Number
  }
  
  /*Check Block protection state*/
  u8RegsState1 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
  u8RegsState2 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg2);
  if ((u8RegsState1|u8RegsState2) !=0U)
  {
    GD25Q_SPIFLASH_WriteStatusRegister(0U,0U);
  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver is Already Disabel Block Protection\r\n");
  }

  /*Check Sector Write Complete Flag*/
  u8testConunt = 0;
  while (u8SectorWriteFlag == 0U)
  {
    u8SectorWriteFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3));
    u8testConunt++;
  }
  /*Get Index information*/
  u8SectorSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0));
  u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
  u8SectorWriteCycleFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(2));

  /*Get Page Serial Number*/
  u8testConunt = 0;
  while (u8PageSerialNum == 0xFFU)
  {
    u8PageSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(u8testConunt));
    u8testConunt++;
  }
  
  /*Get Write Serial Number*/
  u8testConunt = 0;
  while (u8SWriteSerialNum == 0xFFU)
  {
    u8SWriteSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8testConunt));
    u8testConunt++;
  }

  /*Get Write Serial Number*/
  u8testConunt = 0;
  while (u8SReadSerialNum == 0xFFU)
  {
    u8SReadSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8testConunt));
    u8testConunt++;
  }
}
void FlashApp_WriteNorFlash()
{
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
  /*Judge Current Page if complete*/
  if (u8SWriteSerialNum % 4 == 0U)//Complete
  {
   u8PageSerialNum++;
   //Set Current Page Number into current address
   GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(u8PageSerialNum),u8PageSerialNum);
   //Write DTC information,length cycle 64 byte
   GD25Q_SPIFLASH_WriteBuffer(dataSend,GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(4+u8PageSerialNum)+POS_NUM(0),u8SendBuffLen);
  }
  else//Uncomplete
  {
    // Write DTC information,length cycle 64 byte
    GD25Q_SPIFLASH_WriteBuffer(dataSend,GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(4+u8PageSerialNum)+POS_NUM(u8SWriteSerialNum)*0x40U,u8SendBuffLen);
  }
  /*Judge Current Sector if comolete*/
  if (u8PageSerialNum == 12U)
  {
    /*Set Sector Write Complete Flag into current sector*/
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0),SECTOR_COMPLETE);

    u8SectorSerialNum++;
    /*Judeg Sector Erase start flag*/
    if (u8SectorSerialNum == 4U && u8SectorWriteCycleFlag == 0U)
    {
      /*Erase 0 sector*/
      u8SectorSerialNum = 0;
      GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum));
      /*Set Sector Write Cycle Flag*/
      u8SectorWriteCycleFlag = 1U;
      for (uint8_t i = 0; i < 128; i++)
      {
        GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3),u8SectorWriteCycleFlag);
      }
    }
    /*Satrt cycle Erase sector*/
    if (u8SectorWriteCycleFlag == 1U)
    {
      GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum));
    }
  }else{
    /*Nothing*/
  }

  
  /*DTC trigger,Write Serial Number add 1*/
  u8SWriteSerialNum++;
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8SWriteSerialNum),u8SWriteSerialNum);

  /*DTC trigger,Read Serial Number add 1*/
  u8SReadSerialNum++;
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8SReadSerialNum),u8SReadSerialNum);
}
