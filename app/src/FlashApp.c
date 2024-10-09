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

uint8_t u8writeCounter=0;
uint8_t u8pageNum=0;
uint8_t u8EraseFlag=0;
uint32_t u32PageAddr=0x0100;
uint8_t u8norFlashSendBuff[256]={0xFF};
uint8_t u8norFlashRecvBuff[256]={0xFF};

static uint8_t u8TxBuffer[80] = {0};

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
  // memset(u8norFlashSendBuff,0x00,256U);
  // memset(u8norFlashRecvBuff,0x00,256U);

  u8writeCounter=0,u8pageNum=0,u8EraseFlag=0xC7;//initial value
  
  /*Check Status Register Whether to enable data protection*/
  u8RegisterState = GD25QDriver_ReadSR();
  if (u8RegisterState != 0U)
  {
    GD25QDriver_WriteSR(0U);
  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Read Register is 0\r\n");
  }
  /*Check Erase flag */
  u8EraseState = GD25QDriver_ReadData(FIRST_PAGE_ADDR,1U,u8norFlashRecvBuff);
  if (u8EraseState)
  {
    if (u8norFlashRecvBuff[0] != 0xC7)
    {
      GD25QDriver_EraseChip();
    }else{
      GD25QDriver_ReadData(FIRST_PAGE_ADDR,3U,u8norFlashRecvBuff);
      sprintf((char *)u8TxBuffer,"GD25QDriver 0x000B Register: 0x%02x,0x%02x,0x%02x\r\n",u8norFlashRecvBuff[0],u8norFlashRecvBuff[1],u8norFlashRecvBuff[2]);
      UartDriver_TxWriteString(u8TxBuffer);
    }
  }
}

void FlashApp_WriteNorFlash(void)
{
  uint8_t u8result;
  uint8_t dataStr[64] = {0};

  // memset(u8norFlashSendBuff,0x00,256);
  // memset(u8norFlashRecvBuff,0x00,256);
  
  if (GD25QDriver_ReadData(FIRST_PAGE_ADDR,3U,u8norFlashRecvBuff))
  {
    u8EraseFlag = u8norFlashRecvBuff[0];
    u8writeCounter = u8norFlashRecvBuff[1];
    u8pageNum = u8norFlashRecvBuff[2];
    u32PageAddr = u32PageAddr+u8writeCounter*0x40;
  }
  
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

  for (uint8_t i = 0; i < 11; i++)
  {
    dataStr[53+i] = 0xFF;   //reversed
  }
  
  u8result = GD25QDriver_Program(u32PageAddr,sizeof(dataStr),dataStr);
  if (u8result)
  {
    if(GD25QDriver_ReadData(u32PageAddr,sizeof(dataStr),u8norFlashRecvBuff))
    {
      u8norFlashSendBuff[0] = u8EraseFlag;
      u8norFlashSendBuff[1] = u8writeCounter+1;;
      u8norFlashSendBuff[2] = u8pageNum+1;

      if(GD25QDriver_Program(FIRST_PAGE_ADDR,3U,u8norFlashSendBuff))
      {
        if(GD25QDriver_ReadData(FIRST_PAGE_ADDR,3U,u8norFlashRecvBuff))
        {
          sprintf((char *)u8TxBuffer,"GD25QDriver 0x000B Register: 0x%02x,0x%02x,0x%02x\r\n",u8norFlashRecvBuff[0],u8norFlashRecvBuff[1],u8norFlashRecvBuff[2]);
          UartDriver_TxWriteString(u8TxBuffer);
        }
      }
    }
  }
}
