/*
 * GD25QDriver.h
 *
 *  Created on: 2024年9月23日
 *      Author: Administrator
 */

#ifndef DRIVER_INC_GD25QDRIVER_H_
#define DRIVER_INC_GD25QDRIVER_H_

#include "main.h"

#define GD25Q80E_PAGE_LEN     256U
#define GD25Q80E_NUM_PAGES    4096U
#define GD25Q80E_NUM_BYTES    1048576U

#define GD25Q80E_FAST_READ    0x0B
#define GD25Q80E_WRITE_ENABLE 0x06
#define GD25Q80E_CHIP_ERASE   0xC7
#define GD25Q80E_SECTOR_ERASE 0xD8
#define GD25Q80E_READ_STATUS1 0x05
#define GD25Q80E_WRITE_STATUS 0x01 
#define GD25Q80E_PAGE_PROGRAM 0x02
#define GD25Q80E_DEVICE_ID    0xAB
#define GD25Q80E_UNIQUE_ID    0x4B

#define GD25Q80E_STATUS_BUSY  0x01

#define GD25Q80E_DEVICE_ID_RES  0x13 /* Expected device_id for GD25Q80E */


uint8_t GD25QDriver_GetDeviceId(void);
void GD25QDriver_SetUp(void);
uint8_t GD25QDriver_ReadSR(void);
uint8_t GD25QDriver_WriteSR(uint8_t u8sr);
void GD25QDriver_WaitWhileBusy(void);
uint8_t GD25QDriver_WriteEnable(void);
uint8_t GD25QDriver_EraseChip(void);
uint8_t GD25QDriver_PageProgram(const uint32_t addr, const uint16_t len, const uint8_t* data);
uint8_t GD25QDriver_Program(uint32_t addr, uint32_t len, const uint8_t* data);
uint8_t GD25QDriver_ReadData(uint32_t addr, uint32_t len, uint8_t* const data);

#endif /* DRIVER_INC_GD25QDRIVER_H_ */
