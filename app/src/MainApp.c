/* ************************************************************************** */
/** Descriptive File Name
  @Company
    AUO
  @File Name
    MainApp.c
  @Summary
    Start state machine with function - Power Management APP.
  @Description
    RUN Main APP with State .
 **/
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "app/inc/MainApp.h"
#include "app/inc/TC0App.h"
#include "app/inc/StackTaskApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/BacklightApp.h"
#include "app/inc/I2C2SlaveApp.h"
#include "app/inc/INTBApp.h"
#include "app/inc/DeviceApp.h"
#include "app/inc/WdtApp.h"
#include "app/inc/PowerApp.h"
#include "app/inc/FlashApp.h"
#include "app/inc/TDDIApp.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/AdcDriver.h"
#include "driver/inc/I2C4MDriver.h"
#include "driver/inc/PwmDriver.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/EicDriver.h"


#define CY_ASSERT_FAILED          (0u)
#define SYNC_VOLT_SAMPLE_CNT      3U
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

static uint8_t u8MAIN_STATUS = STATE_BOOT;
static uint8_t u8TxBuffer[60] = {0};

uint16_t u16SyncVoltSample[SYNC_VOLT_SAMPLE_CNT] = {0U};
uint8_t u8SYNCSampleCount = 0U;
bool u8SYNCSampleReady = FALSE;
/*  Function: MainApp_Boot_Mode
**  Callfrom: Main_Flow state machine
**        Do: Do basic initial like PORT/CLOCK and other mustbe function.
**        Go: Go back to SLEEP mode if WAKEUP PIN is LOW, otherwise go to PRENORMAL mode for fully initial.
 */
static uint8_t MainApp_Boot_Mode(uint8_t u8Nothing)
{
    uint8_t u8Return;
    uint32_t result = 0U;
    /* Initialize the device and board peripherals */
    uint32_t PC = (uint32_t)(&MainApp_Task);
    result = cybsp_init();
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT0, 4U, CY_GPIO_DM_ANALOG, 0x00U, HSIOM_SEL_GPIO);
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT0, 5U, CY_GPIO_DM_ANALOG, 0x00U, HSIOM_SEL_GPIO);
    Cy_SysClk_WcoBypass(false);
    /* Cypress WCO chip spec request 500ms for TS START (set as 500000UL)*/
    Cy_SysClk_WcoEnable(1000UL);

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        //CY_ASSERT(CY_ASSERT_FAILED);
        u8Return = STATE_PRESLEEP;
    }
    else{
        u8Return = STATE_PRENORMAL;
    }
    WdtApp_CleanCounter();
    /* Configure and enable the UART peripheral */
    UartDriver_Initial();
    TC0App_Initial();
    if(I2C4MDriver_Initialize() == false)
    {
        UartDriver_TxWriteString((uint8_t *)"I2C M driver init fail\r\n");
    }
    RegisterApp_ALL_Initial();
    RegisterApp_DHU_Setup(CMD_DISP_STATUS,1U,0xF3);
    I2C2SlaveApp_Initial();
    StackTaskApp_Global_MissionInitial();
    BacklightApp_Initial();
    DeviceApp_Intial();
    TC0App_DHUTaskClean();
    /* Enable global interrupts */
    __enable_irq();
    
    WdtApp_CheckResetCause();
    WdtApp_Initial();
    sprintf((char *)u8TxBuffer,"BOOT FINISHED, PC:0x%lX\r\n",PC);
    UartDriver_TxWriteString(u8TxBuffer);
    /* Only for flash w/r test*/
    uint8_t Flag[4] = {0x0A, 0x00, 0x00, 0x00};
    FlashApp_WriteRowFlash(&Flag[0],0x0001F000,4U);
    (void) u8Nothing;
    return u8Return;
}

/*  Function: MainApp_PreNormal_Mode
**  Callfrom: Main_Flow state machine
**        Do: Do all initial like SERCOM/ADC/TC and other mustbe function.
**        Go: Strightly go into NORMAL mode.
 */
static uint8_t MainApp_PreNormal_Mode(uint8_t u8Nothing)
{
    WdtApp_CleanCounter();
    /*ADC initial*/
    AdcDriver_Initial(ADC_SAR0_TYPE, ADC_SAR0_CONFIG);
    /*EIC initial*/
    EicDriver_Initial();
    PowerApp_PowerGoodInitial();
    PowerApp_Sequence(POWER_ON);
    /*Do LCD Power On Sequence*/
    sprintf((char *)u8TxBuffer,"PRENORMAL FINISHED\r\n");
    UartDriver_TxWriteString(u8TxBuffer);
    /* Need to put at the end of prenormal task*/
    TC0App_NormalWorkStartSet(TRUE);
    (void) u8Nothing;
    return STATE_HANDSHAKE;
}

/*  Function: MainApp_Normal_Mode
**  Callfrom: Main_Flow state machine
**        Do: Do most time function.
**        Go: Strightly go into NORMAL mode.
 */
static uint8_t MainApp_HandShake_Mode(uint8_t u8Nothing)
{
    uint8_t u8Return;
    WdtApp_CleanCounter();
    /* Check Disp En Cmd*/
    if(RegisterApp_DHU_Read(CMD_DISP_EN,CMD_DATA_POS) == 0x01U)
    {
        u8Return = STATE_NORMAL;
        sprintf((char *)u8TxBuffer,"HANDSHAKE FINISHED\r\n");
    }else{
        u8Return = STATE_HANDSHAKE;
        sprintf((char *)u8TxBuffer,"WAIT HANDSHAKE\r\n");
    }
    /* Do or Check Handshake function*/
    UartDriver_TxWriteString(u8TxBuffer);
    /* Test Disp En Cmd -- need to be deleted*/
    RegisterApp_DHU_Setup(CMD_DISP_EN,CMD_DATA_POS,0x01U);
    (void) u8Nothing;
    return u8Return;
}

/*  Function: MainApp_Normal_Mode
**  Callfrom: Main_Flow state machine
**        Do: Do most time function.
**        Go: Strightly go into NORMAL mode.
 */
static uint8_t MainApp_Normal_Mode(uint8_t u8Nothing)
{
    uint8_t u8Return;
    uint16_t u16AdcSyncVolt;
    StackTaskApp_MissionAction();
    INTBApp_Flow();
    // sprintf((char *)u8TxBuffer,"NORMAL FINISHED 0x%02x\r\n",RegisterApp_DHU_Read(CMD_DISP_STATUS,0U));
    // UartDriver_TxWriteString(u8TxBuffer);
    /* Get SYNC Voltage */
    u16AdcSyncVolt = AdcDriver_ChannelResultGet(ADC_SAR0_TYPE, ADC_SAR0_CH3_SYNCVOLT);
    /* Do check the data base(samples) is ready for result output (SYNC_VOLT_SAMPLE_CNT = 3 times)*/
    u16SyncVoltSample[u8SYNCSampleCount] = u16AdcSyncVolt;
    if(u8SYNCSampleCount == (SYNC_VOLT_SAMPLE_CNT - 1U)){u8SYNCSampleReady = TRUE;}
    u8SYNCSampleCount = ((u8SYNCSampleCount + 1U) > (SYNC_VOLT_SAMPLE_CNT - 1U)) ? 0U : (u8SYNCSampleCount + 1U);
    if (u8SYNCSampleReady == TRUE)
    {
        u8SYNCSampleReady = FALSE;
        uint16_t u16SyncVolDebounce = 0U;
        for(uint8_t u8count = 0U; u8count < SYNC_VOLT_SAMPLE_CNT; u8count++)
        {
            u16SyncVolDebounce += u16SyncVoltSample[u8count]/SYNC_VOLT_SAMPLE_CNT;
        }
        if(((RegisterApp_DHU_Read(CMD_DISP_SHUTD,1U) & 0x01U) == 0x00U) && (u16SyncVolDebounce > 413 && u16SyncVolDebounce < 2114))
        {
            u8Return = STATE_NORMAL;
        }else{
            u8Return = STATE_PRESLEEP;
        }
    }
    else 
    {
        if((RegisterApp_DHU_Read(CMD_DISP_SHUTD,1U) & 0x01U) == 0x00U)
        {
            u8Return = STATE_NORMAL;
        }else{
            u8Return = STATE_PRESLEEP;
        }
    }
    WdtApp_CleanCounter();
    /* Test WDT timeout - ~3.2sec reset (ILO has 40Kz +/- 50%, so it should consider as 2.5)*/
    // TC0App_DelayMS(3000U);
    // WdtApp_CleanCounter();
    (void) u8Nothing;
    return u8Return;
}

/*  Function: MainApp_PreSleep_Mode
**  Callfrom: Main_Flow state machine
**        Do: De-Init Driver & Power Off Seq
**        Go: Strightly go into SLEEP mode.
 */
static uint8_t MainApp_PreSleep_Mode(uint8_t u8Nothing)
{
    uint8_t u8Return;
    WdtApp_CleanCounter();
    /* Do LCD Power Off Sequence*/
    INTBApp_InitSwitch(INTB_DEINITIAL);
    PowerApp_Sequence(POWER_OFF);
    sprintf((char *)u8TxBuffer,"PRESLEEP FINISHED\r\n");
    UartDriver_TxWriteString(u8TxBuffer);
    u8Return = STATE_SLEEP;
    (void) u8Nothing;
    return u8Return;
}

/*  Function: MainApp_Sleep_Mode
**  Callfrom: Main_Flow state machine
**        Do: ShutDown MCU / EN_DET CHECK
**        Go: Keep Sleep Mode until EN_DET is High (Before Power is Down)
 */
static uint8_t MainApp_Sleep_Mode(uint8_t u8Nothing)
{
    uint8_t u8Return;
    uint16_t u16AdcSyncVolt;
    // WdtApp_CleanCounter();
    INTBApp_Flow();
    /* Do Power Off Sequence*/
    sprintf((char *)u8TxBuffer,"SLEEP FINISHED\r\n");
    UartDriver_TxWriteString(u8TxBuffer);
    u8Return = STATE_SLEEP;
    (void) u8Nothing;
    TC0App_DelayMS(200U);
    /* Get SYNC Voltage */
    u16AdcSyncVolt = AdcDriver_ChannelResultGet(ADC_SAR0_TYPE, ADC_SAR0_CH3_SYNCVOLT);
    /* Do check the data base(samples) is ready for result output (SYNC_VOLT_SAMPLE_CNT = 3 times)*/
    u16SyncVoltSample[u8SYNCSampleCount] = u16AdcSyncVolt;
    if(u8SYNCSampleCount == (SYNC_VOLT_SAMPLE_CNT - 1U)){u8SYNCSampleReady = TRUE;}
    u8SYNCSampleCount = ((u8SYNCSampleCount + 1U) > (SYNC_VOLT_SAMPLE_CNT - 1U)) ? 0U : (u8SYNCSampleCount + 1U);
    if (u8SYNCSampleReady == TRUE)
    {
        u8SYNCSampleReady = FALSE;
        uint16_t u16SyncVolDebounce = 0U;
        for(uint8_t u8count = 0U; u8count < SYNC_VOLT_SAMPLE_CNT; u8count++)
        {
            u16SyncVolDebounce += u16SyncVoltSample[u8count]/SYNC_VOLT_SAMPLE_CNT;
        }
        if(u16SyncVolDebounce > 413 && u16SyncVolDebounce < 2114)
        {
            u8Return = STATE_BOOT;
        }else{
            PortDriver_PinClear(HVLDO_EN_PORT,HVLDO_EN_PIN);
        }
    }
    if(u16AdcSyncVolt > 413 && u16AdcSyncVolt < 2114)
    {
        u8Return = STATE_BOOT;
    }else{
        PortDriver_PinClear(HVLDO_EN_PORT,HVLDO_EN_PIN);
    }
    return u8Return;
}

/*  Function: MainApp_Flow
**  Callfrom: MainApp_Task() at main.c
**        Do: Do State Machine Case
**        Go: loop state machine
 */
static uint8_t MainApp_Flow(uint8_t u8State)
{
    uint8_t u8CurrentState;
    
    switch(u8State)
    {
        case STATE_BOOT:
            u8CurrentState = MainApp_Boot_Mode(NOTHING);
        break;

        case STATE_PRENORMAL:
            u8CurrentState = MainApp_PreNormal_Mode(NOTHING);
        break;

        case STATE_HANDSHAKE:
            u8CurrentState = MainApp_HandShake_Mode(NOTHING);
        break;

        case STATE_NORMAL:
            u8CurrentState = MainApp_Normal_Mode(NOTHING);
        break;

        case STATE_PRESLEEP:
            u8CurrentState = MainApp_PreSleep_Mode(NOTHING);
        break;

        case STATE_SLEEP:
            u8CurrentState = MainApp_Sleep_Mode(NOTHING);;
        break;

        case STATE_WAKE_HOST:
            u8CurrentState = STATE_BOOT;
        break;

        default:
            u8CurrentState = STATE_BOOT;
        break;
    }

    return u8CurrentState;
}

uint8_t MainApp_Task(void)
{
    uint8_t u8Return = FALSE;
    uint8_t u8Status = u8MAIN_STATUS;
    u8Status = MainApp_Flow(u8Status);
    u8MAIN_STATUS = u8Status;
    
    if((u8Status != STATE_BOOT)
        && (u8Status != STATE_PRENORMAL)
        && (u8Status != STATE_NORMAL)
        && (u8Status != STATE_HANDSHAKE)
        && (u8Status != STATE_PRESLEEP)
        && (u8Status != STATE_SLEEP)
        && (u8Status != STATE_WAKE_HOST))
    {
        u8Return=FALSE;
    }
    else
    {
        u8Return=TRUE;
    }
    return u8Return;
}
/* *****************************************************************************
 End of File
 */
