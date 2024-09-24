/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    FlashApp.c

  @Summary
    Setup Flash relevent function for main function to call.

  @Description
    -Setup a function for flashing MCU memory.
    -Setup a function for External Flash Write/Read.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/FlashApp.h"
#include "app/inc/TC0App.h"
#include "driver/inc/NVMDriver.h"
#include "driver/inc/GD25QDriver.h"

void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length)
{
    uint8_t dataStr[256] = {0};
    (void)memcpy((void *)dataStr, (void *) FLASH_ROW_ADDRESS( address ), SIZE_ROW);
    uint8_t start_addr = (uint8_t)(address & 0xFF);
    for(uint16_t addr = 0U; addr < length; addr ++){
        if((start_addr + addr) <= 0xFF){
            dataStr[start_addr + addr] = data[addr];
        }else{
            /* error write*/
        }
    }
    NVMDriver_PageWrite(dataStr,FLASH_ROW_ADDRESS(address));
}

// uint8_t FlashApp_CheckNroFlash()
// {
//   GD25Q80_ReadData(uint32_t u32addr, uint16_t u16len, uint8_t* u8data);

// }
// void FlashApp_WriteNorFlash(uint32_t u32addr, uint16_t u16status)
// {
//   uint8_t u8dataStr[256] = {0};

//   dataStr[0] = u32addr >> 24;
//   dataStr[1] = u32addr >> 16;
//   dataStr[2] = u32addr >> 8;
//   dataStr[3] = u32addr >> 0;

//   dataStr[4] = u16status >> 8;
//   dataStr[5] = u16status >> 0;
  
//   GD25Q80E_PageProgram(u32addr, 6U, u8dataStr);

// }

// void FlashApp_WriteNorFlash(uint8_t data[], const uint32_t address, uint8_t length)
// {
//   uint8_t dataStr[256] = {0};
//   uint32_t u32PageAddr = 0x00FF;

//   dataStr[0] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 24);
//   dataStr[1] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 16);
//   dataStr[2] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 8);
//   dataStr[3] = (uint8_t)(TC0App_TimerReturn(TIMER_FLASH_LOG_COUNT) >> 0);

//   GD25Q80E_PageProgram();


// }