/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"

uint8_t u8gd25qBuff[GD25Q_PAGESIZE]={0xFF};

uint8_t GD25QDriver_ReadSR(void)
{
  uint8_t u8result;
  uint8_t u8spiStatus;
  uint8_t u8sendBuff[2]={GD25Q_ReadStatusReg,0xFF};
  uint8_t u8recvBuff[2]={0xFF};
  uint16_t u16buffSize=sizeof(u8sendBuff);
  
  u8spiStatus = SPIMDriver_Transfer(u8sendBuff,u8recvBuff,u16buffSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u8result = u8recvBuff[1];
  }else{
    u8result=0xFF;
  }
  
  return u8result;
}

void GD25QDriver_Wait_Busy(void)   
{   
	while((GD25QDriver_ReadSR()&0x01)==0x01);
}

uint8_t GD25QDriver_WriteEnable(bool benable)
{
  uint8_t u8result;
  uint8_t u8spiStatus;
  uint8_t u8sendBuff[1]={0xFF};
  uint16_t u16buffSize=sizeof(u8sendBuff);
  if (benable)
  {
    u8sendBuff[0]=GD25Q_WriteEnable;
  }
  else
  {
    u8sendBuff[0]=GD25Q_WriteDisable;
  }
  u8spiStatus = SPIMDriver_Transfer(u8sendBuff,NULL,u16buffSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u8result = TRUE;
  }else{
    u8result = FALSE;
  }
  
  return u8result;
}

uint8_t GD25QDriver_WriteSR(uint8_t u8sr)
{
  uint8_t u8result;
  uint8_t u8spiStatus;
  uint8_t u8sendBuff[2]={0xFF};
  uint16_t u16buffSize=sizeof(u8sendBuff);

  u8result=GD25QDriver_WriteEnable(TRUE);
  if (u8result)
  {
    u8sendBuff[0]=GD25Q_WriteStatusReg;
    u8sendBuff[1]=u8sr;
    
    u8spiStatus = SPIMDriver_Transfer(u8sendBuff,NULL,u16buffSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  
  GD25QDriver_Wait_Busy();

  return u8result;
}

uint16_t GD25QDriver_ReadDeviceID(void)
{
  uint16_t u16result=0xFFFF;
  uint8_t u8spiStatus;
  uint8_t u8sendBuff[6]={GD25Q_ManufactDeviceID,0x00,0x00,0x00,0xFF,0xFF};
  uint8_t u8recvBuff[6]={0xFF};
  uint16_t u16buffSize=sizeof(u8sendBuff);
  
  u8spiStatus = SPIMDriver_Transfer(u8sendBuff,u8recvBuff,u16buffSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u16result = (u8recvBuff[4] << 8) | u8recvBuff[5];
  }else{
    u16result=0xFFFF;
  }
  
  return u16result;
}

uint8_t GD25QDriver_ReadData(uint8_t* u8pBuffer,uint32_t u32ReadAddr,uint16_t u16NumByteToRead)   
{ 
  uint8_t u8result;
  uint8_t u8spiStatus;
  uint8_t u8sendBuff[4+256]={0xFF};
  uint8_t u8recvBuff[4+256]={0xFF};
  uint16_t u16buffSize=4+u16NumByteToRead;
  
  u8sendBuff[0]=GD25Q_ReadData;
  u8sendBuff[1] = (u32ReadAddr & 0xFF0000) >> 16;
  u8sendBuff[2] = (u32ReadAddr & 0xFF00) >> 8;
  u8sendBuff[3] = u32ReadAddr & 0xFF;

  u8spiStatus = SPIMDriver_Transfer(u8sendBuff,u8recvBuff,u16buffSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    for (uint16_t i = 0; i < u16NumByteToRead; i++)
    {
      u8pBuffer[i] = u8recvBuff[i+4];
    }
    u8result = TRUE;
  }else{
    u8result = FALSE;
  }
  
  return u8result;
}

uint8_t GD25QDriver_PageProgram(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite)   
{
  uint8_t u8result;

  u8result=GD25QDriver_WriteEnable(TRUE);
  if (u8result)
  {
    uint8_t u8spiStatus;
    uint8_t u8sendBuff[4+256]={0xFF};
    uint8_t u8recvBuff[4+256]={0xFF};
    uint16_t u16buffSize=4+u16NumByteToWrite;

    u8sendBuff[0]=GD25Q_PageProgram;
    u8sendBuff[1] = (u32WriteAddr & 0xFF0000) >> 16;
    u8sendBuff[2] = (u32WriteAddr & 0xFF00) >> 8;
    u8sendBuff[3] = u32WriteAddr & 0xFF;

    for (uint8_t i = 0; i < u16NumByteToWrite; i++)
    {
      u8sendBuff[4+i]=u8pBuffer[i];
    }
    
    u8spiStatus = SPIMDriver_Transfer(u8sendBuff,u8recvBuff,u16buffSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }

  GD25QDriver_Wait_Busy();

  return u8result;
} 

uint8_t GD25QDriver_PageProgram_NoCheck(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite)   
{
  uint8_t u8result;
  uint16_t u16pageremain;
  u16pageremain=256-u32WriteAddr%256; 
  if(u16NumByteToWrite<=u16pageremain)u16pageremain=u16NumByteToWrite;
  while (TRUE)
  {
    u8result=GD25QDriver_PageProgram(u8pBuffer,u32WriteAddr,u16pageremain);
    if(u8result==TRUE)
    {
      if(u16NumByteToWrite==u16pageremain)break;
      else //NumByteToWrite>pageremain
      {
        u8pBuffer+=u16pageremain;
        u32WriteAddr+=u16pageremain;	

        u16NumByteToWrite-=u16pageremain; 
        if(u16NumByteToWrite>256)u16pageremain=256;
        else u16pageremain=u16NumByteToWrite;
      }
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  
  return u8result;
}

uint8_t GD25QDriver_SectorErase(uint32_t u32DstAddr)
{
  uint8_t u8result;

  u8result=GD25QDriver_WriteEnable(TRUE);
  if (u8result)
  {
    uint8_t u8spiStatus;
    uint8_t u8sendBuff[4]={0xFF};
    uint16_t u16buffSize=sizeof(u8sendBuff);

    u32DstAddr*=4096;

    u8sendBuff[0]=GD25Q_SectorErase;
    u8sendBuff[1] = (u32DstAddr & 0xFF0000) >> 16;
    u8sendBuff[2] = (u32DstAddr & 0xFF00) >> 8;
    u8sendBuff[3] = u32DstAddr & 0xFF;

    u8spiStatus = SPIMDriver_Transfer(u8sendBuff,NULL,u16buffSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  Cy_SysLib_Delay(300);
  GD25QDriver_Wait_Busy();

  return u8result;

}

uint8_t GD25QDriver_EraseChip(void)
{
  uint8_t u8result;

  u8result=GD25QDriver_WriteEnable(TRUE);
  if (u8result)
  {
    uint8_t u8spiStatus;
    uint8_t u8sendBuff[1]={GD25Q_ChipErase};
    uint8_t u8recvBuff[1]={0xFF};
    uint16_t u16buffSize=sizeof(u8sendBuff);

    u8spiStatus = SPIMDriver_Transfer(u8sendBuff,u8recvBuff,u16buffSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  Cy_SysLib_Delay(3000);
  GD25QDriver_Wait_Busy();

  return u8result;
}

uint8_t GD25QDriver_WriteSector(uint8_t* pdata, uint32_t addr, uint16_t size)
{
    uint8_t u8result = 0;
    uint16_t page_offset = 0;
    uint16_t page_remain = 0;

    /* Calculate the offset address within the page */
    page_offset = addr%GD25Q_SECTORSIZE;
    /* Calculate the remaining space on the page */
    page_remain = GD25Q_SECTORSIZE - page_offset;
    
    if(size <= page_remain){
        page_remain = size;
    }
    
    while(1)
    {
        u8result = GD25QDriver_PageProgram(pdata, addr, page_remain);
        if(page_remain != size){
            addr += page_remain;
            pdata += page_remain;
            size -= page_remain;
            if(size > GD25Q_SECTORSIZE){
                page_remain = GD25Q_SECTORSIZE;
            }
            else{
                page_remain = size;
            }
        }else{
            break;
        }
    }
    return u8result;
}

uint8_t GD25QDriver_WriteData(uint8_t* pdata, uint32_t address, uint16_t size)
{
    uint8_t u8result = 0;    
    uint32_t sector_pos = 0;
    uint16_t sector_offset = 0;            
    uint16_t sector_remain = 0;
    uint32_t i;
    
    /* sector address */
    sector_pos = address/GD25Q_PAGESIZE;
    /* Calculate the address offset within the sector */
    sector_offset = address%GD25Q_PAGESIZE;
    /* Calculate the remaining space within the sector */
    sector_remain = GD25Q_PAGESIZE - sector_offset;
    
    if(size <= sector_remain){
        sector_remain = size;
    }
    while(1)
    {
        /* Read all data in the current sector */
        GD25QDriver_ReadData(u8gd25qBuff, sector_pos*GD25Q_PAGESIZE, GD25Q_PAGESIZE);
        for(i = 0; i < sector_remain; i++){
            if(u8gd25qBuff[sector_offset + i] != 0xFF)
                break;
        }
        if(i < sector_remain){
            /* Erase the current sector */
            GD25QDriver_SectorErase(sector_pos*GD25Q_PAGESIZE);
            for(i = 0; i < sector_remain; i++){
            	u8gd25qBuff[sector_offset + i] = pdata[i];
            }
            u8result = GD25QDriver_WriteSector(u8gd25qBuff, sector_pos*GD25Q_PAGESIZE, GD25Q_PAGESIZE);
        }else{
            u8result = GD25QDriver_WriteSector(pdata, address, sector_remain);
        }
        
        if(size == sector_remain){
            break;
        }else{
            sector_pos++;
            sector_offset = 0;
            
            pdata += sector_remain;
            address += sector_remain;
            size -= sector_remain;
            if(size > GD25Q_PAGESIZE){
                sector_remain = GD25Q_PAGESIZE;
            }else{
                sector_remain = size;
            }
        }
    }
    
    return u8result;
}
