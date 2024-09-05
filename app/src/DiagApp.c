/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    DiagApp.c

  @Summary
    Report Diag Information for Host cmd DISP_STATUS(0x00) & DTC(0xA3).

  @Description
    -Setup a function for IO/Register Check flow mechanism.
    -Setup a function for Host cmd DISP_STATUS content status change.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/DiagApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/INTBApp.h"
#include "app/inc/PowerApp.h"
#include "app/inc/DisplayChipApp.h"
#include "app/inc/BacklightApp.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"

static uint8_t u8DiagDispByte0 = 0x00U;
static uint8_t u8DiagDispByte1 = 0x01U;
static uint8_t u8TxBuffer[80] = {0};
static uint8_t u8DiagRstReqStatus = 0x00U;
static uint8_t u8DiagIsrStatus = 0x00U;
static uint8_t u8DiagI2cFaultStatus = 0x00U;

void DiagApp_DispStatusClear(uint8_t ByteNumber, uint8_t MaskValue)
{
    uint8_t u8OldByte0 = u8DiagDispByte0;
    uint8_t u8OldByte1 = u8DiagDispByte1;
    if(ByteNumber == 0x00U)
    {
        u8DiagDispByte0 &= ~MaskValue;
        RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS,u8DiagDispByte0);
    }else if(ByteNumber == 0x01U)
    {
        u8DiagDispByte1 &= ~MaskValue;
        RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1);
    }else{
        /*DO NOTHING*/
    }

    /* Check if the data is New event (Pull Request Necessary)*/
    if (((u8OldByte0 & DISP0_LATCHED_MASK) != (u8DiagDispByte0 & DISP0_LATCHED_MASK)) || 
        ((u8OldByte1 & DISP1_LATCHED_MASK) != (u8DiagDispByte1 & DISP1_LATCHED_MASK)))
    {
        INTBApp_PullReqSetOrClear(INTB_REQ_SET);
        //RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_ERR_SET);
        DiagApp_RtnIsrCheck(true,INTB_INT_ERR_MASK);
    }
    (void)u8OldByte0;
    (void)u8OldByte1;
}

void DiagApp_DispStatusSet(uint8_t ByteNumber, uint8_t MaskValue)
{
    uint8_t u8OldByte0 = u8DiagDispByte0;
    uint8_t u8OldByte1 = u8DiagDispByte1;
    if(ByteNumber == 0x00U)
    {
        u8DiagDispByte0 |= MaskValue;
        RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS,u8DiagDispByte0);
    }else if(ByteNumber == 0x01U)
    {
        u8DiagDispByte1 |= MaskValue;
        RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1);
    }else{
        /*DO NOTHING*/
    }

    /* Check if the data is New event (Pull Request Necessary)*/
    if (((u8OldByte0 & DISP0_LATCHED_MASK) != (u8DiagDispByte0 & DISP0_LATCHED_MASK)) || 
        ((u8OldByte1 & DISP1_LATCHED_MASK) != (u8DiagDispByte1 & DISP1_LATCHED_MASK)))
    {
        INTBApp_PullReqSetOrClear(INTB_REQ_SET);
        //RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,INTB_INT_ERR_SET);
        DiagApp_RtnIsrCheck(true,INTB_INT_ERR_MASK);
    }
    (void)u8OldByte0;
    (void)u8OldByte1;
}

uint8_t DiagApp_ConsecutiveCheckIO(DiagIO* ds1)
{
    DiagIO ds = *ds1;
    if (IO_HIGH == PortDrvier_PinRead(ds.Port,ds.PortNumber)){
        ds.ConsecutiveHighCnt += 1;
        ds.ConsecutiveLowCnt = 0;
    }else{
        ds.ConsecutiveHighCnt = 0;
        ds.ConsecutiveLowCnt += 1;
    }

    if (ds.ConsecutiveHighCnt >= ds.Threshlod){
        ds.ConsecutiveHighCnt = ds.Threshlod;
        ds.Status = IO_STATUS_HIGH;
    }else if(ds.ConsecutiveLowCnt >= ds.Threshlod){
        ds.ConsecutiveLowCnt = ds.Threshlod;
        ds.Status = IO_STATUS_LOW;
    }else{
        /* status no change*/
        ds.Status = IO_STATUS_SWIM;
    }
    *ds1 = ds;
    return ds.Status;
}

uint8_t DiagApp_ConsecutiveCheckRegister(DiagIO* ds1,bool isgood)
{
    DiagIO ds = *ds1;
    if (true == isgood){
        ds.ConsecutiveHighCnt += 1;
        ds.ConsecutiveLowCnt = 0;
    }else{
        ds.ConsecutiveHighCnt = 0;
        ds.ConsecutiveLowCnt += 1;
    }

    if (ds.ConsecutiveHighCnt >= ds.Threshlod){
        ds.ConsecutiveHighCnt = ds.Threshlod;
        ds.Status = IO_STATUS_HIGH;
    }else if(ds.ConsecutiveLowCnt >= ds.Threshlod){
        ds.ConsecutiveLowCnt = ds.Threshlod;
        ds.Status = IO_STATUS_LOW;
    }else{
        /* status no change*/
        ds.Status = IO_STATUS_SWIM;
    }
    *ds1 = ds;
    return ds.Status;
}

bool DiagApp_RtnRstRequestCheck(bool set ,uint8_t u8DiagRstReqMask)
{
    bool breturn = false;
    if(set)
    {
        u8DiagRstReqStatus |= u8DiagRstReqMask;
    }else{
        u8DiagRstReqStatus &= ~u8DiagRstReqMask;
    }
    if(u8DiagRstReqStatus != 0x00U)
    {
        breturn = true;
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_RSTRQ_MASK);
        BacklightApp_RstRqSwitchSet(BLT_DISABLE);
    }else{
        breturn = false;
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_RSTRQ_MASK);
        BacklightApp_RstRqSwitchSet(BLT_ENABLE);
    }
    RegisterApp_DHU_Setup(CMD_DTC,DTC_RST_RQ,u8DiagRstReqStatus);
    return breturn;
}

bool DiagApp_RtnIsrCheck(bool set,uint8_t u8DiagIsrMask)
{
    bool breturn = false;
    if(set)
    {
        u8DiagIsrStatus |= u8DiagIsrMask;
    }else{
        u8DiagIsrStatus &= ~u8DiagIsrMask;
    }
    RegisterApp_DHU_Setup(CMD_ISR_STATUS,CMD_DATA_POS,u8DiagIsrStatus);
    return breturn;
}

void DiagApp_I2CMasterFaultCheck(bool set ,uint8_t u8DiagI2cFaultMask)
{
    if(set)
    {
        u8DiagI2cFaultStatus |= u8DiagI2cFaultMask;
    }else{
        u8DiagI2cFaultStatus &= ~u8DiagI2cFaultMask;
    }
    RegisterApp_DHU_Setup(CMD_DTC,DTC_I2CM_FAULT,u8DiagI2cFaultStatus);
}

DiagIO FAULT_LED;
DiagIO FAULT_LCD;
DiagIO FAULT_BIAS;
DiagIO STATUS_LOCK;
DiagIO STATUS_FPC;
void DiagApp_CheckFlowInitial()
{
    FAULT_LED.Status = IO_STATUS_SWIM;
    FAULT_LED.Port = LED_FAULT_PORT;
    FAULT_LED.PortNumber = LED_FAULT_PIN;
    FAULT_LED.Threshlod = 4;
    FAULT_LED.ConsecutiveHighCnt =  0;
    FAULT_LED.ConsecutiveLowCnt = 0;
    FAULT_LED.Report = true;

    FAULT_LCD.Status = IO_STATUS_SWIM;
    FAULT_LCD.Port = DISP_FAULT_PORT;
    FAULT_LCD.PortNumber = DISP_FAULT_PIN;
    FAULT_LCD.Threshlod = 4;
    FAULT_LCD.ConsecutiveHighCnt =  0;
    FAULT_LCD.ConsecutiveLowCnt = 0;
    FAULT_LCD.Report = true;

    FAULT_BIAS.Status = IO_STATUS_SWIM;
    FAULT_BIAS.Port = BIAS_FAULT_PORT;
    FAULT_BIAS.PortNumber = BIAS_FAULT_PIN;
    FAULT_BIAS.Threshlod = 5;
    FAULT_BIAS.ConsecutiveHighCnt =  0;
    FAULT_BIAS.ConsecutiveLowCnt = 0;
    FAULT_BIAS.Report = true;

    STATUS_FPC.Status = IO_STATUS_SWIM;
    STATUS_FPC.Port = FPC_DET_RX_OUT_PORT;
    STATUS_FPC.PortNumber = FPC_DET_RX_OUT_PIN;
    STATUS_FPC.Threshlod = 5;
    STATUS_FPC.ConsecutiveHighCnt = 0;
    STATUS_FPC.ConsecutiveLowCnt = 0;

    STATUS_LOCK.Status = IO_STATUS_SWIM;
    STATUS_LOCK.Port = DES_LOCK_PORT;
    STATUS_LOCK.PortNumber = DES_LOCK_PIN;
    STATUS_LOCK.Threshlod = 4;
    STATUS_LOCK.ConsecutiveHighCnt = 0;
    STATUS_LOCK.ConsecutiveLowCnt = 0;
}

void DiagApp_LcdFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_LCD);
    if(IO_STATUS_HIGH == u8Status1){
        if(IO_HIGH == PortDrvier_PinRead(DISP_FAULT_PORT,DISP_FAULT_PIN))
        {
            DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        }else{/* Do Nothing*/}
        DiagApp_RtnRstRequestCheck(false,DIAG_RST_LCD_MASK);
        FAULT_LCD.Report = true;
    }else if(IO_STATUS_LOW == u8Status1){
        if(FAULT_LCD.Report == true)
        {
            DisplayChipApp_FaultCheck();
            FAULT_LCD.Report = false;
        }
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        DiagApp_RtnRstRequestCheck(true,DIAG_RST_LCD_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        FAULT_LCD.Report = true;
    }
    sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LCD 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_LedFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_LED);
    if(IO_STATUS_HIGH == u8Status1){
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        DiagApp_RtnRstRequestCheck(false,DIAG_RST_LED_MASK);
        FAULT_LED.Report = true;
    }else if(IO_STATUS_LOW == u8Status1){
        if(FAULT_LED.Report == true)
        {
            PowerApp_LP8664_FaultCheck();
            FAULT_LED.Report = false;
        }
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        DiagApp_RtnRstRequestCheck(true,DIAG_RST_LED_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        FAULT_LED.Report = true;
    }
    sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LED 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_BiasFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    uint8_t u8Status2 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_BIAS);
    if(IO_STATUS_HIGH == u8Status1){
        /* Recovery mechanism merge to LCD FAULT
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        */
       DiagApp_RtnRstRequestCheck(false,DIAG_RST_BIAS_MASK);
       FAULT_BIAS.Report = true;
    }else if(IO_STATUS_LOW == u8Status1){
        /* Get error info & latch disp status bit*/
        if(FAULT_BIAS.Report == true)
        {
            PowerApp_RTQ6749_FaultCheck();
            FAULT_BIAS.Report = false;
        }
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        DiagApp_RtnRstRequestCheck(true,DIAG_RST_BIAS_MASK);
    }else{
        FAULT_BIAS.Report = true;
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxBuffer,"FAULT CHECK FLOW> LED 0x%02x LCD 0x%02x\r\n",u8Status1,u8Status2);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_FpcCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&STATUS_FPC);
    if(IO_STATUS_HIGH == u8Status1){
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_DCERR_MASK);
    }else if(IO_STATUS_LOW == u8Status1){
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_DCERR_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxBuffer,"FPC CHECK FLOW> STATUS_RFPC 0x%02x,0x%02x\r\n",u8Status1,STATUS_FPC.ConsecutiveLowCnt);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_LockCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&STATUS_LOCK);
    if(IO_STATUS_HIGH == u8Status1){
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_LLOSS_MASK);
    }else if(IO_STATUS_LOW == u8Status1){
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LLOSS_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxBuffer,"LOCK CHECK FLOW> STATUS_LOCK 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}
