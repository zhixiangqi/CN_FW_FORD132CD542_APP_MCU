/*
 * GD25QDriver.h
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */

#ifndef DRIVER_INC_GD25QDRIVER_H_
#define DRIVER_INC_GD25QDRIVER_H_

#include "main.h"

#define GD25Q_WriteEnable		0x06 
#define GD25Q_WriteDisable		0x04 
#define GD25Q_ReadStatusReg		0x05 
#define GD25Q_WriteStatusReg	0x01 
#define GD25Q_ReadData			0x03 
#define GD25Q_FastReadData		0x0B 
#define GD25Q_FastReadDual		0x3B 
#define GD25Q_PageProgram		0x02 
#define GD25Q_BlockErase		0xD8 
#define GD25Q_SectorErase		0x20 
#define GD25Q_ChipErase			0xC7 
#define GD25Q_PowerDown			0xB9 
#define GD25Q_ReleasePowerDown	0xAB 
#define GD25Q_DeviceID			0xAB 
#define GD25Q_ManufactDeviceID	0x90 
#define GD25Q_JedecDeviceID		0x9F

uint8_t GD25QDriver_ReadSR(void);
void GD25QDriver_Wait_Busy(void);
uint8_t GD25QDriver_WriteEnable(bool benable);
uint8_t GD25QDriver_WriteSR(uint8_t u8sr);
uint16_t GD25QDriver_ReadDeviceID(void);
uint8_t GD25QDriver_ReadData(uint8_t* u8pBuffer,uint32_t u32ReadAddr,uint16_t u16NumByteToRead);
uint8_t GD25QDriver_PageProgram(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite);
uint8_t GD25QDriver_PageProgram_NoCheck(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite);
uint8_t GD25QDriver_SectorErase(uint32_t u32DstAddr);
uint8_t GD25QDriver_EraseChip(void);
void GD25QDriver_WriteAppoint(uint8_t* u8pBuffer,uint32_t u32WriteAddr,uint16_t u16NumByteToWrite);

#endif /* DRIVER_INC_GD25QDRIVER_H_ */
