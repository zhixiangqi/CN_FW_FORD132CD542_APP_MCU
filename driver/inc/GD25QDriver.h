/*
 * GD25QDriver.h
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */

#ifndef DRIVER_INC_GD25QDRIVER_H_
#define DRIVER_INC_GD25QDRIVER_H_

#include "main.h"

#define u8                                      uint8_t
#define u16                                     uint16_t
#define u32                                     uint32_t

#define GD25Q_80CSIG                            0x01U
 
#define GD25Q80CSIGDeviceID						0x13U
#define GD25Q80CSIGManufactureID				0xC813U
#define GD25Q80CSIGIdentificationID				0xC84014U
 
 
#define GD25Q80_BASE_ADDR						0x000000U
#define GD25Q80_CHIP_BYTE_SIZE					0x100000U													// 1 M
#define GD25Q80_BLOCK_BYTE_SIZE					0x010000U													// 64 K
#define GD25Q80_SECTOR_BYTE_SIZE				0x001000U													// 4 K
#define GD25Q80_PAGE_BYTE_SIZE					0x000100U													// 256Byte
 
#define GD25Q80_BLOCK_ADDRESS0					0x000000U
#define GD25Q80_BLOCK_ADDRESS1					0x010000U
#define GD25Q80_BLOCK_ADDRESS2					0x020000U
#define GD25Q80_BLOCK_ADDRESS3					0x030000U
#define GD25Q80_BLOCK_ADDRESS4					0x040000U
#define GD25Q80_BLOCK_ADDRESS5					0x050000U
#define GD25Q80_BLOCK_ADDRESS6					0x060000U
#define GD25Q80_BLOCK_ADDRESS7					0x070000U
#define GD25Q80_BLOCK_ADDRESS8					0x080000U
#define GD25Q80_BLOCK_ADDRESS9					0x090000U
#define GD25Q80_BLOCK_ADDRESS10					0x0A0000U
#define GD25Q80_BLOCK_ADDRESS11					0x0B0000U
#define GD25Q80_BLOCK_ADDRESS12					0x0C0000U
#define GD25Q80_BLOCK_ADDRESS13					0x0D0000U
#define GD25Q80_BLOCK_ADDRESS14					0x0E0000U
#define GD25Q80_BLOCK_ADDRESS15					0x0F0000U
 
#define GD25Q80_SECTOR_ADDRESS(nSector)			    (GD25Q80_BASE_ADDR + (GD25Q80_SECTOR_BYTE_SIZE * (nSector)))
#define GD25Q80_PAGE_ADDRESS(nPage)				    (GD25Q80_BASE_ADDR + (GD25Q80_PAGE_BYTE_SIZE * (nPage)))

#define GD25Q80_SECTOR_NUM						    256
#define GD25Q80_PAGE_NUM						    4096
 
 
#define GD25Q_Dummy_Byte						0xFFU
 
/* GD25Q80CSIG 指令表 */
#define GD25Q_WriteEnable						0x06U
#define GD25Q_WriteDisable						0x04U
 
#define GD25Q_ReadStatusReg1					0x05U
#define GD25Q_ReadStatusReg2					0x35U
#define GD25Q_WriteStatusReg					0x01U
 
#define GD25Q_ReadData							0x03U
#define GD25Q_FastReadData						0x0BU
#define GD25Q_FastReadDual						0x3BU
#define GD25Q_PageProgram						0x02U
 
#define GD25Q_SectorErase						0x20U
#define GD25Q_BlockErase						0xD8U
#define GD25Q_ChipErase							0xC7U
 
#define GD25Q_PowerDown							0xB9U
#define GD25Q_ReleasePowerDown					0xABU
#define GD25Q_DeviceID							0xABU
#define GD25Q_ManufactDeviceID					0x90U
#define GD25Q_JedecDeviceID						0x9FU
/* GD25Q80CSIG End Command */
 
typedef enum
{
	GD25Q80CSIG_OK								= 0x00,
	GD25Q80CSIG_ERROR							= 0x01
}GD25Q_StatusTypeDef;

bool GD25Q_SPIFLASH_Init(void);															//GD25Q SPIFLASH 初始化
bool GD25Q_SPIFLASH_ReadByte(u8 *rxBuffer);												//SPI FLASH 读取一个字节数据
bool GD25Q_SPIFLASH_SendByte(u8 *txBuffer);												//SPI FLASH 发送一个字节数据
bool GD25Q_SPIFLASH_ReadDeviceID(u8 *deviceID);											//GD25Q SPIFLASH 读取设备ID
bool GD25Q_SPIFLASH_ReadManufactureID(u16 *manufactureID);								//GD25Q SPIFLASH 读取厂商ID
bool GD25Q_SPIFLASH_ReadIdentificationID(u32 *identificationID);						//GD25Q SPIFLASH 读取型号ID
bool GD25Q_SPIFLASH_WriteEnable(void);													//GD25Q SPIFLASH 写使能
bool GD25Q_SPIFLASH_WriteDisable(void);													//GD25Q SPIFLASH 写禁止
bool GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister, u8 *rxBuffer);				//GD25Q SPIFLASH 读状态寄存器
bool GD25Q_SPIFLASH_WriteStatusRegister(u8 srLow, u8 srHigh);							//GD25Q SPIFLASH 写状态寄存器
bool GD25Q_SPIFLASH_WaitForBusy(void);													//GD25Q SPIFLASH 等待设备空闲
bool GD25Q_SPIFLASH_PowerDown(void);													//GD25Q SPIFLASH 进入掉电模式
bool GD25Q_SPIFLASH_WakeUp(void);														//GD25Q SPIFLASH 唤醒
bool GD25Q_SPIFLASH_EraseChip(void);													//GD25Q SPIFLASH 擦除Chip
bool GD25Q_SPIFLASH_EraseBlock(u32 BlockAddr);											//GD25Q SPIFLASH 擦除Block
bool GD25Q_SPIFLASH_EraseSector(u32 SectorAddr);										//GD25Q SPIFLASH 擦除Sector
bool GD25Q_SPIFLASH_ReadBuffer(u8 pBuffer[], u32 ReadAddr, u16 NumByteToRead);			//GD25Q SPIFLASH 读取SPI FLASH数据
bool GD25Q_SPIFLASH_WritePage(u8 pBuffer[], u32 WriteAddr, u16 NumByteToWrite);			//GD25Q SPIFLASH 写入SPI FLASH数据Page
bool GD25Q_SPIFLASH_GetByte(u32 ReadAddr, u8 *rxBuffer);								//GD25Q SPIFLASH 读取1个Byte
bool GD25Q_SPIFLASH_GetHalfWord(u32 ReadAddr, u16 *rxBuffer);							//GD25Q SPIFLASH 读取2个Byte
bool GD25Q_SPIFLASH_GetWord(u32 ReadAddr, u32 *rxBuffer);								//GD25Q SPIFLASH 读取4个Byte
bool GD25Q_SPIFLASH_SetByte(u32 WriteAddr, u8 val);										//GD25Q SPIFLASH 写入1个Byte
bool GD25Q_SPIFLASH_SetHalfWord(u32 WriteAddr, u16 val);								//GD25Q SPIFLASH 写入2个Byte
bool GD25Q_SPIFLASH_SetWord(u32 WriteAddr, u32 val);									//GD25Q SPIFLASH 写入4个Byte
u32 GD25Q_SPIFLASH_Use_Address(u8 nSector, u8 nPage , u8 nPos);		//GD25Q SPIFLASH 设定Ues Address

#endif /* DRIVER_INC_GD25QDRIVER_H_ */