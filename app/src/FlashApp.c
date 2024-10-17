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

uint8_t u8writeCnt = 0;
uint8_t u8pageNum = 0;

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

  /*Check Status Register Whether to enable data protection*/
  u8RegisterState = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
  if (u8RegisterState != 0U)
  {
    GD25Q_SPIFLASH_WriteStatusRegister(0x00,0x0C);
  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Read Register is 0\r\n");
  }

  /*Check Erase flag */
  if (GD25Q_SPIFLASH_GetByte(SECOND_PAGE_ADDR + ERASE_FLAG_OFFEST) != ERASE_FLAG)
  {
    GD25Q_SPIFLASH_EraseChip();
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
    /* Initial pageNum,writeCnt Value*/
    GD25Q_SPIFLASH_SetByte(FIRST_PAGE_ADDR + u8pageNum, u8pageNum);
    GD25Q_SPIFLASH_SetByte(SECOND_PAGE_ADDR + WRITECOUNT_OFFEST, u8writeCnt);
  }
}
void FlashApp_WriteNorFlash(void)
{
  uint8_t dataSend[64] = {0xFF};
  uint8_t dataRecv[64] = {0xFF};

  /* Judge Page Number*/
  u8pageNum = GD25Q_SPIFLASH_GetByte(FIRST_PAGE_ADDR+u8pageNum);
  if (u8pageNum < PAGE_SIZE)
  {
    /* Judge Write Counter Number*/
    u8writeCnt = GD25Q_SPIFLASH_GetByte(SECOND_PAGE_ADDR + WRITECOUNT_OFFEST);

    /* Get DTC Information, all 64 bytes*/
    dataSend[0] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
    dataSend[1] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
    dataSend[2] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
    dataSend[3] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);
    dataSend[4] = 0xFF;  //reversed
    dataSend[5] = 0xFF;  //reversed

    dataSend[6] = 0x0A;  //Function type
    dataSend[7] = CMD_DISP_STATUS; //Error Code
    dataSend[8] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
    dataSend[9] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1);

    dataSend[10] = 0x0B;  //Function type
    dataSend[11] = CMD_DTC; //Error Code
    for (uint8_t i = 0; i < 40; i++)
    {
      dataSend[12+i] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+i);
    }

    dataSend[53] = 0xFF;   //reversed
    dataSend[54] = 0xFF;   //reversed

    dataSend[55] = ERASE_FLAG;    
    dataSend[56] = u8writeCnt++;   
    dataSend[57] = 0xFF;

    for (uint8_t i = 0; i < 6; i++)
    {
      dataSend[58+i] = 0xFF;   //reversed
    }
    
    /* Write DTC information,length cycle 64 byte*/
    GD25Q_SPIFLASH_WriteBuffer(dataSend, SECOND_PAGE_ADDR+ u8writeCnt*LOG_LENGTH, LOG_LENGTH);
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Write DTC Success\r\n");

    /* Read DTC information*/
    GD25Q_SPIFLASH_ReadBuffer(dataRecv, SECOND_PAGE_ADDR+ u8writeCnt*LOG_LENGTH, LOG_LENGTH);
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Read DTC Success\r\n");

    /* Check Page Counter*/
    if (u8writeCnt % 4 == 0)
    {
      u8pageNum++;
      GD25Q_SPIFLASH_SetByte(FIRST_PAGE_ADDR+u8pageNum, u8pageNum);
    }

  }else{
    UartDriver_TxWriteString((uint8_t *)"GD25QDriver Write 256 Page Complete\r\n");
    /* Check 256 page write complete */
    if (GD25Q_SPIFLASH_GetHalfWord(SECOND_PAGE_ADDR-1)==0xFFFE)
    {
      /* Read lastest page information */
      GD25Q_SPIFLASH_ReadBuffer(dataRecv,FIRST_PAGE_ADDR+ SECOND_PAGE_ADDR*u8pageNum, PAGE_SIZE);
      /* Erase First Block*/
      GD25Q_SPIFLASH_EraseBlock(FIRST_PAGE_ADDR);
      /* Put lastest page information into Second page*/
      GD25Q_SPIFLASH_WritePage(dataRecv,SECOND_PAGE_ADDR,PAGE_SIZE); 
    }
  }
}
