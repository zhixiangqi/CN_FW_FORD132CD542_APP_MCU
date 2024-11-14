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

uint8_t u8SectorWriteFlag;
uint8_t u8EraseFlag;
uint8_t u8SectorWriteCycleFlag;
uint8_t u8CurentSector;
uint8_t u8CurnetPage;
uint8_t u8CurnetWrite;
uint8_t u8CurentRead;
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
  uint8_t u8TxBuffer[60] = {0};
  /*Check Chip if Erase*/
  u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
  if (u8EraseFlag != CHIP_ERASE_FALG)
  {
    GD25Q_SPIFLASH_EraseChip();//Erase Chip
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
    
    /*Initial every sector Index information*/
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(0)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0),SECTOR_UNCOMPLETE);
    for (uint8_t i = 0; i < 128; i++)
    {
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1),CHIP_ERASE_FALG);
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(2),SECTOR_SERIAL_NUM(0));
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3),WRITE_CYCLE_FALG);
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(0),PAGE_SERIAL_NUM(0));
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(0),WRITE_SERIAL_NUM(0));
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(i)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(0),READ_SERIAL_NUM(0));
    }
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
  do
  {
    u8SectorWriteFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0));
    u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(1));
    u8CurentSector = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(2));
    u8SectorWriteCycleFlag = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8testConunt)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3));
    u8testConunt++;
  } while (u8SectorWriteFlag == 0x00U);
  sprintf((char *)u8TxBuffer,"u8SectorWriteFlag%d,u8EraseFlag%d,u8CurentSector%d\r\n",u8SectorWriteFlag,u8EraseFlag,u8CurentSector);
  UartDriver_TxWriteString((uint8_t*)u8TxBuffer);

  /*Check Current Page*/
  u8testConunt = 0;
  do
  {
    u8CurnetPage = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(u8testConunt));
    u8testConunt++;
  } while (u8CurnetPage == 0xFFU);
  sprintf((char *)u8TxBuffer,"u8CurnetPage%d\r\n",u8CurnetPage);
  UartDriver_TxWriteString((uint8_t*)u8TxBuffer);

  /*Check Current Write Counter*/
  u8testConunt = 0;
  do
  {
    u8CurnetWrite = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8testConunt));
    u8testConunt++;
  } while (u8CurnetWrite == 0xFFU);
  sprintf((char *)u8TxBuffer,"u8CurnetWrite%d\r\n",u8CurnetWrite);
  UartDriver_TxWriteString((uint8_t*)u8TxBuffer);

  /*Check Current Read Counter*/
  u8testConunt = 0;
  do
  {
    u8CurentRead = GD25Q_SPIFLASH_GetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8testConunt));
    u8testConunt++;
  } while (u8CurentRead == 0xFFU);
  sprintf((char *)u8TxBuffer,"u8CurentRead%d\r\n",u8CurentRead);
  UartDriver_TxWriteString((uint8_t*)u8TxBuffer);
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
  
  /*Judge Current Page*/
  if (u8CurnetWrite % 4 == 0U)
  {
    u8CurnetPage++;
    // Write DTC information,length cycle 64 byte
    GD25Q_SPIFLASH_WriteBuffer(dataSend,GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(4+u8CurnetPage)+POS_NUM(0),u8SendBuffLen);
    // Read DTC information,length cycle 64 byte
    GD25Q_SPIFLASH_WriteBuffer(dataRecv,GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(4+u8CurnetPage)+POS_NUM(0),u8RecvBuffLen);
  }else{
    // Write DTC information,length cycle 64 byte
    GD25Q_SPIFLASH_WriteBuffer(dataSend,GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(4+u8CurnetPage)+POS_NUM(u8CurnetWrite)*0x40U,u8SendBuffLen);
    // Read DTC information,length cycle 64 byte
    GD25Q_SPIFLASH_WriteBuffer(dataRecv,GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(4+u8CurnetPage)+POS_NUM(u8CurnetWrite)*0x40U,u8RecvBuffLen);
  }

  if (u8CurnetPage == 12U)
  {
    u8CurnetPage = 0U;
    u8SectorWriteFlag = SECTOR_COMPLETE;
    /*Set Sector Write Complete Flag*/
    GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+GD25Q80_PAGE_ADDRESS(0)+POS_NUM(0),u8SectorWriteFlag);
  }

  if (u8SectorWriteFlag)
  {
   u8CurentSector++;
   if (u8CurentSector == 128U)
   {
    if (u8SectorWriteCycleFlag == 0U)
    {
      /*Check SectorWriteCycleFlag*/
      u8SectorWriteCycleFlag = 1U;
      GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(0)+POS_NUM(3),u8SectorWriteCycleFlag);
      /*Erase 0 sector*/
      u8CurentSector = 0;
      GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8CurentSector));
    }else{
      GD25Q_SPIFLASH_EraseSector(GD25Q80_SECTOR_ADDRESS(u8CurentSector));
    }
   }
  }
  if (u8CurnetWrite == 72U)
  {
    u8CurnetWrite = 0;
  }else{
    u8CurnetWrite++;
  }
  
  if (u8CurentRead == 72U)
  {
    u8CurentRead = 0;
  }else{
    u8CurentRead++;
  }
  
  /*Set Current page*/
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(1)+POS_NUM(u8CurnetPage),u8CurnetPage);
  /*Set Current Write Counter*/
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(2)+POS_NUM(u8CurnetWrite),u8CurnetWrite);
  /*Set Current Read Counter*/
  GD25Q_SPIFLASH_SetByte(GD25Q80_SECTOR_ADDRESS(u8CurentSector)+ GD25Q80_PAGE_ADDRESS(3)+POS_NUM(u8CurentRead),u8CurentRead);
}
