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

uint8_t u8writeCounter;
uint8_t u8pageNum;

static uint8_t u8TxBuffer[80] = {0};

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

void FlashApp_CheckNorFlash(void)
{
  uint8_t u8RegisterState;
  uint16_t i = 0;
  uint8_t dataSend[256] = {0xFF};
  uint8_t dataRecv[256] = {0xFF};

  /*Check Status Register Whether to enable data protection*/
  u8RegisterState = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
  if (u8RegisterState != 0U)
  {
    GD25Q_SPIFLASH_WriteStatusRegister(0x00,0x0C);
  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Read Register is 0\r\n");
  }

  /* Check Page Counter*/
  GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR,PAGE_SIZE);
	while (i<PAGE_SIZE) {
		if (dataRecv[i] == 0xFF)
    {
      dataSend[i] = i;
      GD25Q_SPIFLASH_SetByte(FIRST_PAGE_ADDR+i, dataSend[i]);
      i++;
      break;
    }else
    {
      i++;
    }
  }
  /* Check 256 page write complete */
  if (GD25Q_SPIFLASH_ReadBuffer(dataRecv,0xFF,1)==0xFF)
  {
    i = 1;
    GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR+(0x0100*0xFF),PAGE_SIZE);
    GD25Q_SPIFLASH_EraseBlock(FIRST_PAGE_ADDR);
    GD25Q_SPIFLASH_WritePage(dataRecv,FIRST_PAGE_ADDR+(0x0100*i)); 
  }
  
  /*Check Erase flag */
  if (GD25Q_SPIFLASH_GetByte(FIRST_PAGE_ADDR+(0x0100*i)) != 0xC7)
  {
    GD25Q_SPIFLASH_EraseChip();
    dataSend[0] = 0xC7;
    dataSend[1] = u8writeCounter++;
    dataSend[2] = u8pageNum++;
    dataSend[3] = 0xFF;
    GD25Q_SPIFLASH_WriteBuffer(dataSend,FIRST_PAGE_ADDR+(0x0100*i),4U);
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Write Register Success\r\n");
    
    GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR+(0x0100*i),4U);
    sprintf((char *)u8TxBuffer,"GD25QDriver 0x0000 Register: 0x%02x,0x%02x,0x%02x,0x%02x\r\n",dataRecv[0],dataRecv[1],dataRecv[2],dataRecv[3]);
    UartDriver_TxWriteString(u8TxBuffer);
  }

}
// void FlashApp_CheckNorFlash(void)
// {
//   uint8_t dataSend[64] = {0xFF};
//   uint8_t dataRecv[64] = {0xFF};
//   uint8_t u8RegisterState;

//   /*Check Status Register Whether to enable data protection*/
//   u8RegisterState = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
//   if (u8RegisterState != 0U)
//   {
//     GD25Q_SPIFLASH_WriteStatusRegister(0x00,0x0C);
//   }else{
//     UartDriver_TxWriteString((uint8_t *)"GD25QDriver Read Register is 0\r\n");
//   }

//   /*Check Erase flag */
//   if (GD25Q_SPIFLASH_GetByte(FIRST_PAGE_ADDR) != 0xC7)
//   {
//     GD25Q_SPIFLASH_EraseChip();

//     u8EraseFlag = 0xC7;
//     dataSend[0] = u8EraseFlag;
//     dataSend[1] = u8writeCounter;
//     dataSend[2] = u8pageNum;
//     GD25Q_SPIFLASH_WriteBuffer(dataSend,FIRST_PAGE_ADDR,3U);
//     UartDriver_TxWriteString((uint8_t *)"GD25QDriver Write Register Success\r\n");
//   }else{
//     GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR,3U);
//     sprintf((char *)u8TxBuffer,"GD25QDriver 0x0000 Register: 0x%02x,0x%02x,0x%02x\r\n",dataRecv[0],dataRecv[1],dataRecv[2]);
//     UartDriver_TxWriteString(u8TxBuffer);
//   }
// }

// void FlashApp_WriteNorFlash(void)
// {
//   uint8_t dataSend[64] = {0xFF};
//   uint8_t dataRecv[64] = {0xFF};
  
//   GD25Q_SPIFLASH_ReadBuffer(dataRecv,u32PageAddr,sizeof(dataRecv));

//   dataSend[0] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
//   dataSend[1] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
//   dataSend[2] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
//   dataSend[3] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);
//   dataSend[4] = 0xFF;  //reversed
//   dataSend[5] = 0xFF;  //reversed

//   dataSend[6] = 0x0A;  //Function type
//   dataSend[7] = CMD_DISP_STATUS; //Error Code
//   dataSend[8] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
//   dataSend[9] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1);

//   dataSend[10] = 0x0B;  //Function type
//   dataSend[11] = CMD_DTC; //Error Code
//   for (uint8_t i = 0; i < 40; i++)
//   {
//     dataSend[12+i] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+i);
//   }

//   for (uint8_t i = 0; i < 11; i++)
//   {
//     dataSend[53+i] = 0xFF;   //reversed
//   }
  
//   GD25Q_SPIFLASH_WriteBuffer(dataSend,u32PageAddr,sizeof(dataSend));

//   GD25Q_SPIFLASH_ReadBuffer(dataRecv,u32PageAddr,sizeof(dataRecv));

//   memset(dataSend,0xFF,sizeof(dataSend));
//   memset(dataRecv,0xFF,sizeof(dataSend));

//   GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR,3U);
//   dataSend[0] = u8EraseFlag;
//   // dataSend[1] = u8writeCounter++;
//   // dataSend[2] = u8pageNum++;
//   dataSend[1] = 1;
//   dataSend[2] = 2;

//   GD25Q_SPIFLASH_WriteBuffer(dataSend,FIRST_PAGE_ADDR,3U);

//   GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR,3U);
//   // sprintf((char *)u8TxBuffer,"GD25QDriver 0x0000 Register: 0x%02x,0x%02x,0x%02x\r\n",dataRecv[0],dataRecv[1],dataRecv[2]);
//   // UartDriver_TxWriteString(u8TxBuffer);
// }
