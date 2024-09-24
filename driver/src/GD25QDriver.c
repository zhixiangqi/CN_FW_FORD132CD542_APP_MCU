/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"

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

uint8_t GD25Q_BUFFER[4096];		 
void GD25QDriver_WriteAppoint(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite)  
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	  
  uint16_t i;    
	uint8_t * GD25Q_BUF;	  
  GD25Q_BUF=GD25Q_BUFFER;	     
 	secpos=u32WriteAddr/4096;//sector address  
	secoff=u32WriteAddr%4096;//Offset within a sector
	secremain=4096-secoff;//Remaining space size of sector   

 	if(u16NumByteToWrite<=secremain)secremain=u16NumByteToWrite;//No more than 4096 bytes
	while(1) 
	{	
		GD25QDriver_ReadData(GD25Q_BUF,secpos*4096,4096);//Read out the content of the entire sector
		for(i=0;i<secremain;i++)//Verify data
		{
			if(GD25Q_BUF[secoff+i]!=0XFF)break;//Need to erase  	  
		}
		if(i<secremain)//Need to erase
		{
			GD25QDriver_SectorErase(secpos);		//erase this sector
			for(i=0;i<secremain;i++)	   		//copy
			{
				GD25Q_BUF[i+secoff]=u8pBuffer[i];	  
			}
			GD25QDriver_PageProgram_NoCheck(GD25Q_BUF,secpos*4096,4096);//Write the entire sector  

		}else GD25QDriver_PageProgram_NoCheck(u8pBuffer,u32WriteAddr,secremain);//Write the erased data directly into the remaining section of the sector 				   
		if(u16NumByteToWrite==secremain)break;//Writing completed
		else//Writing not completed
		{
			secpos++;//Increase sector address by 1
			secoff=0;//The offset position is 0 	 

      u8pBuffer+=secremain;  				//Pointer offset
			u32WriteAddr+=secremain;				//Write address offset	   
      u16NumByteToWrite-=secremain;			//Decreasing byte count
			if(u16NumByteToWrite>4096)secremain=4096;//The next sector still cannot be written
			else secremain=u16NumByteToWrite;		//The next sector can be completed now
		}	 
	};	 
}