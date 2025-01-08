/*
 * ExternFlashApp.c
 *
 *  Created on: 2024年12月9日
 *      Author: ZhixiangQi
 */
#include "app/inc/ExternFlashApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/TC0App.h"
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/UartDriver.h"

uint16_t u16CurLogSN =0U;
uint8_t u8externFlashSenBuf[64] = {0U};
uint8_t u8externFlashRecBuf[64] = {0U};

static uint8_t u8WriteComplete = 0U;
static uint8_t u8WriteCycleFlag = 0xFFU;
static uint8_t u8SectorSN = 0U;
static uint8_t u8PageSN = 0U;
static uint8_t u8WriteSN = 0U;
static uint8_t u8ReadSN = 0U;
static uint8_t u8RegSt1,u8RegSt2;
static uint16_t u16WrittenFlag =0U;
static uint16_t u16WrittenFlag_1 =0U;
static uint16_t u16WrittenFlag_64 =0U;

uint8_t u8ExternFlashBuffer[60] = {0U};
void ExternFlashApp_Verify(void)
{
    /*Verify Chip if Erase*/
    u8SectorSN =0U,u8WriteComplete =0U,
    u8SectorSN =0U,u8PageSN =0U,u8WriteSN =0U,u8ReadSN =0U;
    uint8_t u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q_SPIFLASH_Use_Address(FixedSectorAddr,0U,0U));
    if (u8EraseFlag != CHIP_ERASE_FALG)
    {
        GD25Q_SPIFLASH_EraseChip();//Erase Chip
        UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
        GD25Q_SPIFLASH_SetByte(GD25Q_SPIFLASH_Use_Address(FixedSectorAddr,0U,0U),CHIP_ERASE_FALG);
    }
    else
    {
        /*Verify Write Cycle Flag*/
        u8WriteCycleFlag = GD25Q_SPIFLASH_GetByte(GD25Q_SPIFLASH_Use_Address(FixedSectorAddr,0U,1U));
        /*Check Block protection state*/
        u8RegSt1 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
        u8RegSt2 = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg2);
        if ((u8RegSt1|u8RegSt2) !=0U)
        {
            GD25Q_SPIFLASH_WriteStatusRegister(0U,0U);
        }else{
            UartDriver_TxWriteString((uint8_t *)"GD25QDriver is Already Disabel Block Protection\r\n");
        }
        
        /*Verify data integrity */
        while (u8SectorSN <= 127U)
        {
            u16WrittenFlag_1 = GD25Q_SPIFLASH_GetHalfWord(GD25Q_SPIFLASH_Use_Address(u8SectorSN,0U,0*0x40U));
            u16WrittenFlag_64 = GD25Q_SPIFLASH_GetHalfWord(GD25Q_SPIFLASH_Use_Address(u8SectorSN,15U,0*0x40U));
            if (u16WrittenFlag_1 != Writable && u16WrittenFlag_64 == Writable)//Please indicate that the address is within this sector
            {
                break;
            }
            u8SectorSN++;
        }

        /*Further search and write address*/
        while (u8PageSN <= 15U)
        {
            u16WrittenFlag = GD25Q_SPIFLASH_GetHalfWord(GD25Q_SPIFLASH_Use_Address(u8SectorSN,u8PageSN,u8ReadSN*0x40U));
            if (u16WrittenFlag == Writable)
            {
                break;
            }
            u8ReadSN++;
            if (u8ReadSN % 4 == 0U)
            {
                u8ReadSN =0U;
                u8PageSN++;
            }
        }
        u8WriteSN = u8ReadSN;
        u16CurLogSN = u8SectorSN*0x40U+u8PageSN*4U+u8ReadSN;
        sprintf((char *)u8ExternFlashBuffer,"Sector#:%X Page#:%X Write#:%X Cycle#:%X\r\n",u8SectorSN,u8PageSN,u8WriteSN,u8WriteCycleFlag);
        UartDriver_TxWriteString(u8ExternFlashBuffer);
    }
}
void ExternFlashApp_Write(void)
{
    /*Clear Buffer*/
    (void)memset(u8externFlashSenBuf,0U,sizeof(u8externFlashRecBuf));
    (void)memset(u8externFlashRecBuf,0U,sizeof(u8externFlashRecBuf));
    /*HEAD, 1 byte*/
    u8externFlashSenBuf[1] = 0xAAU;
    /*Serial Number, 4 bytes*/
    u8externFlashSenBuf[2] = 0xFFU;
    u8externFlashSenBuf[3] = 0xFFU;
    u8externFlashSenBuf[4] = 0xFFU;
    u8externFlashSenBuf[5] = 0xFFU;
    /*Time Stamp, 8 bytes*/
    u8externFlashSenBuf[6] = (uint8_t)(TC0App_LogTimerReturn(TIMER_FLASH_LOG_COUNT) >> 24U);
    u8externFlashSenBuf[7] = (uint8_t)(TC0App_LogTimerReturn(TIMER_FLASH_LOG_COUNT) >> 16U);
    u8externFlashSenBuf[8] = (uint8_t)(TC0App_LogTimerReturn(TIMER_FLASH_LOG_COUNT) >> 8U);
    u8externFlashSenBuf[9] = (uint8_t)TC0App_LogTimerReturn(TIMER_FLASH_LOG_COUNT);
    u8externFlashSenBuf[10] = 0xFFU;
    u8externFlashSenBuf[11] = 0xFFU;
    u8externFlashSenBuf[12] = 0xFFU;
    u8externFlashSenBuf[13] = 0xFFU;
    /*Length, 1 byte*/
    u8externFlashSenBuf[14] = 0xFFU;
    /*DTC Information, 45 bytes*/
    //CMD DISP_STATE 0x00 4 bytes
    u8externFlashSenBuf[15] = 0x00U;
    u8externFlashSenBuf[16] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
    u8externFlashSenBuf[17] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1U);
    u8externFlashSenBuf[18] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+2U);
    //CMD DTC 0xA3 41 bytes
    u8externFlashSenBuf[19] = 0xA3U;
    for (uint8_t i = 0U; i < 41U; i++)
    {
      u8externFlashSenBuf[20U+i] = RegisterApp_DHU_Read(CMD_DTC,CMD_DATA_POS+i);
    }
    /*CRC, 2 bytes*/
    u8externFlashSenBuf[61] = 0xFFU;
    u8externFlashSenBuf[62] = 0xFFU;
    /*NA, 1 byte*/
    u8externFlashSenBuf[63] = 0xFFU;
    /*Write One DTC Information*/
    GD25Q_SPIFLASH_WriteBuffer(&u8externFlashSenBuf[1],GD25Q_SPIFLASH_Use_Address(u8SectorSN,u8PageSN,u8WriteSN*0x40U+1U),63U);
    /*Prevent abnormal power outage until the entire logic is completed*/
    u8WriteComplete = 1U;
    /*TAIL, 1 byte*/
    if (u8WriteComplete == 1U)
    {
        u8externFlashSenBuf[0] = 0xBBU;
        GD25Q_SPIFLASH_SetByte(GD25Q_SPIFLASH_Use_Address(u8SectorSN,u8PageSN,u8WriteSN*0x40U),u8externFlashSenBuf[0]);
        /*Only for test*/
        // GD25Q_SPIFLASH_ReadBuffer(u8externFlashRecBuf,GD25Q_SPIFLASH_Use_Address(u8SectorSN,u8PageSN,u8WriteSN*0x40U),64U);
        /*Verify Write Cycle Flag*/
        u8WriteCycleFlag = GD25Q_SPIFLASH_GetByte(GD25Q_SPIFLASH_Use_Address(FixedSectorAddr,0U,1U));
        u8WriteSN++;
        if (u8WriteSN % 4 == 0U)
        {
            u8WriteSN =0U;
            u8PageSN++;
            if (u8PageSN == 16U)
            {
                u8PageSN =0U;
                u8SectorSN++;
                if (u8SectorSN == 128U && u8WriteCycleFlag == 0xFFU)
                {
                    u8SectorSN =0U;
                    GD25Q_SPIFLASH_EraseSector(GD25Q_SPIFLASH_Use_Address(u8SectorSN,0U,0U));
                    GD25Q_SPIFLASH_SetByte(GD25Q_SPIFLASH_Use_Address(FixedSectorAddr,0U,1U),128U);
                }else if(u8WriteCycleFlag == 128U)
                {
                    GD25Q_SPIFLASH_EraseSector(GD25Q_SPIFLASH_Use_Address(u8SectorSN,0U,0U));
                }else{
                    /*code*/
                }   
            }
        }
        u8WriteComplete = 0U;
    }else{
        /*nothing*/
    }
    u16CurLogSN = u8SectorSN*0x40U+u8PageSN*4U+u8WriteSN;
    sprintf((char *)u8ExternFlashBuffer,"Sector#:%X Page#:%X Write#:%X Cycle#:%X\r\n",u8SectorSN,u8PageSN,u8WriteSN,u8WriteCycleFlag);
    UartDriver_TxWriteString(u8ExternFlashBuffer);
}
/* *****************************************************************************
 End of File
 */
