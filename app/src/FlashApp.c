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

uint8_t dataSend[256] = {0xFF};
uint8_t dataRecv[256] = {0xFF};

uint8_t u8EraseFlag = 0xC7;
uint8_t u8CurrentPageNum = 0;
uint8_t u8CurrentSectorNum = 0;
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
  // //Check Erase State
  // if (GD25Q_SPIFLASH_GetByte(GD25Q80_PAGE_ADDRESS(0)) != GD25Q_ChipErase)
  // {
  //   //Erase Chip
  //   GD25Q_SPIFLASH_EraseChip();
  //   UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");

  //   //Write Erase Flag in current address
  //   GD25Q_SPIFLASH_SetByte(GD25Q80_PAGE_ADDRESS(0),GD25Q_ChipErase);
  //   UartDriver_TxWriteString((uint8_t *)"GD25QDriver Already write Erase Flag\r\n");

  //   //Initial Read and Write Information
  //   GD25Q_SPIFLASH_SetByte(GD25Q80_PAGE_ADDRESS(0)+1,u16ReadHeadInformation);
  //   GD25Q_SPIFLASH_SetByte(GD25Q80_PAGE_ADDRESS(0)+2,u16WriteHeadInformation);
  // }else{
  //   UartDriver_TxWriteString((uint8_t *)"GD25QDriver is Already Erase\r\n");
  // }
  
  // //Check Block protection state
  // uint8_t u8RegsState1,u8RegsState2;
  // u8RegsState1 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
  // u8RegsState2 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg2);
  // if ((u8RegsState1|u8RegsState2) !=0U)
  // {
  //   GD25Q_SPIFLASH_WriteStatusRegister(0U,0U);
  // }else{
  //   UartDriver_TxWriteString((uint8_t *)"GD25QDriver is Already Disabel Block Protection\r\n");
  // }
}

void FlashApp_WriteNorFlash()
{
  // /*Index, One Page*/
  // //Clear Buffer
  // memset(dataSend,0xFFU,sizeof(dataSend));
  // memset(dataRecv,0xFFU,sizeof(dataRecv));

  // dataSend[0] = u8EraseFlag;
  // dataSend[1] = u8CurrentPageNum;
  // dataSend[2] = u8CurrentSectorNum;
  // dataSend[3] = u8CurrentSectorNum;
  // dataSend[4] = u8CurrentSectorNum;
  // dataSend[5] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
  // dataSend[6] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
  // dataSend[7] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
  // dataSend[8] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);

  // for (uint8_t i = 0; i < 249; i++)
  // {
  // dataSend[7+i] = 0xFF; //Reserved
  // }

  // //Write Index information
  // GD25Q_SPIFLASH_WritePage(dataSend,GD25Q80_PAGE_ADDRESS(0),sizeof(dataSend));

  
  // /*Header file,One Page*/
  // //Clear Buffer
  // memset(dataSend,0xFFU,sizeof(dataSend));
  // memset(dataRecv,0xFFU,sizeof(dataRecv));

  // //Get Write Counter
  // //Write Counter
  // dataSend[0+u8WriteCount] = u8WriteCount;
  
   










  // // Get DTC Information, all 64 bytes
  // dataSend[0] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
  // dataSend[1] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
  // dataSend[2] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
  // dataSend[3] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);
  // dataSend[4] = 0xFF;  //reversed
  // dataSend[5] = 0xFF;  //reversed

  // dataSend[6] = 0x0A;  //Function type
  // dataSend[7] = CMD_DISP_STATUS; //Error Code
  // dataSend[8] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
  // dataSend[9] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1);

  // dataSend[10] = 0x0B;  //Function type
  // dataSend[11] = CMD_DTC; //Error Code
  // for (uint8_t i = 0; i < 41; i++)
  // {
  //   dataSend[12+i] = RegisterApp_DHU_Read(CMD_DTC,CMD_DATA_POS+i);
  // }

  // dataSend[53] = 0xFF;   //reversed
  // dataSend[54] = 0xFF;   //reversed

  // dataSend[55] = GD25Q_ChipErase;    
  // dataSend[56] = u32writeNum++;   
  // dataSend[57] = 0xFF;

  // for (uint8_t i = 0; i < 6; i++)
  // {
  //   dataSend[58+i] = 0xFF;   //reversed
  // }

  // // Write DTC information,length cycle 64 byte,from second sector address
  // GD25Q_SPIFLASH_WriteBuffer(dataSend, GD25Q80_PAGE_ADDRESS(16),sizeof(dataSend));

  // //Read DTC information,length cycle 64 byte,from second sector address
  // GD25Q_SPIFLASH_ReadBuffer(dataRecv, GD25Q80_PAGE_ADDRESS(16),sizeof(dataRecv));
  
}
