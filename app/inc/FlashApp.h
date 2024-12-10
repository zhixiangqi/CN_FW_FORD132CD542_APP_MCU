#ifndef APP_INC_FLASHAPP_H__
#define APP_INC_FLASHAPP_H__

#include "main.h"

#define ADDR_MCUFLASH_FABCTRL   0x0001F100U
#define ADDR_MCUFLASH_MAXIMUM   0x00020000U
#define ADDR_MCUFLASH_APPFLAG   0x0001F000U
#define ADDR_MCUFLASH_SNUMBER   0x0001F100U
#define ADDR_MCUFLASH_DIMMING   0x0001F200U
#define ADDR_SHIFT_PRJNAMEMRK   0x0000DFF8U
//LDRA_EXCLUDE_START 337 S
#if (MCU_POSITION == 0x02U)
	#define PRJ_INFO_SECTION ".prj.info.2"
    #define PRJ_UPDATE_CHECK_ADDR 0x00000000
#elif (MCU_POSITION == 0x0AU)
	#define PRJ_INFO_SECTION ".prj.info.A"
    #define PRJ_UPDATE_CHECK_ADDR 0x00011000
#elif (MCU_POSITION == 0x0BU)
	#define PRJ_INFO_SECTION ".prj.info.B"
    #define PRJ_UPDATE_CHECK_ADDR 0x00003000
#else
	#error MCU_POSITION IS NOT DEFINED OR INVALID
#endif
//LDRA_EXCLUDE_END 337 S
#define SECTOR_SERIAL_NUM(nsector)    	(nsector)
#define PAGE_SERIAL_NUM(npage)    		(npage)
#define WRITE_SERIAL_NUM(nwrite)    	(nwrite)
#define READ_SERIAL_NUM(nread)    		(nread)
#define POS_NUM(npos)    				(npos)

#define CHIP_ERASE_FALG  	0xC7U
#define WRITE_CYCLE_FALG  	0x80U
#define SECTOR_COMPLETE     0x01U
#define SECTOR_UNCOMPLETE   0x00U

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length);
void FlashApp_CheckNorFlash(void);
void FlashApp_WriteNorFlash(void);

#endif
/* *****************************************************************************
 End of File
*/