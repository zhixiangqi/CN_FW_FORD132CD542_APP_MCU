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
#include "driver/inc/NVMDriver.h"
#include "driver/inc/GD25QDriver.h"

uint8_t u8writeCounter=0;
uint8_t u8pageNum=0;
uint8_t u8EraseFlag=0;
uint32_t u32firstPageAddr=0x0000;

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

void FlashApp_CheckNorFlash(void)
{
  uint8_t u8RegisterState;
  uint8_t u8EraseState;
  uint8_t u8sendBuff[256];
  uint8_t u8recvBuff[256];
  
  /*Check Status Register Whether to enable data protection*/
  u8RegisterState = GD25QDriver_ReadSR();
  if (u8RegisterState != 0U)
  {
    GD25QDriver_WriteSR(0);
  }else{
    /*Nothing*/
  }
  /*Check Erase flag */
  u8EraseState = GD25QDriver_ReadData(u8recvBuff,0x0000,0x01);
  if (u8EraseState)
  {
    if (u8recvBuff[0] == 0x00)
    {
      GD25QDriver_EraseChip();
      u8EraseFlag = 1;//Set Erase Flag is 1;
      u8sendBuff[0] = u8EraseFlag;
      GD25QDriver_WriteData(u8sendBuff,0x0000,0x01);
    }else{
      /* Nothing*/
    }
  }
}

void FlashApp_WriteNorFlash(uint32_t u32PageAddr, uint8_t u8length)
{
  uint8_t dataStr[256] = {0};

  // if (u32PageAddr == )
  // {
  //   /* code */
  // }
  
  dataStr[0] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
  dataStr[1] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
  dataStr[2] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
  dataStr[3] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);
  dataStr[4] = 0xFF;  //reversed
  dataStr[5] = 0xFF;  //reversed

  dataStr[6] = 0x0A;  //Function type
  dataStr[7] = CMD_DISP_STATUS; //Error Code
  dataStr[8] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
  dataStr[9] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1);

  dataStr[10] = 0x0B;  //Function type
  dataStr[11] = CMD_DTC; //Error Code
  for (uint8_t i = 0; i < 40; i++)
  {
    dataStr[12+i] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+i);
  }

  GD25QDriver_WriteData(dataStr,u32PageAddr,64U);
}