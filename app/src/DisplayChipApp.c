/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    DisplayChipApp.c

  @Summary
    Setup LCD Driver relevent function for main function to call.

  @Description
    -Setup a function for LCD Fault check.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/DisplayChipApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/DiagApp.h"
#include "app/inc/TC0App.h"
#include "driver/inc/I2C4MDriver.h"
#include "driver/inc/UartDriver.h"

#define CHIP_ADDR   0x6FU
static uint8_t u8TxBuffer[80] = {0};

void DisplayChipApp_FaultCheck(void)
{
  uint8_t u8Status;
  uint8_t u8ASIL[8] = {0};
  uint8_t u8PageCmd[2] = {0};
  /*LCDERR*/
  uint8_t u8ReadRegister0x00[1] = {0x00U};
  uint8_t u8ReadRegister0x01[1] = {0x01U};
  uint8_t u8ReadRegister0x07[1] = {0x07U};
  uint8_t u8ReadRegister0x03[1] = {0x03U};
  uint8_t u8ReadRegister0x04[1] = {0x04U};
  uint8_t u8ReadRegister0x0A[1] = {0x0AU};
  uint8_t u8ReadRegister0x1F[1] = {0x1FU};
  u8PageCmd[0] = 0x1E;u8PageCmd[1] = 0x2B;
  u8Status = I2C4MDriver_Write(CHIP_ADDR,u8PageCmd,sizeof(u8PageCmd));
  if(u8Status == ERROR_NONE){
    u8Status = I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x00,sizeof(u8ReadRegister0x00),&u8ASIL[0],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x01,sizeof(u8ReadRegister0x01),&u8ASIL[1],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x07,sizeof(u8ReadRegister0x07),&u8ASIL[2],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x03,sizeof(u8ReadRegister0x03),&u8ASIL[3],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x04,sizeof(u8ReadRegister0x04),&u8ASIL[4],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x0A,sizeof(u8ReadRegister0x0A),&u8ASIL[5],1U);
    u8Status |= I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x1F,sizeof(u8ReadRegister0x1F),&u8ASIL[6],1U);
    if(u8Status != ERROR_NONE){
            /* Do nothing*/
    }else{
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x00,u8ASIL[0]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x01,u8ASIL[1]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x07,u8ASIL[2]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x03,u8ASIL[3]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x04,u8ASIL[4]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x0A,u8ASIL[5]);
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x1F,u8ASIL[6]);
        /* Ref Hardware_Software_ReleaseNote_FORD CD542_0906.xlsx*/
        if(((u8ASIL[0]&0x7D)!=0x00) || ((u8ASIL[1]&0x03)!=0x00) || ((u8ASIL[2]&0x5F)!=0x00))
        {
            DiagApp_RtnRstRequestCheck(true,DIAG_RST_LCD_MASK);
        }else{
            /* Do nothing*/
        }
        sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LCD [0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]\r\n",u8ASIL[0],u8ASIL[1],u8ASIL[2],u8ASIL[3],u8ASIL[4],u8ASIL[5],u8ASIL[6]);
        UartDriver_TxWriteString(u8TxBuffer);
    }
  }
  if(u8Status != ERROR_NONE){
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LCD_MASK);
        sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LCD I2C ERROR=0x%02x [0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]\r\n",u8Status,u8ASIL[0],u8ASIL[1],u8ASIL[2],u8ASIL[3],u8ASIL[4],u8ASIL[5],u8ASIL[6]);
        UartDriver_TxWriteString(u8TxBuffer);
  }else{
      /* Do nothing*/
  }
  /*TSCERR*/
  uint8_t u8ReadRegister0x1C[1] = {0x1CU};
  u8PageCmd[0] = 0x1E;u8PageCmd[1] = 0x41;
  u8Status = I2C4MDriver_Write(CHIP_ADDR,u8PageCmd,sizeof(u8PageCmd));
  if(u8Status == ERROR_NONE){
    u8Status = I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister0x1C,sizeof(u8ReadRegister0x1C),&u8ASIL[7],1U);
    if(u8Status != ERROR_NONE){
          /* Do nothing*/
    }else{
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LCD_FAULT_0x1C,u8ASIL[7]);
        /* Ref Hardware_Software_ReleaseNote_FORD CD542_0906.xlsx*/
        if((u8ASIL[7]&0x7)!=0x00)
        {
            DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_TSCERR_MASK);
        }else{
            /* Do nothing*/
        }
        sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> TOUCH [0x%02x]\r\n",u8ASIL[7]);
        UartDriver_TxWriteString(u8TxBuffer);
    }
  }
  if(u8Status != ERROR_NONE){
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LCD_MASK);
        sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LCD I2C ERROR=0x%02x [0x%02x]\r\n",u8Status,u8ASIL[7]);
        UartDriver_TxWriteString(u8TxBuffer);
  }else{
      /* Do nothing*/
  }
}
void DisplayChipApp_VerCheck()
{
    uint8_t u8Status;
    uint8_t u8ChipVersion[1] = {0};
    uint8_t u8PageCmd[2] = {0x1EU,0x28U};
    uint8_t u8ReadRegister[1] = {0x1BU};
    u8Status = I2C4MDriver_Write(CHIP_ADDR,u8PageCmd,sizeof(u8PageCmd));
    if(u8Status == ERROR_NONE){
        u8Status = I2C4MDriver_WriteRead(CHIP_ADDR,u8ReadRegister,sizeof(u8ReadRegister),&u8ChipVersion[0],1U);
        if(u8Status != ERROR_NONE){
            /* Do nothing*/
        }else{
            RegisterApp_DHU_Setup(CMD_DTC,DTC_DDI_VERSION,u8ChipVersion[0]);
            sprintf((char *)u8TxBuffer,"VERSION CHECK > LCD Ver: 0x%02x\r\n",u8ChipVersion[0]);
            UartDriver_TxWriteString(u8TxBuffer);
        }
    }else{
        /* Do nothing*/
    }
    if(u8Status != ERROR_NONE){
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LCD_MASK);
        UartDriver_TxWriteString(u8TxBuffer);
    }else{
        /* Do nothing*/
        DiagApp_I2CMasterFaultCheck(false,DIAG_I2CM_LCD_MASK);
    }
}