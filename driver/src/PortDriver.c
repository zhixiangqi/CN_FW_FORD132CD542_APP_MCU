#include "driver/inc/PortDriver.h"
//LDRA_EXCLUDE_START 496 S
uint8_t PortDrvier_PinRead(const GPIO_PRT_Type *base, uint32_t u32Pin)
{
    uint32_t u32DM = Cy_GPIO_GetDrivemode(base,u32Pin) & 0x0FU;
    uint8_t u8rtn = 0xFFU;
    if ((u32DM != 0x00UL) && (u32DM != 0x08UL))
    {
        if((u32DM & 0x0FUL) < 0x08UL)
        {
            u8rtn = (uint8_t)(Cy_GPIO_Read(base,u32Pin) & 0xFFU);
        }else if((u32DM & 0x0FUL) > 0x08U){
            u8rtn = (uint8_t)(Cy_GPIO_ReadOut(base,u32Pin) & 0xFFU);
        }else{
            u8rtn = 0xFAU;
        }
    }else{
        u8rtn = 0xFAU;
    }
    return u8rtn;
}

void PortDriver_PinWrite(GPIO_PRT_Type *base, uint32_t u32Pin, uint32_t value)
{
    Cy_GPIO_Write(base,u32Pin,value);
}

void PortDriver_PinSet(GPIO_PRT_Type *base, uint32_t u32Pin)
{
    Cy_GPIO_Set(base,u32Pin);
}

void PortDriver_PinClear(GPIO_PRT_Type *base, uint32_t u32Pin)
{
    Cy_GPIO_Clr(base,u32Pin);
}

void PortDriver_PinToggle(GPIO_PRT_Type *base, uint32_t u32Pin)
{
    Cy_GPIO_Inv(base,u32Pin);
}
//LDRA_EXCLUDE_END 496 S