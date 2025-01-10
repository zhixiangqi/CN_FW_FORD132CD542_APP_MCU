/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "app/inc/DiagApp.h"
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"
#include "driver/inc/UartDriver.h"

GD25Q_StatusTypeDef GD25QStatus = GD25Q80CSIG_OK;

bool GD25Q_SPIFLASH_Init(void)
{
    bool brstate = false;
	u32 u32ReadIdentificationID[1] = {0U};
	GD25QStatus = GD25Q80CSIG_OK;
	
	if(GD25Q_SPIFLASH_WakeUp())
    {
        brstate = true;
        if (GD25Q_SPIFLASH_ReadIdentificationID(u32ReadIdentificationID))
        {
            brstate = true;
            if (u32ReadIdentificationID[0] != GD25Q80CSIGIdentificationID) {
                GD25QStatus = GD25Q80CSIG_ERROR;
            }
        }else{
            brstate = false;
            GD25QStatus = GD25Q80CSIG_ERROR;
        }
    }else{
        brstate = false;
    }
	return brstate;
}

bool GD25Q_SPIFLASH_ReadByte(u8 *rxBuffer)
{
    bool brstate = false;
    u8 sendByte = GD25Q_Dummy_Byte;

    if (SPIMDriver_Transfer(&sendByte, rxBuffer,1U) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
    }
    return brstate;
}

bool GD25Q_SPIFLASH_SendByte(u8 *txBuffer)
{
    bool bwstate = false;
	u8 readByte = GD25Q_Dummy_Byte;
	
	if (SPIMDriver_Transfer(txBuffer, &readByte, 1U) != CY_SCB_SPI_SUCCESS) {
	   bwstate = false;
    }
    else
    {
        bwstate = true;
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_ReadDeviceID(u8 *deviceID)
{
    bool brstate = false;
    u8 sendBuffer[4+1] = {0U};
    u8 recvBuffer[4+1] = {0U};
    sendBuffer[0] = GD25Q_DeviceID;
    sendBuffer[1] = GD25Q_Dummy_Byte;
    sendBuffer[2] = GD25Q_Dummy_Byte;
    sendBuffer[3] = GD25Q_Dummy_Byte;
    sendBuffer[4] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+1U) == CY_SCB_SPI_SUCCESS)
    {
        *deviceID = recvBuffer[4];
        brstate = true;
    }
    else
    {
        *deviceID = GD25Q_Dummy_Byte;
        brstate = false;
    }
    return brstate;
}

bool GD25Q_SPIFLASH_ReadManufactureID(u16 *manufactureID)
{
    bool brstate = false;
    u8 sendBuffer[4+2] = {0U};
    u8 recvBuffer[4+2] = {0U};
    sendBuffer[0] = GD25Q_ManufactDeviceID;
    sendBuffer[1] = 0x00U;
    sendBuffer[2] = 0x00U;
    sendBuffer[3] = 0x00U;
    sendBuffer[4] = GD25Q_Dummy_Byte;
    sendBuffer[5] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+2U) == CY_SCB_SPI_SUCCESS)
    {
        *manufactureID = ((uint16_t)recvBuffer[4] << 16) | recvBuffer[5];
        brstate = true;
    }
    else
    {
        *manufactureID = (GD25Q_Dummy_Byte << 8) | GD25Q_Dummy_Byte;
        brstate = false;
    }
    return brstate;
}

bool GD25Q_SPIFLASH_ReadIdentificationID(u32 *identificationID)
{
    bool brstate = false;
    u8 sendBuffer[4] = {0U};
    u8 recvBuffer[4] = {0U};
    sendBuffer[0] = GD25Q_JedecDeviceID;
    sendBuffer[1] = GD25Q_Dummy_Byte;
    sendBuffer[2] = GD25Q_Dummy_Byte;
    sendBuffer[3] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U) == CY_SCB_SPI_SUCCESS)
    {
        *identificationID = ((uint32_t)recvBuffer[1] << 16) | ((uint32_t)recvBuffer[2] << 8) | recvBuffer[3];
        brstate = true;
    }
    else
    {
        *identificationID = (GD25Q_Dummy_Byte << 16) | (GD25Q_Dummy_Byte << 8) | GD25Q_Dummy_Byte;
        brstate = false;
    }
    return brstate;
}

bool GD25Q_SPIFLASH_WriteEnable(void)
{
    bool bwstate = false;
    u8 sendBuffer[1] = {GD25Q_WriteEnable};
	if(GD25Q_SPIFLASH_SendByte(sendBuffer))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_WriteDisable(void)
{
    bool bwstate = false;
    u8 sendBuffer[1] = {GD25Q_WriteDisable};
	if(GD25Q_SPIFLASH_SendByte(sendBuffer))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister, u8 *rxBuffer)
{
    bool brstate = false;
    u8 sendBuffer[2] = {0U};
    sendBuffer[0] = statusRegister;
    sendBuffer[1] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, rxBuffer, 2U) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
    }
    return brstate;
}

bool GD25Q_SPIFLASH_WriteStatusRegister(u8 srLow, u8 srHigh){
    bool bwstate = false;
    u8 sendBuffer[3] = {0U};
    u8 recvBuffer[3] = {0U};
    sendBuffer[0] = GD25Q_WriteStatusReg;
    sendBuffer[1] = srLow;
    sendBuffer[2] = srHigh;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 3U) != CY_SCB_SPI_SUCCESS) {
        bwstate = false;
    }
    else
    {
        bwstate = true;
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_WaitForBusy()
{
    bool brstate = false;
    u8 statusRegister[2] = {0U};
    u8 timeout = 100U;
    do
    {
        if(GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1, statusRegister))
        {
            brstate = true;
        }
        else
        {
            brstate = false;
        }
        timeout--;
    }while (((statusRegister[1] & 0x01U) == 0x01U) && (timeout > 0U));

    if ((statusRegister[1] & 0x01U) == 0x00U)
    {
        brstate = true;
    }else{
        brstate = false;
    }
    
    return brstate;
}

bool GD25Q_SPIFLASH_PowerDown(void)
{
    bool bwstate = false;
    u8 sendBuffer[1] = {GD25Q_PowerDown};
    if(GD25Q_SPIFLASH_SendByte(sendBuffer))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_WakeUp(void)
{
    bool bwstate = false;
    u8 sendBuffer[1] = {GD25Q_ReleasePowerDown};
    if(GD25Q_SPIFLASH_SendByte(sendBuffer))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    /* Wait for TRES1 */
	Cy_SysLib_Delay(5);
    return bwstate;
}

bool GD25Q_SPIFLASH_EraseChip(void)
{
    bool bwstate = false;
    u8 u8debounce = 3U;
    u8 sendBuffer[1] = {GD25Q_ChipErase};
    if(GD25Q_SPIFLASH_WriteEnable() == false)
    {
        bwstate = false;
    }
    else
    {
        bwstate = true;
        do
        {
            bwstate = GD25Q_SPIFLASH_WaitForBusy();
            u8debounce--;
        } while ((bwstate == false) && (u8debounce > 0U));

        if (bwstate == true)
        {
            if(GD25Q_SPIFLASH_SendByte(sendBuffer) == false)
            {
                bwstate = false;
            }
            else
            {
                bwstate = true;
                u8debounce = 200U;
                do
                {
                    bwstate = GD25Q_SPIFLASH_WaitForBusy();
                    u8debounce--;
                } while ((bwstate == false) && (u8debounce > 0U));
                if (bwstate == false)
                {
                    UartDriver_TxWriteString((uint8_t *)"EraseChip No Complete!\r\n");
                }
            }
        }else{
            UartDriver_TxWriteString((uint8_t *)"Wait Flash fault happen!\r\n");
        }
    }
    if (bwstate == false)
    {
        DiagApp_FlashFaultCheck(true ,DIAG_FLASH_SPIINT_MASK);
        Cy_SCB_SPI_DeInit(SPI0M_MCU_HW);
        (void)SPIMDriver_Initialize();
    }

    return bwstate;
}

bool GD25Q_SPIFLASH_EraseBlock(u32 BlockAddr)
{
    bool bwstate = false;
    u8 u8debounce = 3U;

    if(GD25Q_SPIFLASH_WriteEnable() == false)
    {
        bwstate = false;
    }
    else
    {
        bwstate = true;
        do
        {
            bwstate = GD25Q_SPIFLASH_WaitForBusy();
            u8debounce--;
        } while ((bwstate == false) && (u8debounce > 0U));

        if (bwstate == true)
        {
            u8 sendBuffer[4] = {0U};
            u8 recvBuffer[4] = {0U};
            sendBuffer[0] = GD25Q_BlockErase;
            sendBuffer[1] = (uint8_t)((BlockAddr & 0xFF0000U) >> 16U);
            sendBuffer[2] = (uint8_t)((BlockAddr & 0xFF00U) >> 8U);
            sendBuffer[3] = (uint8_t)(BlockAddr & 0xFFU);
            if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U) != CY_SCB_SPI_SUCCESS) {
                bwstate = false;
            }
            else
            {
                bwstate = true;
                u8debounce = 50U;
                do
                {
                    bwstate = GD25Q_SPIFLASH_WaitForBusy();
                    u8debounce--;
                } while ((bwstate == false) && (u8debounce > 0U));
                if (bwstate == false)
                {
                    UartDriver_TxWriteString((uint8_t *)"EraseBlock No Complete!\r\n");
                }
            }
        }else{
            UartDriver_TxWriteString((uint8_t *)"Wait Flash fault happen!\r\n");
        }
    }
    if (bwstate == false)
    {
        DiagApp_FlashFaultCheck(true ,DIAG_FLASH_SPIINT_MASK);
        Cy_SCB_SPI_DeInit(SPI0M_MCU_HW);
        (void)SPIMDriver_Initialize();
    }
    
    return bwstate;
}

bool GD25Q_SPIFLASH_EraseSector(u32 SectorAddr)
{
    bool bwstate = false;
    u8 u8debounce = 3U;

    if(GD25Q_SPIFLASH_WriteEnable() == false)
    {
        bwstate = false;
    }
    else
    {
        bwstate = true;
        do
        {
            bwstate = GD25Q_SPIFLASH_WaitForBusy();
            u8debounce--;
        } while ((bwstate == false) && (u8debounce > 0U));

        if (bwstate == true)
        {
            u8 sendBuffer[4] = {0U};
            u8 recvBuffer[4] = {0U};
            sendBuffer[0] = GD25Q_SectorErase;
            sendBuffer[1] = (uint8_t)((SectorAddr & 0xFF0000U) >> 16U);
            sendBuffer[2] = (uint8_t)((SectorAddr & 0xFF00U) >> 8U);
            sendBuffer[3] = (uint8_t)(SectorAddr & 0xFFU);
            if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U) != CY_SCB_SPI_SUCCESS) {
                bwstate = false;
            }
            else
            {
                bwstate = true;
                u8debounce = 25U;
                do
                {
                    bwstate = GD25Q_SPIFLASH_WaitForBusy();
                    u8debounce--;
                } while ((bwstate == false) && (u8debounce > 0U));
                if (bwstate == false)
                {
                    UartDriver_TxWriteString((uint8_t *)"EraseSector No Complete!\r\n");
                }
            }
        }else{
            UartDriver_TxWriteString((uint8_t *)"Wait Flash fault happen!\r\n");
        }
    }
    if (bwstate == false)
    {
        DiagApp_FlashFaultCheck(true ,DIAG_FLASH_SPIINT_MASK);
        Cy_SCB_SPI_DeInit(SPI0M_MCU_HW);
        (void)SPIMDriver_Initialize();
    }
    
    return bwstate;
}

bool GD25Q_SPIFLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    bool brstate = false;
    u8 sendBuffer[256] = {0U};
    u8 recvBuffer[256] = {0U};
    sendBuffer[0] = GD25Q_ReadData;
    sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U);
    sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);
    sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+NumByteToRead) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
        for (u16 p = 0U; p < NumByteToRead; p++)
        {
            pBuffer[p] = recvBuffer[p+4U];
        }
    }
    return brstate;
}

bool GD25Q_SPIFLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    bool bwstate = false;
    u8 u8debounce = 3U;

    if(GD25Q_SPIFLASH_WriteEnable() == false)
    {
        bwstate = false;
    }
    else
    {
        bwstate = true;
        do
        {
            bwstate = GD25Q_SPIFLASH_WaitForBusy();
            u8debounce--;
        } while ((bwstate == false) && (u8debounce > 0U));

        if (bwstate == true)
        {
            u8 sendBuffer[256] = {0U};
            u8 recvBuffer[256] = {0U};
            sendBuffer[0] = GD25Q_PageProgram;
            sendBuffer[1] = (uint8_t)((WriteAddr & 0xFF0000U) >> 16U);
            sendBuffer[2] = (uint8_t)((WriteAddr & 0xFF00U) >> 8U);
            sendBuffer[3] = (uint8_t)(WriteAddr & 0xFFU);
            for (u16 p = 0U; p < NumByteToWrite; p++)
            {
                sendBuffer[p+4U] = pBuffer[p];
            }
            if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+NumByteToWrite) != CY_SCB_SPI_SUCCESS) {
                bwstate = false;
            }
            else
            {
                bwstate = true;
                u8debounce = 3U;
                do
                {
                    bwstate = GD25Q_SPIFLASH_WaitForBusy();
                    u8debounce--;
                } while ((bwstate == false) && (u8debounce > 0U));
                if (bwstate == false)
                {
                    UartDriver_TxWriteString((uint8_t *)"Write Flash fault happen!\r\n");
                }
            }
        }else{
            UartDriver_TxWriteString((uint8_t *)"Wait Flash fault happen!\r\n");
        }
    }
    if (bwstate == false)
    {
        DiagApp_FlashFaultCheck(true ,DIAG_FLASH_SPIINT_MASK);
        Cy_SCB_SPI_DeInit(SPI0M_MCU_HW);
        (void)SPIMDriver_Initialize();
    }
    return bwstate;
}

bool GD25Q_SPIFLASH_GetByte(u32 ReadAddr, u8 *rxBuffer)
{
    bool brstate = false;
    u8 sendBuffer[4+1] = {0U};
    u8 recvBuffer[4+1] = {0U};
    sendBuffer[0] = GD25Q_ReadData;
    sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U);
    sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);
    sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);
    sendBuffer[4] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+1U) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
        *rxBuffer = recvBuffer[4U];
    }
    return brstate;
}

bool GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr, u16 *rxBuffer)
{
    bool brstate = false;
    u8 sendBuffer[4+2] = {0U};
    u8 recvBuffer[4+2] = {0U};
    sendBuffer[0] = GD25Q_ReadData;
    sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U);
    sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);
    sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);
    sendBuffer[4] = GD25Q_Dummy_Byte;
    sendBuffer[5] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+2U) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
        *rxBuffer = (u16)(recvBuffer[4U] << 8U) | recvBuffer[5U];
    }
    return brstate;
}

bool GD25Q_SPIFLASH_GetWord(u32 ReadAddr, u32 *rxBuffer)
{
    bool brstate = false;
    u8 sendBuffer[4+4] = {0U};
    u8 recvBuffer[4+4] = {0U};
    sendBuffer[0] = GD25Q_ReadData;
    sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U);
    sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);
    sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);
    sendBuffer[4] = GD25Q_Dummy_Byte;
    sendBuffer[5] = GD25Q_Dummy_Byte;
    sendBuffer[6] = GD25Q_Dummy_Byte;
    sendBuffer[7] = GD25Q_Dummy_Byte;
    if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+4U) != CY_SCB_SPI_SUCCESS) {
        brstate = false;
    }
    else
    {
        brstate = true;
        *rxBuffer = (u32)(recvBuffer[4U] << 24U) | (u32)(recvBuffer[5U] << 16U) | (u32)(recvBuffer[6U] << 8U) | recvBuffer[7U];
    }
    return brstate;
}

bool GD25Q_SPIFLASH_SetByte(u32 WriteAddr, u8 val)
{
    bool bwstate = false;
    if(GD25Q_SPIFLASH_WritePage(&val, WriteAddr, 1U))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    return bwstate;
}
bool GD25Q_SPIFLASH_SetHalfWord(u32 WriteAddr, u16 val)
{
    bool bwstate = false;
    u8 u8value[2] = {0U};
    u8value[0] = (uint8_t)((val & 0xFF00U) >> 8U);
    u8value[1] = (uint8_t)(val & 0xFFU);
    if (GD25Q_SPIFLASH_WritePage(u8value, WriteAddr, 2U))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    
    return bwstate;
}

bool GD25Q_SPIFLASH_SetWord(u32 WriteAddr, u32 val)
{
    bool bwstate = false;
    u8 u8value[4] = {0U};
    u8value[0] = (uint8_t)((val & 0xFF000000U) >> 24U);
    u8value[1] = (uint8_t)((val & 0xFF0000U) >> 16U);
    u8value[2] = (uint8_t)((val & 0xFF00U) >> 8U);
    u8value[3] = (uint8_t)(val & 0xFFU);
    if (GD25Q_SPIFLASH_WritePage(u8value, WriteAddr, 4U))
    {
        bwstate = true;
    }
    else
    {
        bwstate = false;
    }
    return bwstate;
}

u32 GD25Q_SPIFLASH_Use_Address(u8 nSector, u8 nPage , u8 nPos)
{
	u32 u32UseAddress = 0U;
	u32UseAddress = GD25Q80_BASE_ADDR + (GD25Q80_SECTOR_BYTE_SIZE * nSector);
	u32UseAddress += GD25Q80_BASE_ADDR + (GD25Q80_PAGE_BYTE_SIZE * nPage);
	u32UseAddress += nPos;

	return u32UseAddress;
}
// End of File