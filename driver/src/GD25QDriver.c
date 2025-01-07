/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"

u8 sendBuffer[GD25Q80_PAGE_BYTE_SIZE]={0x00};
u8 recvBuffer[GD25Q80_PAGE_BYTE_SIZE]={0x00};
//LDRA_EXCLUDE_START 496 S
GD25Q_StatusTypeDef GD25QStatus = GD25Q80CSIG_OK;
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_Init(void)
 @Description			GD25Q_SPIFLASH_Init			: GD25Q SPIFLASH 初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_Init(void)
{
	u32 ReadIdentificationID;
	GD25QStatus = GD25Q80CSIG_OK;
	
	GD25Q_SPIFLASH_WakeUp();
	ReadIdentificationID = GD25Q_SPIFLASH_ReadIdentificationID();
	if (ReadIdentificationID != GD25Q80CSIGIdentificationID) {
		GD25QStatus = GD25Q80CSIG_ERROR;
	}
}
 
/**********************************************************************************************************
 @Function			GD25Q_StatusTypeDef GD25Q_SPIFLASH_Get_Status(void)
 @Description			GD25Q_SPIFLASH_Get_Status	: GD25Q SPIFLASH 状态
 @Input				void
 @Return				GD25Q_StatusTypeDef
**********************************************************************************************************/
GD25Q_StatusTypeDef GD25Q_SPIFLASH_Get_Status(void)
{
#ifdef	GD25Q_80CSIG
	return GD25QStatus;
#else
	return GD25Q80CSIG_ERROR;
#endif
}
 
/**********************************************************************************************************
 @Function			u8 GD25Q_SPI_FLASH_ReadByte(void)
 @Description			GD25Q_SPI_FLASH_ReadByte		: SPI FLASH 读取一个字节数据
 @Input				void
 @Return				readByte					: 读取到的数据
**********************************************************************************************************/
u8 GD25Q_SPI_FLASH_ReadByte(void)
{
	u8 readByte, sendByte = GD25Q_Dummy_Byte;
	
	if (SPIMDriver_Transfer(&sendByte, &readByte, 1) != CY_SCB_SPI_SUCCESS) {
		readByte = GD25Q_Dummy_Byte;
	}
	return readByte;
}
 
/**********************************************************************************************************
 @Function			u8 GD25Q_SPI_FLASH_SendByte(u8 byte)
 @Description			GD25Q_SPI_FLASH_SendByte		: SPI FLASH 发送一个字节数据
 @Input				byte						: 发送的数据
 @Return				void
**********************************************************************************************************/
u8 GD25Q_SPI_FLASH_SendByte(u8 byte)
{
	u8 readByte = 0x00U, sendByte = byte;
	
	if (SPIMDriver_Transfer(&sendByte, &readByte, 1) != CY_SCB_SPI_SUCCESS) {
		readByte = GD25Q_Dummy_Byte;
	}
	return readByte;
}

/**********************************************************************************************************
 @Function			u32 GD25Q_SPIFLASH_ReadDeviceID(void)
 @Description			GD25Q_SPIFLASH_ReadDeviceID			: GD25Q SPIFLASH 读取设备ID
 @Input				void
 @Return				设备ID
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_ReadDeviceID(void)
{
	u32 deviceID = 0U;
	
  sendBuffer[0] = GD25Q_DeviceID;
  sendBuffer[1] = GD25Q_Dummy_Byte;
  sendBuffer[2] = GD25Q_Dummy_Byte;
  sendBuffer[3] = GD25Q_Dummy_Byte;
  sendBuffer[4] = GD25Q_Dummy_Byte;
  if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 5) == CY_SCB_SPI_SUCCESS)
  {
    deviceID = recvBuffer[4];
  }else{
		deviceID = GD25Q_Dummy_Byte;
  }
	
	return deviceID;
}
 
/**********************************************************************************************************
 @Function			u32 GD25Q_SPIFLASH_ReadManufactureID(void)
 @Description			GD25Q_SPIFLASH_ReadManufactureID		: GD25Q SPIFLASH 读取厂商ID
 @Input				void
 @Return				厂商ID
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_ReadManufactureID(void)
{
	u32 manufactureID = 0U;
	
	sendBuffer[0] = GD25Q_ManufactDeviceID;
	sendBuffer[1] = 0x00U;
	sendBuffer[2] = 0x00U;
	sendBuffer[3] = 0x00U;
	sendBuffer[4] = GD25Q_Dummy_Byte;
	sendBuffer[5] = GD25Q_Dummy_Byte;
	if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 6) == CY_SCB_SPI_SUCCESS)
	{
		manufactureID |= (uint32_t)recvBuffer[4] << 8;
		manufactureID |= recvBuffer[5];
	}else{
		manufactureID |= GD25Q_Dummy_Byte << 8;
		manufactureID |= GD25Q_Dummy_Byte;
	}
		
	return manufactureID;
}
 
/**********************************************************************************************************
 @Function			u32 GD25Q_SPIFLASH_ReadIdentificationID(void)
 @Description			GD25Q_SPIFLASH_ReadIdentificationID	: GD25Q SPIFLASH 读取型号ID
 @Input				void
 @Return				型号ID
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_ReadIdentificationID(void)
{
	u32 identificationID = 0U;
	
	sendBuffer[0] = GD25Q_JedecDeviceID;
	sendBuffer[1] = GD25Q_Dummy_Byte;
	sendBuffer[2] = GD25Q_Dummy_Byte;
	sendBuffer[3] = GD25Q_Dummy_Byte;
	if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4) == CY_SCB_SPI_SUCCESS)
	{
		identificationID = ((uint32_t)recvBuffer[1] << 16) | ((uint32_t)recvBuffer[2] << 8) | recvBuffer[3];
	}else{
		identificationID = (GD25Q_Dummy_Byte << 16) | (GD25Q_Dummy_Byte << 8) | GD25Q_Dummy_Byte;
	}
	
	return identificationID;
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WriteEnable(void)
 @Description			GD25Q_SPIFLASH_WriteEnable	: GD25Q SPIFLASH 写使能
 @Input				void
 @Return				void
 @attention			WEL置位
**********************************************************************************************************/
void GD25Q_SPIFLASH_WriteEnable(void)
{
	(void)GD25Q_SPI_FLASH_SendByte(GD25Q_WriteEnable);
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WriteDisable(void)
 @Description			GD25Q_SPIFLASH_WriteDisable	: GD25Q SPIFLASH 写禁止
 @Input				void
 @Return				void
 @attention			WEL清零
**********************************************************************************************************/
void GD25Q_SPIFLASH_WriteDisable(void)
{
	(void)GD25Q_SPI_FLASH_SendByte(GD25Q_WriteDisable);
}
 
/**********************************************************************************************************
 @Function			u8 GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister)
 @Description			GD25Q_SPIFLASH_ReadStatusRegister	: GD25Q SPIFLASH 读状态寄存器
 @Input				状态寄存器地址
 @Return				状态寄存器值
**********************************************************************************************************/
u8 GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister)
{
	u8 readByte = 0U;
	
	sendBuffer[0] = statusRegister;
	sendBuffer[1] = GD25Q_Dummy_Byte;
	if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 2) == CY_SCB_SPI_SUCCESS)
	{
		readByte = recvBuffer[1];
	}else{
		readByte = GD25Q_Dummy_Byte;
	}
	
	return readByte;
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WriteStatusRegister(u8 srLow, u8 srHigh)
 @Description			GD25Q_SPIFLASH_WriteStatusRegister	: GD25Q SPIFLASH 写状态寄存器
 @Input				srLow						: 状态寄存器低字节
					srHigh						: 状态寄存器高字节
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_WriteStatusRegister(u8 srLow, u8 srHigh)
{
  sendBuffer[0] = GD25Q_WriteStatusReg;
  sendBuffer[1] = srLow;
  sendBuffer[2] = srHigh;
  (void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 3U);
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WaitForBusy(void)
 @Description			GD25Q_SPIFLASH_WaitForBusy		: GD25Q SPIFLASH 等待设备空闲
 @Input				void
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_WaitForBusy(void)
{
	u8 flashStatus = 0U;
	
	/* Loop as long as the memory is busy with a write cycle */
	do {
		flashStatus = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
	}
	while ((flashStatus & 0x01U) == 0x01U);
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_PowerDown(void)
 @Description			GD25Q_SPIFLASH_PowerDown			: GD25Q SPIFLASH 进入掉电模式
 @Input				void
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_PowerDown(void)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* Send "Power Down" instruction */
	(void)GD25Q_SPI_FLASH_SendByte(GD25Q_PowerDown);
	
	/* Wait for TDP */
	Cy_SysLib_Delay(3);
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WakeUp(void)
 @Description			GD25Q_SPIFLASH_WakeUp			: GD25Q SPIFLASH 唤醒
 @Input				void
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_WakeUp(void)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* Send "Release Power Down" instruction */
	(void)GD25Q_SPI_FLASH_SendByte(GD25Q_ReleasePowerDown);
	
	/* Wait for TRES1 */
	Cy_SysLib_Delay(5);
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_EraseChip(void)
 @Description			GD25Q_SPIFLASH_EraseChip			: GD25Q SPIFLASH 擦除Chip
 @Input				void
 @Return				void
 @attention			800MS
**********************************************************************************************************/
void GD25Q_SPIFLASH_EraseChip(void)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* 发送FLASH写使能命令 */
	GD25Q_SPIFLASH_WriteEnable();
	
	/* 等待FLASH空闲 */
	GD25Q_SPIFLASH_WaitForBusy();
	
	/* 发送片擦除命令 */
	(void)GD25Q_SPI_FLASH_SendByte(GD25Q_ChipErase);
	
	/* 等待擦除完毕 */
	GD25Q_SPIFLASH_WaitForBusy();
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_EraseBlock(u32 BlockAddr)
 @Description			GD25Q_SPIFLASH_EraseBlock		: GD25Q SPIFLASH 擦除Block
 @Input				BlockAddr						: 块地址(0x10000倍数)
 @Return				void
 @attention			GD25Q80CSIG SPI FLASH 擦除块大小为64KB(65536Byte),即一个块区大小,在写入数据之前要求先擦除空间
					50MS
**********************************************************************************************************/
void GD25Q_SPIFLASH_EraseBlock(u32 BlockAddr)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* 发送FLASH写使能命令 */
	GD25Q_SPIFLASH_WriteEnable();
	
	/* 等待FLASH空闲 */
	GD25Q_SPIFLASH_WaitForBusy();

	sendBuffer[0] = GD25Q_BlockErase;             /* 发送块区擦除命令 */
	sendBuffer[1] = (uint8_t)((BlockAddr & 0xFF0000U) >> 16U); /* 发送块区擦除地址高位 */
	sendBuffer[2] = (uint8_t)((BlockAddr & 0xFF00U) >> 8U);    /* 发送块区擦除地址中位 */
	sendBuffer[3] = (uint8_t)(BlockAddr & 0xFFU);            /* 发送块区擦除地址低位 */
		
	(void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 4);
	
	/* 等待擦除完毕 */
	GD25Q_SPIFLASH_WaitForBusy();
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_EraseSector(u32 SectorAddr)
 @Description			GD25Q_SPIFLASH_EraseSector		: GD25Q SPIFLASH 擦除Sector
 @Input				SectorAddr					: 扇区地址(0x1000倍数)
 @Return				void
 @attention			GD25Q80CSIG SPI FLASH 最小擦除块大小为4KB(4096Byte),即一个扇区大小,在写入数据之前要求先擦除空间
					20MS
**********************************************************************************************************/
void GD25Q_SPIFLASH_EraseSector(u32 SectorAddr)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* 发送FLASH写使能命令 */
	GD25Q_SPIFLASH_WriteEnable();
	
	/* 等待FLASH空闲 */
	GD25Q_SPIFLASH_WaitForBusy();

	sendBuffer[0] = GD25Q_SectorErase;             /* 发送扇区擦除命令 */
	sendBuffer[1] = (uint8_t)((SectorAddr & 0xFF0000U) >> 16U); /* 发送扇区擦除地址高位 */
	sendBuffer[2] = (uint8_t)((SectorAddr & 0xFF00U) >> 8U);    /* 发送扇区擦除地址中位 */
	sendBuffer[3] = (uint8_t)(SectorAddr & 0xFFU);            /* 发送扇区擦除地址低位 */
		
	(void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U);
	
	/* 等待擦除完毕 */
	GD25Q_SPIFLASH_WaitForBusy();
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
 @Description			GD25Q_SPIFLASH_ReadBuffer		: GD25Q SPIFLASH 读取SPI FLASH数据
 @Input				pBuffer						: 数据存储区
					ReadAddr						: 数据读取地址
					NumByteToRead					: 读取数据长度
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	sendBuffer[0] = GD25Q_ReadData;                 /* 发送读指令 */
	sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U); /* 发送块区擦除地址高位 */
	sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);    /* 发送块区擦除地址中位 */
	sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);            /* 发送块区擦除地址低位 */

	for (u16 i = 0U; i < NumByteToRead; i++)
	{
		sendBuffer[4U+i] = GD25Q_Dummy_Byte;
	}
	
	(void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+NumByteToRead);

	/* 读取数据 */
	for (u16 p = 0U; p < NumByteToRead; p++)
	{
		pBuffer[p] = recvBuffer[p+4U];
	}
	
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
 @Description			GD25Q_SPIFLASH_WritePage			: GD25Q SPIFLASH 写入SPI FLASH数据Page
 @Input				pBuffer						: 数据存储区
					WriteAddr						: 数据写入地址
					NumByteToWrite					: 写入数据长度
 @Return				void
 @attention			写入字节数不得超过该页写入剩余字节数
**********************************************************************************************************/
void GD25Q_SPIFLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
#ifdef GD25Q_80CSIG
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	/* 发送FLASH写使能命令 */
	GD25Q_SPIFLASH_WriteEnable();
	
	/* 等待FLASH空闲 */
	GD25Q_SPIFLASH_WaitForBusy();

	sendBuffer[0] = GD25Q_PageProgram;            /* 发送FLASH写使能命令 */
	sendBuffer[1] = (uint8_t)((WriteAddr & 0xFF0000U) >> 16U); /* 发送块区擦除地址高位 */
	sendBuffer[2] = (uint8_t)((WriteAddr & 0xFF00U) >> 8U);    /* 发送块区擦除地址中位 */
	sendBuffer[3] = (uint8_t)(WriteAddr & 0xFFU);            /* 发送块区擦除地址低位 */

	/* 写入数据 */
	for (u16 i = 0U; i < NumByteToWrite; i++)
	{
		sendBuffer[4U+i]=pBuffer[i];
	}

	(void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+NumByteToWrite);
		
	/* 等待写入完毕 */
	GD25Q_SPIFLASH_WaitForBusy();
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WriteBuffer(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
 @Description			GD25Q_SPIFLASH_WriteBuffer		: GD25Q SPIFLASH 写入SPI FLASH数据
 @Input				pBuffer						: 数据存储区
					WriteAddr						: 数据写入地址
					NumByteToWrite					: 写入数据长度
 @Return				void
 @attention			具有自动换页功能
**********************************************************************************************************/
//LDRA_EXCLUDE_START 139 S
void GD25Q_SPIFLASH_WriteBuffer(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
#ifdef GD25Q_80CSIG
	u16 pagereMain;
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) 
	{
		return;
	}
	else
	{
		//Do nothing
	}
	pagereMain = (uint16_t)(GD25Q80_PAGE_BYTE_SIZE - (WriteAddr % GD25Q80_PAGE_BYTE_SIZE));
	if (NumByteToWrite <= pagereMain) 
	{
       pagereMain = NumByteToWrite;
	}
	else
	{
		//Do nothing
	}
	//LDRA_EXCLUDE_START 28 D
	//LDRA_EXCLUDE_START 45 D
	while (true) 
	{
		GD25Q_SPIFLASH_WritePage(pBuffer, WriteAddr, pagereMain);
		if (NumByteToWrite == pagereMain) 
		{
			break;
		}
		else 
		{
			pBuffer += pagereMain;
			WriteAddr += pagereMain;
			
			NumByteToWrite -= pagereMain;
			if (NumByteToWrite > GD25Q80_PAGE_BYTE_SIZE) 
			{
				pagereMain = GD25Q80_PAGE_BYTE_SIZE;
			}
			else 
			{
              pagereMain = NumByteToWrite;
			}
		}
	}
	//LDRA_EXCLUDE_END 28 D
	//LDRA_EXCLUDE_END 45 D
#endif
}
//LDRA_EXCLUDE_END 139 S
/**********************************************************************************************************
 @Function			unsigned char GD25Q_SPIFLASH_GetByte(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetByte						: GD25Q SPIFLASH 读取1个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
u8 GD25Q_SPIFLASH_GetByte(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	u8 val = 0U;
	
	GD25Q_SPIFLASH_ReadBuffer(&val, ReadAddr, 1U);
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned short GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetHalfWord					: GD25Q SPIFLASH 读取2个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
//LDRA_EXCLUDE_START 531 S
u16 GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	u16 val = 0U;
	u8 tmpval[2]={0};
	
	GD25Q_SPIFLASH_ReadBuffer(tmpval, ReadAddr, 2);
	
	val |= (uint16_t)tmpval[1];
	val |= (uint16_t)tmpval[0] << 8;
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned int GD25Q_SPIFLASH_GetWord(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetWord						: GD25Q SPIFLASH 读取4个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_GetWord(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	
	u32 val = 0U;
	u8 tmpval[4]={0};
    
	GD25Q_SPIFLASH_ReadBuffer(tmpval, ReadAddr, 4);
	
	val |= tmpval[0];
	val |= (uint32_t)tmpval[1] << 8;
	val |= (uint32_t)tmpval[2] << 16;
	val |= (uint32_t)tmpval[3] << 24;
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_SetByte(u32 WriteAddr, unsigned char val)
 @Description			GD25Q_SPIFLASH_SetByte						: GD25Q SPIFLASH 写入1个Byte
 @Input				val
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_SetByte(u32 WriteAddr, u8 val)
{
#ifdef GD25Q_80CSIG
	GD25Q_SPIFLASH_WriteBuffer(&val, WriteAddr, 1);
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_SetHalfWord(u32 WriteAddr, unsigned short val)
 @Description			GD25Q_SPIFLASH_SetHalfWord					: GD25Q SPIFLASH 写入2个Byte
 @Input				val
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_SetHalfWord(u32 WriteAddr, u16 val)
{
#ifdef GD25Q_80CSIG
	u8 tmpval[2]={0};
	
	tmpval[0] = (uint8_t)(val & 0xFFU);
	tmpval[1] = (uint8_t)((val >> 8) & 0xFFU);
	
	GD25Q_SPIFLASH_WriteBuffer(tmpval, WriteAddr, 2);
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_SetWord(u32 WriteAddr, unsigned int val)
 @Description			GD25Q_SPIFLASH_SetWord						: GD25Q SPIFLASH 写入4个Byte
 @Input				val
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_SetWord(u32 WriteAddr, u32 val)
{
#ifdef GD25Q_80CSIG
	u8 tmpval[4]={0};
	
	tmpval[0] = (uint8_t)(val & 0xFFU);
	tmpval[1] = (uint8_t)((val >> 8U) & 0xFFU);
	tmpval[2] = (uint8_t)((val >> 16U) & 0xFFU);
	tmpval[3] = (uint8_t)((val >> 24U) & 0xFFU);
	
	GD25Q_SPIFLASH_WriteBuffer(tmpval, WriteAddr, 4);
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned int GD25Q_SPIFLASH_GetNumofByte(u32 ReadAddr, u16 AllNum, u8 ByteVal)
 @Description			GD25Q_SPIFLASH_GetNumofByte					: GD25Q SPIFLASH 读取指定地址中指定字节中有该值的个数
 @Input				ReadAddr									: 读取地址
					AllNum									: 读取字节数
					ByteVal									: 匹配值
 @Return				val
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_GetNumofByte(u32 ReadAddr, u16 AllNum, u8 ByteVal)
{
#ifdef GD25Q_80CSIG
	u32 ReadByteval = 0U;
	
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return 0U;
	}
	
	sendBuffer[0] = GD25Q_ReadData;                 /* 发送读指令 */
	sendBuffer[1] = (uint8_t)((ReadAddr & 0xFF0000U) >> 16U); /* 发送块区擦除地址高位 */
	sendBuffer[2] = (uint8_t)((ReadAddr & 0xFF00U) >> 8U);    /* 发送块区擦除地址中位 */
	sendBuffer[3] = (uint8_t)(ReadAddr & 0xFFU);            /* 发送块区擦除地址低位 */
	
	(void)SPIMDriver_Transfer(sendBuffer, recvBuffer, 4U+AllNum);

	/* 读取数据 */
	for (u16 i = 0U; i < AllNum; i++)
	{
		if (ByteVal == recvBuffer[4U+i]) {
				ReadByteval++;
		}
	}

	return ReadByteval;
#endif
}

/**********************************************************************************************************
 @Function			unsigned int GD25Q_SPIFLASH_Use_Address(u8 nSector, u8 nPage , u8 nPos)
 @Description			GD25Q_SPIFLASH_GetNumofByte					: GD25Q SPIFLASH 设定Ues Address
 @Input				nSector									: Sector Serial Number
					nPage									: Page Serial Number
					nPos									: Pos Serial Number
 @Return				val
**********************************************************************************************************/
u32 GD25Q_SPIFLASH_Use_Address(u8 nSector, u8 nPage , u8 nPos)
{
	u32 u32UseAddress = 0U;
	u32UseAddress = GD25Q80_BASE_ADDR + (GD25Q80_SECTOR_BYTE_SIZE * nSector);
	u32UseAddress += GD25Q80_BASE_ADDR + (GD25Q80_PAGE_BYTE_SIZE * nPage);
	u32UseAddress += nPos;

	return u32UseAddress;
}
/********************************************** END OF FLEE **********************************************/
