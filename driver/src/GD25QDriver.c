/*
 * GD25QDriver.c
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */
#include "driver/inc/GD25QDriver.h"
#include "driver/inc/SPIMDriver.h"

u8 sendBuffer[GD25Q80_PAGE_BYTE_SIZE]={0xFF};
u8 recvBuffer[GD25Q80_PAGE_BYTE_SIZE]={0xFF};

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
	if ((ReadIdentificationID != GD25Q80CSIGIdentificationID) && (ReadIdentificationID != GD25Q40BSIGIdentificationID)) {
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
	u8 readByte = 0x00, sendByte = byte;
	
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
	u32 deviceID = 0;
	
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
	u32 manufactureID = 0;
	
  sendBuffer[0] = GD25Q_ManufactDeviceID;
  sendBuffer[1] = 0x00;
  sendBuffer[2] = 0x00;
  sendBuffer[3] = 0x00;
  sendBuffer[4] = GD25Q_Dummy_Byte;
  sendBuffer[5] = GD25Q_Dummy_Byte;
  if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 6) == CY_SCB_SPI_SUCCESS)
  {
    manufactureID |= recvBuffer[4] << 8;
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
	u32 identificationID = 0;
	
  sendBuffer[0] = GD25Q_JedecDeviceID;
  sendBuffer[1] = GD25Q_Dummy_Byte;
  sendBuffer[2] = GD25Q_Dummy_Byte;
  sendBuffer[3] = GD25Q_Dummy_Byte;
  if (SPIMDriver_Transfer(sendBuffer, recvBuffer, 4) == CY_SCB_SPI_SUCCESS)
  {
    identificationID = (recvBuffer[1] << 16) | (recvBuffer[2] << 8) | recvBuffer[3];
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
	GD25Q_SPI_FLASH_SendByte(GD25Q_WriteEnable);
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
	GD25Q_SPI_FLASH_SendByte(GD25Q_WriteDisable);
}
 
/**********************************************************************************************************
 @Function			u8 GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister)
 @Description			GD25Q_SPIFLASH_ReadStatusRegister	: GD25Q SPIFLASH 读状态寄存器
 @Input				状态寄存器地址
 @Return				状态寄存器值
**********************************************************************************************************/
u8 GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister)
{
	u8 readByte = 0;
	
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
  SPIMDriver_Transfer(sendBuffer, recvBuffer, 3);
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_WaitForBusy(void)
 @Description			GD25Q_SPIFLASH_WaitForBusy		: GD25Q SPIFLASH 等待设备空闲
 @Input				void
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_WaitForBusy(void)
{
	u8 flashStatus = 0;
	
	/* Loop as long as the memory is busy with a write cycle */
	do {
		flashStatus = GD25Q_SPIFLASH_ReadStatusRegister(GD25Q_ReadStatusReg1);
	}
	while ((flashStatus & 0x01) == 0x01);
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
	GD25Q_SPI_FLASH_SendByte(GD25Q_PowerDown);
	
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
	GD25Q_SPI_FLASH_SendByte(GD25Q_ReleasePowerDown);
	
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
	GD25Q_SPI_FLASH_SendByte(GD25Q_ChipErase);
	
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
	sendBuffer[1] = (BlockAddr & 0xFF0000) >> 16; /* 发送块区擦除地址高位 */
	sendBuffer[2] = (BlockAddr & 0xFF00) >> 8;    /* 发送块区擦除地址中位 */
	sendBuffer[3] =  BlockAddr & 0xFF;            /* 发送块区擦除地址低位 */
		
	SPIMDriver_Transfer(sendBuffer, recvBuffer, 4);
	
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
	sendBuffer[1] = (SectorAddr & 0xFF0000) >> 16; /* 发送扇区擦除地址高位 */
	sendBuffer[2] = (SectorAddr & 0xFF00) >> 8;    /* 发送扇区擦除地址中位 */
	sendBuffer[3] =  SectorAddr & 0xFF;            /* 发送扇区擦除地址低位 */
		
	SPIMDriver_Transfer(sendBuffer, recvBuffer, 4);
	
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
	sendBuffer[1] = (ReadAddr & 0xFF0000) >> 16; /* 发送块区擦除地址高位 */
	sendBuffer[2] = (ReadAddr & 0xFF00) >> 8;    /* 发送块区擦除地址中位 */
	sendBuffer[3] =  ReadAddr & 0xFF;            /* 发送块区擦除地址低位 */

	for (u16 i = 0; i < NumByteToRead; i++)
	{
		sendBuffer[4+i] = GD25Q_Dummy_Byte;
	}
	
	SPIMDriver_Transfer(sendBuffer, recvBuffer, 4+NumByteToRead);

	/* 读取数据 */
	for (u16 i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = recvBuffer[i+4];
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
	sendBuffer[1] = (WriteAddr & 0xFF0000) >> 16; /* 发送块区擦除地址高位 */
	sendBuffer[2] = (WriteAddr & 0xFF00) >> 8;    /* 发送块区擦除地址中位 */
	sendBuffer[3] =  WriteAddr & 0xFF;            /* 发送块区擦除地址低位 */

	/* 写入数据 */
	for (u16 i = 0; i < NumByteToWrite; i++)
	{
		sendBuffer[4+i]=pBuffer[i];
	}

	SPIMDriver_Transfer(sendBuffer, recvBuffer, 4+NumByteToWrite);
		
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
void GD25Q_SPIFLASH_WriteBuffer(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
#ifdef GD25Q_80CSIG
	u16 pagereMain;
	
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return;
	}
	
	pagereMain = GD25Q80_PAGE_BYTE_SIZE - WriteAddr % GD25Q80_PAGE_BYTE_SIZE;
	if (NumByteToWrite <= pagereMain) pagereMain = NumByteToWrite;
	
	while (true) {
		GD25Q_SPIFLASH_WritePage(pBuffer, WriteAddr, pagereMain);
		if (NumByteToWrite == pagereMain) break;
		else {
			pBuffer += pagereMain;
			WriteAddr += pagereMain;
			
			NumByteToWrite -= pagereMain;
			if (NumByteToWrite > GD25Q80_PAGE_BYTE_SIZE) pagereMain = GD25Q80_PAGE_BYTE_SIZE;
			else pagereMain = NumByteToWrite;
		}
	}
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned char GD25Q_SPIFLASH_GetByte(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetByte						: GD25Q SPIFLASH 读取1个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char GD25Q_SPIFLASH_GetByte(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	unsigned char val = 0;
	
	GD25Q_SPIFLASH_ReadBuffer(&val, ReadAddr, 1);
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned short GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetHalfWord					: GD25Q SPIFLASH 读取2个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned short GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	unsigned short val = 0;
	unsigned char tmpval[2];
	
	GD25Q_SPIFLASH_ReadBuffer(tmpval, ReadAddr, 2);
	
	val |= tmpval[0];
	val |= tmpval[1] << 8;
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			unsigned int GD25Q_SPIFLASH_GetWord(u32 ReadAddr)
 @Description			GD25Q_SPIFLASH_GetWord						: GD25Q SPIFLASH 读取4个Byte
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int GD25Q_SPIFLASH_GetWord(u32 ReadAddr)
{
#ifdef GD25Q_80CSIG
	unsigned int val = 0;
	unsigned char tmpval[4];
	
	GD25Q_SPIFLASH_ReadBuffer(tmpval, ReadAddr, 4);
	
	val |= tmpval[0];
	val |= tmpval[1] << 8;
	val |= tmpval[2] << 16;
	val |= tmpval[3] << 24;
	
	return val;
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_SetByte(u32 WriteAddr, unsigned char val)
 @Description			GD25Q_SPIFLASH_SetByte						: GD25Q SPIFLASH 写入1个Byte
 @Input				val
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_SetByte(u32 WriteAddr, unsigned char val)
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
void GD25Q_SPIFLASH_SetHalfWord(u32 WriteAddr, unsigned short val)
{
#ifdef GD25Q_80CSIG
	unsigned char tmpval[2];
	
	tmpval[0] = val & 0xFF;
	tmpval[1] = (val >> 8) & 0xFF;
	
	GD25Q_SPIFLASH_WriteBuffer(tmpval, WriteAddr, 2);
#endif
}
 
/**********************************************************************************************************
 @Function			void GD25Q_SPIFLASH_SetWord(u32 WriteAddr, unsigned int val)
 @Description			GD25Q_SPIFLASH_SetWord						: GD25Q SPIFLASH 写入4个Byte
 @Input				val
 @Return				void
**********************************************************************************************************/
void GD25Q_SPIFLASH_SetWord(u32 WriteAddr, unsigned int val)
{
#ifdef GD25Q_80CSIG
	unsigned char tmpval[4];
	
	tmpval[0] = val & 0xFF;
	tmpval[1] = (val >> 8) & 0xFF;
	tmpval[2] = (val >> 16) & 0xFF;
	tmpval[3] = (val >> 24) & 0xFF;
	
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
unsigned int GD25Q_SPIFLASH_GetNumofByte(u32 ReadAddr, u16 AllNum, u8 ByteVal)
{
#ifdef GD25Q_80CSIG
	unsigned int ReadByteval = 0;
	
	if (GD25Q80CSIG_OK != GD25Q_SPIFLASH_Get_Status()) {
		return 0;
	}
	
	sendBuffer[0] = GD25Q_ReadData;                 /* 发送读指令 */
	sendBuffer[1] = (ReadAddr & 0xFF0000) >> 16; /* 发送块区擦除地址高位 */
	sendBuffer[2] = (ReadAddr & 0xFF00) >> 8;    /* 发送块区擦除地址中位 */
	sendBuffer[3] =  ReadAddr & 0xFF;            /* 发送块区擦除地址低位 */
	
	SPIMDriver_Transfer(sendBuffer, recvBuffer, 4+AllNum);

	/* 读取数据 */
	for (u16 i = 0; i < AllNum; i++)
	{
		if (ByteVal == recvBuffer[4+i]) {
				ReadByteval++;
		}
	}

	return ReadByteval;
#endif
}
 
/********************************************** END OF FLEE **********************************************/