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
#include "app/inc/RegisterApp.h"
#include "app/inc/WdtApp.h"
#include "driver/inc/NVMDriver.h"
//LDRA_EXCLUDE_START 496 S
//LDRA_EXCLUDE_START 554 S
void FlashApp_WriteRowFlash(uint8_t data[], const uint32_t address, uint8_t length)
{
  uint8_t dataStr[256] = {0};
  (void)memcpy((void *)dataStr, (void *) FLASH_ROW_ADDRESS( address ), SIZE_ROW);
  uint8_t start_addr = (uint8_t)(address & 0xFFU);
  for(uint16_t addr = 0U; addr < length; addr ++){
      if((start_addr + addr) <= 0xFFU){
          dataStr[start_addr + addr] = data[addr];
      }else{
          /* error write*/
      }
  }
  (void)NVMDriver_PageWrite(dataStr,(const uint32_t)FLASH_ROW_ADDRESS(address));
}
//LDRA_EXCLUDE_END 496 S
//LDRA_EXCLUDE_END 554 S