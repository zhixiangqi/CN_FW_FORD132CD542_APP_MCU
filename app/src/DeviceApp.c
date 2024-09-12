/* ************************************************************************** */
/** Descriptive File Name

  @Company
    AUO

  @File Name
    DeviceApp.c

  @Summary
    Show the SPSS part number information into register for Host requestion.

  @Description
    -Setup part number information at the start of MCU process (BOOT MODE).
    -Setup a back-door cmd for perpheral driver usage.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app/inc/DeviceApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/FlashApp.h"
#include "driver/inc/PortDriver.h"
#include "driver/inc/I2C4MDriver.h"

/*
**  Ascii to Hex Converter: https://zh-tw.rakko.tools/tools/77/
*/

#define NUM_DISP_ID    2U
static uint8_t DV_DISP_ID[NUM_DISP_ID] = {
    /* EXAMPLE: H1BT-14F180-FA*/
    0x3F,SUBREVISION_SW_VER
};
#define NUM_CORE_FPN    25U
static uint8_t DV_CORE_ASSB_FPN[NUM_CORE_FPN] = {
    /* EXAMPLE: H1BT-14F180-FA
    0x48,0x31,0x42,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x46,0x41
    */
    /* Ford 13.2 CD542: SB6T-14F180-AA*/
    0x53,0x42,0x36,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x41,0x41
};
#define NUM_DELIVER_FPN    25U
static uint8_t DV_DELIVER_ASSB_FPN[NUM_DELIVER_FPN] = {
    /* EXAMPLE: H1BT-14F180-FA
    0x48,0x31,0x42,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x46,0x41
    */
    /* Ford 13.2 CD542: SB6T-18B955-AA*/
    0x53,0x42,0x36,0x54,0x2d,0x31,0x38,0x42,0x39,0x35,0x35,0x2d,0x41,0x41
};
#define NUM_SW_FPN    25U
static uint8_t DV_SW_FPN[NUM_SW_FPN] = {
    /* EXAMPLE: H1BT-14F180-FA
    0x48,0x31,0x42,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x46,0x41
    */
    /* Ford 13.2 CD542: SB6T-14D358-AA*/
    0x53,0x42,0x36,0x54,0x2d,0x31,0x34,0x44,0x33,0x35,0x38,0x2d,0x41,0x41
};
#define NUM_SERIAL_FPN    25U
static uint8_t DV_SERIAL_FPN[NUM_SERIAL_FPN] = {
    /* EXAMPLE: H1BT-14F180-FA*/
    0x48,0x31,0x42,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x46,0x41
};
#define NUM_MAINCAL_FPN    25U
static uint8_t DV_MAINCAL_FPN[NUM_MAINCAL_FPN] = {
    /* EXAMPLE: H1BT-14F180-FA
    0x48,0x31,0x42,0x54,0x2d,0x31,0x34,0x46,0x31,0x38,0x30,0x2d,0x46,0x41
    */
    /* Ford 13.2 CD542: SB6T-14D359-AA*/
    0x53,0x42,0x36,0x54,0x2d,0x31,0x34,0x44,0x33,0x35,0x39,0x2d,0x41,0x41
};

void DeviceApp_Intial(void)
{
    uint8_t u8HW_Ver = PortDrvier_PinRead(PCBA_A0_PORT,PCBA_A0_PIN)
                     +(PortDrvier_PinRead(PCBA_A1_PORT,PCBA_A1_PIN)<<1)
                     +(PortDrvier_PinRead(PCBA_A2_PORT,PCBA_A2_PIN)<<2)
                     +(PortDrvier_PinRead(PCBA_A3_PORT,PCBA_A3_PIN)<<3);
    RegisterApp_DHU_Setup(CMD_DTC,DTC_SW_VERSION,SUBREVISION_SW_VER);
    RegisterApp_DHU_Setup(CMD_DTC,DTC_HW_VERSION,u8HW_Ver);
    for(uint16_t index=0;index<NUM_DISP_ID;index++)
    {
        RegisterApp_DHU_Setup(CMD_DISP_ID,CMD_DATA_POS+index,DV_DISP_ID[index]);
    }
    for(uint16_t index=0;index<NUM_CORE_FPN;index++)
    {
        RegisterApp_DHU_Setup(CMD_CORE_ASMB,CMD_DATA_POS+index,DV_CORE_ASSB_FPN[index]);
    }
    for(uint16_t index=0;index<NUM_DELIVER_FPN;index++)
    {
        RegisterApp_DHU_Setup(CMD_DELIVER_ASMB,CMD_DATA_POS+index,DV_DELIVER_ASSB_FPN[index]);
    }
    for(uint16_t index=0;index<NUM_SW_FPN;index++)
    {
        RegisterApp_DHU_Setup(CMD_SW_FPN,CMD_DATA_POS+index,DV_SW_FPN[index]);
    }
    for(uint16_t index=0;index<NUM_SERIAL_FPN;index++)
    {
        RegisterApp_DHU_Setup(CMD_SN,CMD_DATA_POS+index,DV_SERIAL_FPN[index]);
    }
    for(uint16_t index=0;index<NUM_MAINCAL_FPN;index++)
    {
        RegisterApp_DHU_Setup(CMD_MC_FPN,CMD_DATA_POS+index,DV_MAINCAL_FPN[index]);
    }
}

void DeviceApp_0xF1FabCommCtrl(void)
{
    uint8_t u8CommObject; /*OBJECT:SERCOM0~5...*/
    uint8_t u8CommType;   /*TYPE:Read,Write,Init,Deinit*/
    uint8_t u8CommAddr;   /*Address*/
    uint8_t u8CommLength; /*Length*/
    uint8_t TxBuff[256] = {0U};
    uint8_t RxBuff[256] = {0U};

    u8CommObject = RegisterApp_DHU_Read(CMD_FAB_CTRL, CMD_DATA_POS + 2U);
    u8CommType = RegisterApp_DHU_Read(CMD_FAB_CTRL, CMD_DATA_POS + 3U);
    u8CommAddr = RegisterApp_DHU_Read(CMD_FAB_CTRL, CMD_DATA_POS + 4U);
    u8CommLength = RegisterApp_DHU_Read(CMD_FAB_CTRL, CMD_DATA_POS + 5U);

    switch (u8CommType)
    {
    case CommType_WRITE:
        /* code */
        for(uint8_t index = 0U; index < u8CommLength; index++)
        {
            TxBuff[index] = RegisterApp_DHU_Read(CMD_FAB_CTRL,CMD_DATA_POS+2U+4U+index);
        }
        (void)I2C4MDriver_Write(u8CommAddr,TxBuff,u8CommLength);
        break;

    case CommType_READ:
        /* code */
        (void)I2C4MDriver_Read(u8CommAddr,RxBuff,u8CommLength);
        RegisterApp_DHU_Setup(0xF2U,0x00U,u8CommObject);
        RegisterApp_DHU_Setup(0xF2U,0x01U,u8CommAddr);
        RegisterApp_DHU_Setup(0xF2U,0x02U,u8CommLength);
        for(uint8_t index = 0U; index < u8CommLength; index++)
        {
            RegisterApp_DHU_Setup(0xF2U,index+3U,RxBuff[index]);
        }
        break;

    case CommType_INIT:
        /* code */
        I2C4MDriver_Initialize();
        break;

    case CommType_STOP:
        /* code */
        
        break;

    case CommType_UARTENABLE:
        /* code */
        
        break;

    case CommType_UARTDISABLE:
        /* code */
        
        break;

    case CommType_PortRead:
        /* code */
        RegisterApp_DHU_Setup(0xF2U,0x00U,0xFFU);
        RegisterApp_DHU_Setup(0xF2U,0x01U,u8CommAddr);
        RegisterApp_DHU_Setup(0xF2U,0x02U,0xFFU);
        RegisterApp_DHU_Setup(0xF2U,0x03U,PortDrvier_PinRead(((GPIO_PRT_Type*) &GPIO->PRT[(u8CommAddr>>4)]),(u8CommAddr&0x0F)));
        break;

    case CommType_PortSet:
        /* code */
        RegisterApp_DHU_Setup(0xF2U,0x00U,0xFFU);
        RegisterApp_DHU_Setup(0xF2U,0x01U,u8CommAddr);
        RegisterApp_DHU_Setup(0xF2U,0x02U,0xFFU);
        (void)PortDriver_PinSet(((GPIO_PRT_Type*) &GPIO->PRT[(u8CommAddr>>4)]),(u8CommAddr&0x0F));
        RegisterApp_DHU_Setup(0xF2U,0x03U,PortDrvier_PinRead(((GPIO_PRT_Type*) &GPIO->PRT[(u8CommAddr>>4)]),(u8CommAddr&0x0F)));
        break;

    case CommType_PortClean:
        /* code */
        RegisterApp_DHU_Setup(0xF2U,0x00U,0xFFU);
        RegisterApp_DHU_Setup(0xF2U,0x01U,u8CommAddr);
        RegisterApp_DHU_Setup(0xF2U,0x02U,0xFFU);
        (void)PortDriver_PinClear(((GPIO_PRT_Type*) &GPIO->PRT[(u8CommAddr>>4)]),(u8CommAddr&0x0F));
        RegisterApp_DHU_Setup(0xF2U,0x03U,PortDrvier_PinRead(((GPIO_PRT_Type*) &GPIO->PRT[(u8CommAddr>>4)]),(u8CommAddr&0x0F)));
        break;

    case CommType_FLASHWRITE:
        
        break;

    case CommType_FLASHREAD:
        
        break;

    default:
        // default
        break;
    }

    (void)u8CommObject;
    (void)u8CommAddr;
    (void)u8CommLength;
    (void)u8CommType;
    (void)TxBuff;
    (void)RxBuff;
}