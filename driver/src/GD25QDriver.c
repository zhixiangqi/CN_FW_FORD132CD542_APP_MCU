/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"

uint8_t u8DeviceId;
uint8_t u8SendBuffer[256];
uint8_t u8RecvBuffer[256];
uint16_t u16BufferSize;

/* Release power down / Device ID */  
uint8_t GD25QDriver_GetDeviceId(void)
{
  uint8_t u8spiStatus;
  uint8_t u8result;

  u8SendBuffer[0]=GD25Q80E_DEVICE_ID;
  /* Read 3 dummy bytes */
  u8SendBuffer[1]=0xFF;
  u8SendBuffer[2]=0xFF;
  u8SendBuffer[3]=0xFF;
  /*receive buffer*/
  u8SendBuffer[4]=0xFF;
  /*initial recvive buffer*/
  for (size_t i = 0; i < 5; i++)
  {
   u8RecvBuffer[i]=0xFF;
  }
  /*set length*/
  u16BufferSize=5U;
  u8spiStatus=SPIMDriver_Transfer(u8SendBuffer,u8RecvBuffer,u16BufferSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u8result = u8RecvBuffer[u16BufferSize-1];
  }else{
    u8result=0xFF;
  }

  return u8result;
}

void GD25QDriver_SetUp(void)
{
  u8DeviceId = 0;
  while(1)
  {
    if(u8DeviceId == GD25Q80E_DEVICE_ID_RES)
    break;

    u8DeviceId = GD25QDriver_GetDeviceId();
  }
}

uint8_t GD25QDriver_ReadSR(void)
{
  uint8_t u8spiStatus;
  uint8_t u8result;

  u8SendBuffer[0]=GD25Q80E_READ_STATUS1;
  u8SendBuffer[1]=0xFF;
  /*initial recvive buffer*/
  for (size_t i = 0; i < 2; i++)
  {
   u8RecvBuffer[i]=0xFF;
  }
  /*set length*/
  u16BufferSize=2U;
  u8spiStatus=SPIMDriver_Transfer(u8SendBuffer,u8RecvBuffer,u16BufferSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u8result = u8RecvBuffer[u16BufferSize-1];
  }else{
    u8result=0xFF;
  }

  return u8result;
}
void GD25QDriver_WaitWhileBusy(void)
{
  while (GD25QDriver_ReadSR() & GD25Q80E_STATUS_BUSY);
}
uint8_t GD25QDriver_WriteSR(uint8_t u8sr)
{
  uint8_t u8result;
  uint8_t u8spiStatus;

  u8result=GD25QDriver_WriteEnable();
  if (u8result)
  {
    u8SendBuffer[0]=GD25Q80E_WRITE_STATUS;
    u8SendBuffer[1]=u8sr;
    /*set length*/
    u16BufferSize=2U;
    u8spiStatus = SPIMDriver_Transfer(u8SendBuffer,NULL,u16BufferSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  
  GD25QDriver_WaitWhileBusy();

  return u8result;
}

uint8_t GD25QDriver_WriteEnable(void)
{
  GD25QDriver_WaitWhileBusy();
  
  uint8_t u8result;
  uint8_t u8spiStatus;

  u8SendBuffer[0]=GD25Q80E_WRITE_ENABLE;
  /*set length*/
  u16BufferSize=1U;
  u8spiStatus = SPIMDriver_Transfer(u8SendBuffer,NULL,u16BufferSize);
  if (u8spiStatus == CY_SCB_SPI_SUCCESS)
  {
    u8result = TRUE;
  }else{
    u8result = FALSE;
  }
  
  return u8result;
}

uint8_t GD25QDriver_EraseChip(void)
{
  uint8_t u8result;
  uint8_t u8spiStatus;

  u8DeviceId = 0;
  while(1)
  {
    if(u8DeviceId == GD25Q80E_DEVICE_ID_RES)
    break;

    u8DeviceId = GD25QDriver_GetDeviceId();
  }

  u8result=GD25QDriver_WriteEnable();
  if (u8result)
  {
    u8SendBuffer[0]=GD25Q80E_CHIP_ERASE;
    /*set length*/
    u16BufferSize=1U;
    u8spiStatus = SPIMDriver_Transfer(u8SendBuffer,NULL,u16BufferSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }

  return u8result;
}

/* write up to 256 byte page or partial page */
uint8_t GD25QDriver_PageProgram(const uint32_t addr, const uint16_t len, const uint8_t* data)
{
  uint8_t u8result;
  uint8_t u8spiStatus;

  /* do nothing if asked to write beyond a page boundary */
  if (((addr & 0xFF) + len) > GD25Q80E_PAGE_LEN)
    {
    u8result = FALSE;
  }else{
    u8result = TRUE;
  }

  /* do nothing if we would overflow the flash */
  if (addr > (GD25Q80E_NUM_BYTES - len))
    {
    u8result = FALSE;
  }else{
    u8result = TRUE;
  }
  if (u8result)
  {
    u8result=GD25QDriver_WriteEnable();
    if (u8result)
    {
      u8SendBuffer[0]=GD25Q80E_PAGE_PROGRAM;
      u8SendBuffer[1]=(addr & 0xFF0000) >> 16;
      u8SendBuffer[2]=(addr & 0xFF00) >> 8;
      u8SendBuffer[3]=addr & 0xFF;
      for (uint16_t i = 0; i < len; i++)
      {
        u8SendBuffer[4+i]=data[i];
      }
      /*set length*/
      u16BufferSize=4+len;
      u8spiStatus = SPIMDriver_Transfer(u8SendBuffer,NULL,u16BufferSize);
      if (u8spiStatus == CY_SCB_SPI_SUCCESS)
      {
        u8result = TRUE;
      }else{
        u8result = FALSE;
      }
    }
  }
  return u8result;
}

/* write an arbitrary number of bytes */
uint8_t GD25QDriver_Program(uint32_t addr, uint32_t len, const uint8_t* data)
{
  uint8_t u8result;
  uint16_t first_block_len;

  u8DeviceId = 0;
  while(1)
  {
    if(u8DeviceId == GD25Q80E_DEVICE_ID_RES)
    break;

    u8DeviceId = GD25QDriver_GetDeviceId();
  }
  
  /* do nothing if we would overflow the flash */
  if ((len > GD25Q80E_NUM_BYTES) || (addr > GD25Q80E_NUM_BYTES)|| ((addr + len) > GD25Q80E_NUM_BYTES))
  {
    u8result = FALSE;
  }else{
    u8result = TRUE;
  }
  if (u8result)
  {
    /* handle start not at page boundary */
    first_block_len = GD25Q80E_PAGE_LEN - (addr % GD25Q80E_PAGE_LEN);
    if (len < first_block_len)
      first_block_len = len;
    if (first_block_len) {
      GD25QDriver_PageProgram(addr, first_block_len, data);
      addr += first_block_len;
      data += first_block_len;
      len -= first_block_len;
    }

    /* one page at a time on boundaries */
    while (len >= GD25Q80E_PAGE_LEN) {
      GD25QDriver_PageProgram(addr, GD25Q80E_PAGE_LEN, data);
      addr += GD25Q80E_PAGE_LEN;
      data += GD25Q80E_PAGE_LEN;
      len -= GD25Q80E_PAGE_LEN;
    }

    /* handle end not at page boundary */
    if (len) {
      GD25QDriver_PageProgram(addr, len, data);
    }
  }
  return u8result;
}

/* read data */
uint8_t GD25QDriver_ReadData(uint32_t addr, uint32_t len, uint8_t* const data)
{
  uint8_t u8result;
  uint8_t u8spiStatus;

  // /* do nothing if we would overflow the flash */
  if ((len > GD25Q80E_NUM_BYTES) || (addr > GD25Q80E_NUM_BYTES) || ((addr + len) > GD25Q80E_NUM_BYTES))
  {
    u8result = FALSE;
  }else{
    u8result = TRUE;
  }
  if (u8result)
  {
    GD25QDriver_WaitWhileBusy();

    u8SendBuffer[0] = GD25Q80E_FAST_READ;
    u8SendBuffer[1] = (addr & 0xFF0000) >> 16;
    u8SendBuffer[2] = (addr & 0xFF00) >> 8;
    u8SendBuffer[3] = addr & 0xFF;
    /*initial recvive buffer*/
    for (size_t i = 0; i < len; i++)
    {
    u8RecvBuffer[i]=0xFF;
    }
    /*set length*/
    u16BufferSize=4+len;
    u8spiStatus = SPIMDriver_Transfer(u8SendBuffer,u8RecvBuffer,u16BufferSize);
    if (u8spiStatus == CY_SCB_SPI_SUCCESS)
    {
      for (uint32_t i = 0; i < len; i++)
      {
        data[i] = u8RecvBuffer[i+4];
      }
      u8result = TRUE;
    }else{
      u8result = FALSE;
    }
  }
  
  return u8result;
}