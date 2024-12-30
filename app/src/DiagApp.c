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
#include "app/inc/FlashApp.h"
#include "app/inc/ExternFlashApp.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/UartDriver.h"

static uint8_t u8DiagDispByte0 = 0x00U;
static uint8_t u8DiagDispByte1 = 0x01U;
static uint8_t u8TxDiagBuffer[80] = {0};
static uint8_t u8DiagRstReqStatus = 0x00U;
static uint8_t u8DiagIsrStatus = 0x00U;
static uint8_t u8DiagI2cFaultStatus = 0x00U;

uint8_t u8DiagDeveFautSt = 0U;
uint8_t u8LatchMaskClearSt = 0U;
//LDRA_EXCLUDE_START 8 D
void DiagApp_DispStatusClear(uint8_t ByteNumber, uint8_t MaskValue)
{
    uint8_t u8OldByte0 = u8DiagDispByte0;
    uint8_t u8OldByte1 = u8DiagDispByte1;
    if(ByteNumber == 0x00U)
    {
        if ((MaskValue & DISP0_LATCHED_MASK) == MaskValue)
        {
            if (u8LatchMaskClearSt == 0U)
            {
                u8DiagDispByte0 &= ~MaskValue;
                RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS,u8DiagDispByte0); 
            }
        }else{
            u8DiagDispByte0 &= ~MaskValue;
            RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS,u8DiagDispByte0); 
        }
    }else if(ByteNumber == 0x01U)
    {
        if ((MaskValue & DISP1_LATCHED_MASK) == MaskValue)
        {
            if (u8LatchMaskClearSt == 0U)
            {
                u8DiagDispByte1 &= ~MaskValue;
                RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1); 
            }
        }else{
            u8DiagDispByte1 &= ~MaskValue;
            RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1);
        }
    }else{
        /*DO NOTHING*/
    }

    /* Check if the data is New event (Pull Request Necessary)*/
    if ((u8OldByte0 != u8DiagDispByte0) || 
        (u8OldByte1 != u8DiagDispByte1 ))
    {
        INTBApp_PullReqSetOrClear(INTB_REQ_SET);
        (void)DiagApp_RtnIsrCheck(true,INTB_INT_ERR_MASK);
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

    uint8_t u8DispEnState = RegisterApp_DHU_Read(CMD_DISP_EN,CMD_DATA_POS);
    /* Check DISP_ST is correct*/
    if((u8DispEnState & DISPEN_DISP_MASK)  == DISPEN_DISP_MASK){
        if(((u8DiagDispByte0 & DISP0_DPSTOK_MASK) != 0x00U) || ((u8DiagDispByte1 & DISP1_DPSTOK_MASK) != 0x00U)){
            /* Set DISP_ST as 0*/
            u8DiagDispByte1 &= ~(uint8_t)DISP1_DISPST_MASK;
        }else{
            /* Set DISP_ST as 1*/
            u8DiagDispByte1 |= DISP1_DISPST_MASK;
        }
        RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1);
    }else{
        /*DO NOTHING*/
    }

    /* Check TSC_ST is correct*/
    if((u8DispEnState & DISPEN_DISP_MASK)  == DISPEN_DISP_MASK){
        if((u8DispEnState & DISPEN_TSC_MASK)  == DISPEN_TSC_MASK){
            if(((u8DiagDispByte0 & DISP0_TSCTOK_MASK) != 0x00U) || ((u8DiagDispByte1 & DISP1_DPSTOK_MASK) != 0x00U)){
                /* Set TSC_ST as 0*/
                u8DiagDispByte1 &= ~(uint8_t)DISP1_TSCST_MASK;
            }else{
                /* Set TSC_ST as 1*/
                u8DiagDispByte1 |= DISP1_TSCST_MASK;
            }
            RegisterApp_DHU_Setup(CMD_DISP_STATUS,CMD_DATA_POS+1U,u8DiagDispByte1);
        }else{
            /*DO NOTHING*/
        }
    }else{
        /*DO NOTHING*/
    }

    /* Check if the data is New event (Pull Request Necessary)*/
    if ((u8OldByte0 != u8DiagDispByte0) || 
        (u8OldByte1 != u8DiagDispByte1 ))
    {
        if (((u8OldByte0 & DISP0_LATCHED_MASK) != (u8DiagDispByte0 & DISP0_LATCHED_MASK)) || 
            ((u8OldByte1 & DISP1_LATCHED_MASK) != (u8DiagDispByte1 & DISP1_LATCHED_MASK)))
        {
            u8LatchMaskClearSt = 1U;
        }
        INTBApp_PullReqSetOrClear(INTB_REQ_SET);
        (void)DiagApp_RtnIsrCheck(true,INTB_INT_ERR_MASK);
        /* Only for Nor Flash Test*/
        ExternFlashApp_Write();
    }
    (void)u8OldByte0;
    (void)u8OldByte1;
}

uint8_t DiagApp_ConsecutiveCheckIO(DiagIO* ds1)
{
    DiagIO ds = *ds1;
    if (IO_HIGH == PortDrvier_PinRead(ds.Port,ds.PortNumber)){
        ds.ConsecutiveHighCnt += 1U;
        ds.ConsecutiveLowCnt = 0U;
    }else{
        ds.ConsecutiveHighCnt = 0U;
        ds.ConsecutiveLowCnt += 1U;
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
    uint8_t ResultIOStatus=ds.Status;
    return ResultIOStatus;
}

uint8_t DiagApp_ConsecutiveCheckRegister(DiagIO* ds1,bool isgood)
{
    DiagIO ds = *ds1;
    if (true == isgood){
        ds.ConsecutiveHighCnt += 1U;
        ds.ConsecutiveLowCnt = 0U;
    }else{
        ds.ConsecutiveHighCnt = 0U;
        ds.ConsecutiveLowCnt += 1U;
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
    uint8_t ResultStatus = ds.Status;
    return ResultStatus;
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
DiagIO STATUS_LFPC;
DiagIO STATUS_RFPC;
void DiagApp_CheckFlowInitial(void)
{
    FAULT_LED.Status = IO_STATUS_SWIM;
    FAULT_LED.Port = LED_FAULT_PORT;
    FAULT_LED.PortNumber = LED_FAULT_PIN;
    FAULT_LED.Threshlod = 4U;
    FAULT_LED.ConsecutiveHighCnt =  0U;
    FAULT_LED.ConsecutiveLowCnt = 0U;
    FAULT_LED.Report = true;

    FAULT_LCD.Status = IO_STATUS_SWIM;
    FAULT_LCD.Port = DISP_FAULT_PORT;
    FAULT_LCD.PortNumber = DISP_FAULT_PIN;
    FAULT_LCD.Threshlod = 4U;
    FAULT_LCD.ConsecutiveHighCnt =  0U;
    FAULT_LCD.ConsecutiveLowCnt = 0U;
    FAULT_LCD.Report = true;

    FAULT_BIAS.Status = IO_STATUS_SWIM;
    FAULT_BIAS.Port = BIAS_FAULT_PORT;
    FAULT_BIAS.PortNumber = BIAS_FAULT_PIN;
    FAULT_BIAS.Threshlod = 5U;
    FAULT_BIAS.ConsecutiveHighCnt =  0U;
    FAULT_BIAS.ConsecutiveLowCnt = 0U;
    FAULT_BIAS.Report = true;

    STATUS_LFPC.Status = IO_STATUS_SWIM;
    STATUS_LFPC.Port = FPC_DET_TX_LOUT_PORT;
    STATUS_LFPC.PortNumber = FPC_DET_TX_LOUT_PIN;
    STATUS_LFPC.Threshlod = 5U;
    STATUS_LFPC.ConsecutiveHighCnt = 0U;
    STATUS_LFPC.ConsecutiveLowCnt = 0U;

    STATUS_RFPC.Status = IO_STATUS_SWIM;
    STATUS_RFPC.Port = FPC_DET_RX_ROUT_PORT;
    STATUS_RFPC.PortNumber = FPC_DET_RX_ROUT_PIN;
    STATUS_RFPC.Threshlod = 5U;
    STATUS_RFPC.ConsecutiveHighCnt = 0U;
    STATUS_RFPC.ConsecutiveLowCnt = 0U;

    STATUS_LOCK.Status = IO_STATUS_SWIM;
    STATUS_LOCK.Port = DES_LOCK_PORT;
    STATUS_LOCK.PortNumber = DES_LOCK_PIN;
    STATUS_LOCK.Threshlod = 4U;
    STATUS_LOCK.ConsecutiveHighCnt = 0U;
    STATUS_LOCK.ConsecutiveLowCnt = 0U;
}
//LDRA_EXCLUDE_START 139 S
void DiagApp_LcdFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_LCD);
    if((IO_STATUS_HIGH == u8Status1) && (IO_STATUS_HIGH == u8PoweI2cFaultSt)){
        u8DiagDeveFautSt = IO_STATUS_HIGH;
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_LCD_MASK);
        FAULT_LCD.Report = true;
    }else if(IO_STATUS_LOW == u8Status1){
        u8DiagDeveFautSt = IO_STATUS_LOW;
        if(FAULT_LCD.Report == true)
        {
            /* Do RST_RQ mechanism in FaultCheck function*/
            DisplayChipApp_FaultCheck();
            FAULT_LCD.Report = false;
        }
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_LCDERR_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
        FAULT_LCD.Report = true;
    }
    sprintf((char *)u8TxDiagBuffer,"FAULT CHECK FLOW> LCD 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_LedFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_LED);
    if((IO_STATUS_HIGH == u8Status1) && (IO_STATUS_HIGH == u8PoweI2cFaultSt)){
        u8DiagDeveFautSt = IO_STATUS_HIGH;
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_BLERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_LED_MASK);
    }else if(IO_STATUS_LOW == u8Status1){
        u8DiagDeveFautSt = IO_STATUS_LOW;
        PowerApp_LP8664_FaultCheck();
        FAULT_LED.Report = false;
    }else{
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxDiagBuffer,"FAULT CHECK FLOW> LED 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_BiasFaultCheckFlow(void)
{
    uint8_t u8Status1 = IO_STATUS_SWIM;
    uint8_t u8Status2 = IO_STATUS_SWIM;
    u8Status1 = DiagApp_ConsecutiveCheckIO(&FAULT_BIAS);
    if((IO_STATUS_HIGH == u8Status1) && (IO_STATUS_HIGH == u8PoweI2cFaultSt)){
        u8DiagDeveFautSt = IO_STATUS_HIGH;
        DiagApp_DispStatusClear(DISP_STATUS_BYTE1,DISP1_DISPERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(false,DIAG_RST_BIAS_MASK);
        FAULT_BIAS.Report = true;
    }else if(IO_STATUS_LOW == u8Status1){
        u8DiagDeveFautSt = IO_STATUS_LOW;
        /* Get error info & latch disp status bit*/
        if(FAULT_BIAS.Report == true)
        {
            PowerApp_RTQ6749_FaultCheck();
            FAULT_BIAS.Report = false;
        }
        DiagApp_DispStatusSet(DISP_STATUS_BYTE1,DISP1_DISPERR_MASK);
        (void)DiagApp_RtnRstRequestCheck(true,DIAG_RST_BIAS_MASK);
    }else{
        FAULT_BIAS.Report = true;
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxDiagBuffer,"FAULT CHECK FLOW> LED 0x%02x LCD 0x%02x\r\n",u8Status1,u8Status2);
    //UartDriver_TxWriteString(u8TxBuffer);
}

void DiagApp_FpcCheckFlow(void)
{
    uint8_t u8StatusR = IO_STATUS_SWIM;
    uint8_t u8StatusL = IO_STATUS_SWIM;
    uint8_t u8Status1 = IO_STATUS_SWIM;
    u8StatusR = DiagApp_ConsecutiveCheckIO(&STATUS_RFPC);
    u8StatusL = DiagApp_ConsecutiveCheckIO(&STATUS_LFPC);
    u8Status1 = (u8StatusR & u8StatusL);
    if(IO_STATUS_SWIM == u8Status1){
        /* When voltage at swim state, Do nothing*/
    }else if(IO_STATUS_HIGH == (u8Status1 & IO_STATUS_HLMASK)){
        DiagApp_DispStatusClear(DISP_STATUS_BYTE0,DISP0_DCERR_MASK);
    }else if(IO_STATUS_LOW == (u8Status1 & IO_STATUS_HLMASK)){
        DiagApp_DispStatusSet(DISP_STATUS_BYTE0,DISP0_DCERR_MASK);
    }else{
        /* When voltage at swim state, Do nothing*/
    }
    sprintf((char *)u8TxDiagBuffer,"FPC CHECK FLOW> STATUS_RFPC 0x%02x,0x%02x STATUS_RFPC 0x%02x,0x%02x\r\n",u8StatusR,STATUS_RFPC.ConsecutiveLowCnt,u8StatusL,STATUS_LFPC.ConsecutiveLowCnt);
    RegisterApp_DHU_Setup(CMD_DTC,DTC_DET_FPCR,u8StatusR);
    RegisterApp_DHU_Setup(CMD_DTC,DTC_DET_FPCL,u8StatusL);
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
    sprintf((char *)u8TxDiagBuffer,"LOCK CHECK FLOW> STATUS_LOCK 0x%02x\r\n",u8Status1);
    //UartDriver_TxWriteString(u8TxBuffer);
}
//LDRA_EXCLUDE_END 139 S
//LDRA_EXCLUDE_END 8 D
