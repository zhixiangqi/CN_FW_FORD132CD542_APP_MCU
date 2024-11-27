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
uint8_t u8WriteSerialNum;
uint8_t u8SReadSerialNum;
uint8_t u8RegsState1,u8RegsState2;
uint8_t u8CheckNorFlashFlag;

uint8_t u8testCount = 0;
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

void FlashApp_CheckNorFlash()
{
  uint8_t u8TxBuffer[60] = {0};
  /*Initial Check Nor Flash Complete Flag*/
  u8CheckNorFlashFlag = 0x00U;
  /*Check Chip if Erase*/
  u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
  if (u8EraseFlag == 0xFFU)
  {
    u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(127)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
  }else{
    /*Do nothing*/
  }
  
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
  u8testCount = 0;
  while (u8SectorWriteFlag != 0xFFU)
  {
    u8SectorWriteFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testCount)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3));
    /*Get Index information*/
    u8SectorSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testCount)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0));
    u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testCount)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
    u8SectorWriteCycleFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testCount)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(2));
    u8testCount++;
  }

  /*Get Page Serial Number*/
  u8testCount = 0;
  do
  {
    u8PageSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(u8testCount));
    u8testCount++;
  } while (u8PageSerialNum != 0xFFU);
  /*Get the value to read the previous value of 0xFF*/
  u8PageSerialNum = u8testCount-2;
  sprintf((char *)u8TxBuffer,"PageSerialNum:%02d\r\n",u8PageSerialNum);
  UartDriver_TxWriteString(u8TxBuffer);
  
  /*Get Write Serial Number*/
  u8testCount = 0;
  do
  {
    u8WriteSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8testCount));
    u8testCount++;
  } while (u8WriteSerialNum != 0xFFU);
  /*Get the value to read the previous value of 0xFF*/
  u8WriteSerialNum = u8testCount-2;
  sprintf((char *)u8TxBuffer,"WriteSerialNum:%02d\r\n",u8WriteSerialNum);
  UartDriver_TxWriteString(u8TxBuffer);

  /*Get Write Serial Number*/
  u8testCount = 0;
  do
  {
    u8SReadSerialNum = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8testCount));
    u8testCount++;
  } while (u8SReadSerialNum != 0xFFU);
  /*Get the value to read the previous value of 0xFF*/
  u8SReadSerialNum = u8testCount-2;
  sprintf((char *)u8TxBuffer,"ReadSerialNum:%02d\r\n",u8SReadSerialNum);
  UartDriver_TxWriteString(u8TxBuffer);
  
  /*Check Nor Flash Flag Complete*/
  u8CheckNorFlashFlag = 0x01U;
  sprintf((char *)u8TxBuffer,"Check Nor Flash Complete:%02d\r\n",u8CheckNorFlashFlag);
  UartDriver_TxWriteString(u8TxBuffer);
}

void FlashApp_WriteNorFlash()
{
  if (u8CheckNorFlashFlag == 0x01U)
  {
    uint8_t u8TxBuffer[60] = {0};
    sprintf((char *)u8TxBuffer,"WriteSerialNumber:%02d\r\n",u8WriteSerialNum);
    UartDriver_TxWriteString(u8TxBuffer);
    sprintf((char *)u8TxBuffer,"PageSerialNumber:%02d\r\n",u8PageSerialNum);
    UartDriver_TxWriteString(u8TxBuffer);
    sprintf((char *)u8TxBuffer,"SectorSerialNumber:%02d\r\n",u8SectorSerialNum);
    UartDriver_TxWriteString(u8TxBuffer);
    sprintf((char *)u8TxBuffer,"SectorWriteCycleFlag:%02d\r\n",u8SectorWriteCycleFlag);
    UartDriver_TxWriteString(u8TxBuffer);
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
    if (u8WriteSerialNum % 4 == 0U)//Complete
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
      GD25Q_SPIFLASH_WriteBuffer(dataSend,GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(4+u8PageSerialNum)+POS_NUM(u8WriteSerialNum)*0x40U,u8SendBuffLen);
    }
    /*Judge Current Sector if comolete*/
    if (u8PageSerialNum == 12U)
    {
      /*Initial WriteSerialNumber*/
      u8WriteSerialNum = 0;
      /*Initial ReadSerialNumber*/
      u8SReadSerialNum = 0;
      /*Initial PageSerialNumber*/
      u8PageSerialNum = 0;
      /*Set Sector Write Complete Flag into current sector*/
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0),SECTOR_COMPLETE);
      u8SectorSerialNum++;
      /*Judeg Sector Erase start flag*/
      if (u8SectorSerialNum == 127U && u8SectorWriteCycleFlag == 0xFFU)//Test
      {
        /*Initial SectorSerialNumber*/
        u8SectorSerialNum = 0;
        /*Erase 0 sector*/
        GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum));
        UartDriver_TxWriteString((uint8_t *)"GD25QDriver First Erase Sector Success\r\n");
        /*Set Sector Write Cycle Flag*/
        u8SectorWriteCycleFlag = 1U;
        for (uint8_t i = 0; i < 128; i++)
        {
          GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3),u8SectorWriteCycleFlag);
        }
      } 
      /*Satrt cycle Erase sector*/
      else if (u8SectorWriteCycleFlag == 1U)
      {
        GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum));
        sprintf((char *)u8TxBuffer,"GD25QDriver Cycle Erase Sector Success:%02d\r\n",u8SectorSerialNum);
        UartDriver_TxWriteString(u8TxBuffer);
      }
    }else{
      /*Nothing*/
    }
    
    /*DTC trigger,Write Serial Number add 1*/
    u8WriteSerialNum++;
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8WriteSerialNum),u8WriteSerialNum);

    /*DTC trigger,Read Serial Number add 1*/
    u8SReadSerialNum++;
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8SectorSerialNum)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8SReadSerialNum),u8SReadSerialNum);
  }
}
