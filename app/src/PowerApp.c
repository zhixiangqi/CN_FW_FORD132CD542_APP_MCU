/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    PowerApp.c

  @Summary
    Design Power relevent function with MCU GPIO/ADC control.

  @Description
    -Power Sequence design
    -LED Driver function
    -TFT Driver function
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/PowerApp.h"
#include "app/inc/TC0App.h"
#include "app/inc/DiagApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/FlashApp.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/I2C4MDriver.h"

#define BIAS_ADDR   0x6BU
#define LED_ADDR    0x3AU
#define DDI_ADDR    0x6FU
#define SUPPLY_HIGH_STATUS 0xFFU
#define SUPPLY_LOW_STATUS  0xFFU
#define BOOST_HIGH_STATUS  0xA0U
#define BOOST_LOW_STATUS   0x28U
#define LED_HIGH_STATUS    0xFFU
#define LED_LOW_STATUS     0xFFU

PowerState PowerSt = {0x01U,0x01U,0x01U};
//LDRA_EXCLUDE_START 434 S
static uint8_t u8TxPowerBuffer[60] = {0};

void PowerApp_Sequence(uint8_t u8Action)
{
    switch (u8Action)
    {
    case POWER_ON: //total ms
        /* code */
        PortDriver_PinSet(HVLDO_EN_PORT,HVLDO_EN_PIN);
        PortDriver_PinSet(VBATT_EN_PORT,VBATT_EN_PIN);
        (void)TC0App_DelayMS(10U);
        PortDriver_PinSet(P3V3_EN_PORT,P3V3_EN_PIN);
        (void)TC0App_DelayMS(10U);
        PortDriver_PinSet(P1V2_EN_PORT,P1V2_EN_PIN);
        (void)TC0App_DelayMS(10U);
        PortDriver_PinSet(DISP_RESX_PORT,DISP_RESX_PIN);
        (void)TC0App_DelayMS(40U);
        PortDriver_PinSet(DES_PDB_PORT,DES_PDB_PIN);
        (void)TC0App_DelayMS(10U);
        PortDriver_PinSet(DES_INTB_PORT,DES_INTB_PIN);
        break;

    case POWER_OFF: //total ms
        /* code */
        PortDriver_PinClear(DES_PDB_PORT,DES_PDB_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(DES_INTB_PORT,DES_INTB_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(DISP_RESX_PORT,DISP_RESX_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(P1V2_EN_PORT,P1V2_EN_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(P3V3_EN_PORT,P3V3_EN_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(VBATT_EN_PORT,VBATT_EN_PIN);
        (void)TC0App_DelayMS(100U);
        PortDriver_PinClear(HVLDO_EN_PORT,HVLDO_EN_PIN);
        break;

    case LCD_ON:
        PortDriver_PinSet(BIAS_EN_PORT,BIAS_EN_PIN);
        (void)TC0App_DelayMS(10U);
        PortDriver_PinSet(DISP_STBY_PORT,DISP_STBY_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinSet(LED_EN_PORT,LED_EN_PIN);
        (void)TC0App_DelayMS(15U);
        PowerApp_LP8664_CurrentSet();
        break;

    case LCD_OFF:
        PortDriver_PinClear(LED_EN_PORT,LED_EN_PIN);
        (void)TC0App_DelayMS(5U);
        PortDriver_PinClear(DISP_STBY_PORT,DISP_STBY_PIN);
        PortDriver_PinClear(U301_TSC_RESET_PORT,U301_TSC_RESET_PIN);
        PortDriver_PinClear(BIAS_EN_PORT,BIAS_EN_PIN);
        break;

    default:
		// ...
        break;
    }
}

DiagIO PG_P1V2;
DiagIO PG_P3V3;
DiagIO FAULT_RTQ6749;
DiagIO FAULT_LP8664;
DiagIO FAULT_I2CMASTER;
void PowerApp_PowerGoodInitial(void)
{
    PG_P1V2.Status = IO_STATUS_SWIM;
    PG_P1V2.Port = P1V2_PG_PORT;
    PG_P1V2.PortNumber = P1V2_PG_PIN;
    PG_P1V2.Threshlod = 5U;
    PG_P1V2.ConsecutiveHighCnt =  0U;
    PG_P1V2.ConsecutiveLowCnt = 0U;

    PG_P3V3.Status = IO_STATUS_SWIM;
    PG_P3V3.Port = P3V3_PG_PORT;
    PG_P3V3.PortNumber = P3V3_PG_PIN;
    PG_P3V3.Threshlod = 5U;
    PG_P3V3.ConsecutiveHighCnt = 0U;
    PG_P3V3.ConsecutiveLowCnt = 0U;

    FAULT_RTQ6749.Status = IO_STATUS_SWIM;
    FAULT_RTQ6749.Threshlod = 1U;
    FAULT_RTQ6749.ConsecutiveHighCnt = 0U;
    FAULT_RTQ6749.ConsecutiveLowCnt = 0U;

    FAULT_LP8664.Status = IO_STATUS_SWIM;
    FAULT_LP8664.Threshlod = 1U;
    FAULT_LP8664.ConsecutiveHighCnt = 0U;
    FAULT_LP8664.ConsecutiveLowCnt = 0U;

    FAULT_I2CMASTER.Status = IO_STATUS_SWIM;
    FAULT_I2CMASTER.Threshlod = 5U;
    FAULT_I2CMASTER.ConsecutiveHighCnt = 0U;
    FAULT_I2CMASTER.ConsecutiveLowCnt = 0U;
}

void PowerApp_PowerGoodFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    uint8_t u8Status2 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&PG_P1V2);
    if(IO_STATUS_HIGH == u8Status1){

    }else if(IO_STATUS_LOW == u8Status1){
        /* GO TO SHUTDOWN*/
        RegisterApp_DHU_Setup(CMD_DISP_SHUTD,CMD_DATA_POS,0x01);
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"P1V2 SWIM >> 0x%02x, %d, %d\r\n",u8Status1,PG_P1V2.ConsecutiveHighCnt,PG_P1V2.ConsecutiveLowCnt);
        UartDriver_TxWriteString(u8TxPowerBuffer);
    }
    u8Status2 = DiagApp_ConsecutiveCheckIO(&PG_P3V3);
    if(IO_STATUS_HIGH == u8Status2){

    }else if(IO_STATUS_LOW == u8Status2){
        /* GO TO SHUTDOWN*/
        RegisterApp_DHU_Setup(CMD_DISP_SHUTD,CMD_DATA_POS,0x01);
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"P3V3 SWIM >> 0x%02x, %d, %d\r\n",u8Status2,PG_P3V3.ConsecutiveHighCnt,PG_P3V3.ConsecutiveLowCnt);
        UartDriver_TxWriteString(u8TxPowerBuffer);
    }
    // sprintf((char *)u8TxBuffer,"PG FLOW> P1V2 0x%02x > P3V3 0x%02x\r\n",u8Status1,u8Status2);
    // UartDriver_TxWriteString(u8TxBuffer);
}
//LDRA_EXCLUDE_START 8 D
void PowerApp_RTQ6749_FaultCheck(void)
{
    uint8_t CMD_ControlMultiRead[2] = {0xFFU,0x00};
    uint8_t CMD_DataAddr[1] = {0x00};
    uint8_t RxBuffer[31] = {0U};
    uint8_t u8fault = 0U;
    uint8_t Status = ERROR_NONE;
    uint8_t u8Status = IO_STATUS_SWIM;
    Status = I2C4MDriver_Write(BIAS_ADDR,CMD_ControlMultiRead,2U);
    Status |= I2C4MDriver_WriteRead(BIAS_ADDR,CMD_DataAddr,1U,RxBuffer,30U);
    if(Status != ERROR_NONE)
    {
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_BIAS_MASK);
        sprintf((char *)u8TxPowerBuffer,"I2C M driver transmit fail >> 0x%02x\r\n",Status);
        //UartDriver_TxWriteString(u8TxBuffer);
        u8fault = 0xFFU;
    }else{
        u8fault = RxBuffer[0x1DU];
        RegisterApp_DHU_Setup(CMD_DTC,DTC_BIAS_FAULT_0x1D,u8fault);
        sprintf((char *)u8TxPowerBuffer,"RTQ6749 Fault Analysis >> 0x%02x\r\n",u8fault);
        //UartDriver_TxWriteString(u8TxBuffer);
    }

    if(u8fault == 0x00U)
    {
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_RTQ6749, true);
    }else{
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_RTQ6749, false);
    }

    if(IO_STATUS_HIGH == u8Status){
        //UartDriver_TxWriteString((uint8_t *)"RTQ6749 is Good!\r\n");
    }else if(IO_STATUS_LOW == u8Status){
        /* Disp status set ref BIAS_FAULT IO PIN, not I2C
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        */
        //UartDriver_TxWriteString((uint8_t *)"RTQ6749 fault happen!\r\n");
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"RTQ6749 SWIM >> 0x%02x, %d, %d\r\n",u8Status,FAULT_RTQ6749.ConsecutiveHighCnt,FAULT_RTQ6749.ConsecutiveLowCnt);
        UartDriver_TxWriteString(u8TxPowerBuffer);
    }
}
//LDRA_EXCLUDE_END 8 D
//LDRA_EXCLUDE_START 139 S
//LDRA_EXCLUDE_START 105 D
void PowerApp_LP8664_CurrentSet(void)
{
    uint8_t u8CurrentSet[3] = {0x02U,0xFFU,0x0FU};
    uint8_t u8FlashData[2] = {0};
    uint8_t Status = ERROR_NONE;
     //LDRA_EXCLUDE_START 440 S
    (void)memcpy((void *)u8FlashData,(void *)(ADDR_MCUFLASH_DIMMING), 2U);
     //LDRA_EXCLUDE_END 440 S
    if(((u8FlashData[0U]+(u8FlashData[1U]*256U)) > 0x09FFU) && ((u8FlashData[0U]+(u8FlashData[1U]*256U)) <= 0x0FFFU))
    {
        u8CurrentSet[1U] = u8FlashData[0U];
        u8CurrentSet[2U] = u8FlashData[1U] & 0x0FU;
        Status = I2C4MDriver_Write(LED_ADDR,u8CurrentSet,3U);
        if(Status != ERROR_NONE)
        {
            DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LED_MASK);
            sprintf((char *)u8TxPowerBuffer,"LP8664 Current Set Fail >> 0x%02x\r\n",Status);
            UartDriver_TxWriteString(u8TxPowerBuffer);
        }else{
            sprintf((char *)u8TxPowerBuffer,"LP8664 Current Set >> 0x%02X%02X\r\n",u8FlashData[1],u8FlashData[0]);
            UartDriver_TxWriteString(u8TxPowerBuffer);
        }
    }else{
        /* Do nothing, IC default set as 0x0FFF*/
        sprintf((char *)u8TxPowerBuffer,"LP8664 Current default >> 0x%02X%02X\r\n",u8FlashData[1],u8FlashData[0]);
        UartDriver_TxWriteString(u8TxPowerBuffer);
    }
}
//LDRA_EXCLUDE_END 139 S
//LDRA_EXCLUDE_END 105 D
void PowerApp_LP8664_FaultCheck(void)
{
    uint8_t CMD_DataAddr[1] = {0x00};
    uint8_t RxBuffer[31] = {0U};
    uint8_t u8fault[7] = {0U};
    uint8_t Status = ERROR_NONE;
    uint8_t u8Status = IO_STATUS_SWIM;
    Status = I2C4MDriver_WriteRead(LED_ADDR,CMD_DataAddr,1U,RxBuffer,30U);
    if(Status != ERROR_NONE)
    {
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LED_MASK);
        sprintf((char *)u8TxPowerBuffer,"I2C M driver transmit fail >> 0x%02x\r\n",Status);
        UartDriver_TxWriteString(u8TxPowerBuffer);
        u8fault[0] = 0xFFU;
    }else{
        u8fault[1] = RxBuffer[0x0EU];
        u8fault[1] &= SUPPLY_LOW_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x0E,u8fault[1]);
        u8fault[2] = RxBuffer[0x0FU];
        u8fault[2] &= SUPPLY_HIGH_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x0F,u8fault[2]);
        u8fault[3] = RxBuffer[0x10U];
        u8fault[3] &= BOOST_LOW_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x10,u8fault[3]);
        u8fault[4] = RxBuffer[0x11U];
        u8fault[4] &= BOOST_HIGH_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x11,u8fault[4]);
        u8fault[5] = RxBuffer[0x12U];
        u8fault[5] &= LED_HIGH_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x12,u8fault[5]);
        u8fault[6] = RxBuffer[0x13U];
        u8fault[6] &= LED_LOW_STATUS;
        RegisterApp_DHU_Setup(CMD_DTC,DTC_LED_FAULT_0x13,u8fault[6]);
        u8fault[0] = u8fault[1]|u8fault[2]|u8fault[3]|u8fault[4]|u8fault[5]|u8fault[6];
        sprintf((char *)u8TxPowerBuffer,"LP8664 Fault Analysis >> 0x%02x\r\n",u8fault[0]);
        UartDriver_TxWriteString(u8TxPowerBuffer); 
        /*Verify LED I2C Error Status */
        if ((RegisterApp_DHU_Read(CMD_DTC,DTC_LED_FAULT_0x13) & 0x40U) == 0x40U)
        {
            uint8_t u8CmdDataAddr[3] = {0x12U,0x00U,0x60U};
            (void)I2C4MDriver_Write(LED_ADDR,u8CmdDataAddr,3U);
        }
    }

    if(u8fault[0] == 0x00U)
    {
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_LP8664, true);
    }else{
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_LP8664, false);  
    }

    if(IO_STATUS_HIGH == u8Status){
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_LED_MASK);
    }else if(IO_STATUS_LOW == u8Status){
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(true,DIAG_RST_LED_MASK);
        //UartDriver_TxWriteString((uint8_t *)"LP8664 fault happen!\r\n");
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"LP8664 SWIM >> 0x%02x, %d, %d\r\n",u8Status,FAULT_LP8664.ConsecutiveHighCnt,FAULT_LP8664.ConsecutiveLowCnt);
        //UartDriver_TxWriteString(u8TxBuffer);
    }
}
//LDRA_EXCLUDE_END 8 D

void PowerApp_RTQ6749_I2CFaultCheck(void)
{
    /*RTQ6749 Fault Check*/
    uint8_t CMD_ControlMultiRead[2] = {0xFFU,0x00};
    uint8_t Status = ERROR_NONE;
    uint8_t u8Status = IO_STATUS_SWIM;
    Status = I2C4MDriver_Write(BIAS_ADDR,CMD_ControlMultiRead,2U);
    if(Status != ERROR_NONE)
    {
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_BIAS_MASK);
        UartDriver_TxWriteString((uint8_t *)"RTQ6749 I2C M driver transmit fail\r\n");
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, false);
    }else{
        DiagApp_I2CMasterFaultCheck(false,DIAG_I2CM_BIAS_MASK);
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, true);
        // UartDriver_TxWriteString((uint8_t *)"RTQ6749 I2C M driver transmit success\r\n");
    }

    if(IO_STATUS_HIGH == u8Status){
        PowerSt.PowerBaisFautSt = IO_STATUS_HIGH;
        if (IO_STATUS_HIGH == DiagSt.DiagBaisFautSt)
        {
            DiagApp_DispStatusClear(DISP_STATUS_BYTE1,DISP1_DISPERR_MASK);
            (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_BIAS_MASK);
        }
    }else if(IO_STATUS_LOW == u8Status){
        PowerSt.PowerBaisFautSt = IO_STATUS_LOW;
        DiagApp_DispStatusSet(DISP_STATUS_BYTE1,DISP1_DISPERR_MASK);
        (void) DiagApp_RtnRstRequestCheck(true,DIAG_RST_BIAS_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"RTQ6749 SWIM >> 0x%02x, %d, %d\r\n",u8Status,FAULT_I2CMASTER.ConsecutiveHighCnt,FAULT_I2CMASTER.ConsecutiveLowCnt);
        //UartDriver_TxWriteString(u8TxPowerBuffer);
    }
}

void PowerApp_LP8664_I2CFaultCheck(void)
{
    /*LP8664 Fault Check*/
    uint8_t RxBuffer[1] = {0U};
    uint8_t CMD_DataAddr[1] = {0U};
    uint8_t Status = ERROR_NONE;
    uint8_t u8Status = IO_STATUS_SWIM;
    Status = I2C4MDriver_WriteRead(LED_ADDR,CMD_DataAddr,1U,RxBuffer,1U);
    if(Status != ERROR_NONE)
    {
        DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LED_MASK);
        UartDriver_TxWriteString((uint8_t *)"LP8864 I2C M driver transmit fail\r\n");
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, false);
    }else{
        DiagApp_I2CMasterFaultCheck(false,DIAG_I2CM_LED_MASK);
        u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, true);
        // UartDriver_TxWriteString((uint8_t *)"LP8864 I2C M driver transmit success\r\n");
    }

    if(IO_STATUS_HIGH == u8Status){
        PowerSt.PowerLedFautSt = IO_STATUS_HIGH;
        if (IO_STATUS_HIGH == DiagSt.DiagLedFautSt)
        {
            DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
            (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_LED_MASK);
        }
    }else if(IO_STATUS_LOW == u8Status){
        PowerSt.PowerLedFautSt = IO_STATUS_LOW;
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(true,DIAG_RST_LED_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"LP8664 SWIM >> 0x%02x, %d, %d\r\n",u8Status,FAULT_I2CMASTER.ConsecutiveHighCnt,FAULT_I2CMASTER.ConsecutiveLowCnt);
        //UartDriver_TxWriteString(u8TxPowerBuffer);
    }
}

void PowerApp_DDI_I2CFaultCheck(void)
{
  /*DisplayChipApp Version Check*/
  uint8_t Status = ERROR_NONE;
  uint8_t u8Status = IO_STATUS_SWIM;
  uint8_t u8PageCmd[2] = {0x1EU,0x28U};
  Status = I2C4MDriver_Write(DDI_ADDR,u8PageCmd,sizeof(u8PageCmd));
  if(Status != ERROR_NONE){
    DiagApp_I2CMasterFaultCheck(true,DIAG_I2CM_LCD_MASK);
    UartDriver_TxWriteString((uint8_t *)"DDI I2C M driver transmit fail\r\n");
    u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, false);
  }else{
    DiagApp_I2CMasterFaultCheck(false,DIAG_I2CM_LCD_MASK);
    u8Status = DiagApp_ConsecutiveCheckRegister(&FAULT_I2CMASTER, true);
    // UartDriver_TxWriteString((uint8_t *)"DDI I2C M driver transmit success\r\n");
  }

  if(IO_STATUS_HIGH == u8Status){
        PowerSt.PowerLcdFautSt = IO_STATUS_HIGH;
        if (IO_STATUS_HIGH == DiagSt.DiagLcdFautSt)
        {
            DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
            (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_LCD_MASK);
        }
    }else if(IO_STATUS_LOW == u8Status){
        PowerSt.PowerLcdFautSt = IO_STATUS_LOW;
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(true,DIAG_RST_LCD_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        sprintf((char *)u8TxPowerBuffer,"DDI SWIM >> 0x%02x, %d, %d\r\n",u8Status,FAULT_I2CMASTER.ConsecutiveHighCnt,FAULT_I2CMASTER.ConsecutiveLowCnt);
        //UartDriver_TxWriteString(u8TxBuffer);
    }
}