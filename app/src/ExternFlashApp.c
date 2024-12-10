/*
 * ExternFlashApp.c
 *
 *  Created on: 2024年12月9日
 *      Author: ZhixiangQi
 */
#include "app/inc/ExternFlashApp.h"
#include "app/inc/RegisterApp.h"
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/UartDriver.h"

uint8_t u8externFlashSenBuf[64] = {0U};
uint8_t u8externFlashRecBuf[64] = {0U};

uint8_t u8WriteComplete = 0U;
uint8_t u8SectorSN;
uint8_t u8PageSN;
uint8_t u8WriteSN;
uint8_t u8ReadSN;
void ExternFlashApp_Check(void)
{
    // uint8_t u8TxBuffer[60] = {0U};
    /*Check Chip if Erase*/
    uint8_t u8EraseFlag = 0U;
    u8EraseFlag = GD25Q_SPIFLASH_GetByte(GD25Q_SPIFLASH_Use_Address(0U, 0U, 0U));
    if (u8EraseFlag != CHIP_ERASE_FALG)
    {
        GD25Q_SPIFLASH_EraseChip();//Erase Chip
        UartDriver_TxWriteString((uint8_t *)"GD25QDriver Erase Chip Success\r\n");
    }else{
        /*code*/
    }
}
void ExternFlashApp_Write(void)
{
    /*Clear Buffer*/
    (void)memset(u8externFlashSenBuf,0U,sizeof(u8externFlashRecBuf));
    (void)memset(u8externFlashRecBuf,0U,sizeof(u8externFlashRecBuf));
    /*TAIL, 1 byte*/
    if (u8WriteComplete == 0U)
    {
        u8externFlashSenBuf[0] = 0xBBU;
        u8WriteComplete = 0U;
    }else{
        /*nothing*/
    }
    /*HEAD, 1 byte*/
    u8externFlashSenBuf[1] = 0xAAU;
    /*Serial Number, 4 bytes*/
    u8externFlashSenBuf[2] = 0xFFU;
    u8externFlashSenBuf[3] = 0xFFU;
    u8externFlashSenBuf[4] = 0xFFU;
    u8externFlashSenBuf[5] = 0xFFU;
    /*Time Stamp, 8 bytes*/
    u8externFlashSenBuf[6] = 0xFFU;
    u8externFlashSenBuf[7] = 0xFFU;
    u8externFlashSenBuf[8] = 0xFFU;
    u8externFlashSenBuf[9] = 0xFFU;
    u8externFlashSenBuf[10] = 0xFFU;
    u8externFlashSenBuf[11] = 0xFFU;
    u8externFlashSenBuf[12] = 0xFFU;
    u8externFlashSenBuf[13] = 0xFFU;
    /*Length, 1 byte*/
    u8externFlashSenBuf[15] = 0xFFU;
    /*DTC Information, 45 bytes*/
    //CMD DISP_STATE 0x00 4 bytes
    u8externFlashSenBuf[16] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS);
    u8externFlashSenBuf[17] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+1U);
    u8externFlashSenBuf[18] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+2U);
    u8externFlashSenBuf[19] = RegisterApp_DHU_Read(CMD_DISP_STATUS,CMD_DATA_POS+3U);
    //CMD DTC 0xA3 41 bytes
    for (uint8_t i = 0U; i < 41U; i++)
    {
      u8externFlashSenBuf[20U+i] = RegisterApp_DHU_Read(CMD_DTC,CMD_DATA_POS+i);
    }
    /*CRC, 2 bytes*/
    u8externFlashSenBuf[61] = 0xFFU;
    u8externFlashSenBuf[62] = 0xFFU;
    /*NA, 1 byte*/
    u8externFlashSenBuf[63] = 0xFFU;
    GD25Q_SPIFLASH_WriteBuffer(u8externFlashSenBuf,GD25Q_SPIFLASH_Use_Address(u8SectorSN, u8PageSN, u8WriteSN*0x40U), 64U);
    u8WriteComplete = 1U;
}
/* *****************************************************************************
 End of File
 */
