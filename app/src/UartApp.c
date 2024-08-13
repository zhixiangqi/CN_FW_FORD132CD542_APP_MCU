#include "app/inc/UartApp.h"
#include "app/inc/StackTaskApp.h"
#include "driver/inc/I2C4MDriver.h"
#include "driver/inc/UartDriver.h"
#include "driver/inc/PortDriver.h"

#define UART_MARK_POS   7U
#define UART_CMD_ADDR_POS       (UART_MARK_POS+1U)
#define UART_CMD_W_DATA_POS     (UART_CMD_ADDR_POS+1U)
#define UART_CMD_R_DATA_POS     (UART_CMD_ADDR_POS+1U)
#define UART_CMD_WR_LEN_POS     (UART_CMD_ADDR_POS+1U)
#define UART_CMD_WR_DATA_POS    (UART_CMD_ADDR_POS+2U)
#define UART_CTRL_SET_POS       8U
#define UART_CTRL_PORT_POS      9U
#define UART_CTRL_PIN_POS       10U

bool UartApp_CompareBuffer(char *StringSource, uint8_t rdBuffer[], uint8_t start_pos, uint8_t stop_pos)
{
    bool bresult = false;
    uint8_t u8buffer[255] = {0};
    if(start_pos >= stop_pos)
    {
        for (uint8_t index = 0U;index<(stop_pos-start_pos+1U);index++)
        {
            u8buffer[index] = rdBuffer[index+start_pos];
        }
        if (strcmp(StringSource,(char *)u8buffer) != 0x01U){
            bresult = false;
        }else{
            bresult = true;
        }
    }else{
        bresult = false;
    }
    return bresult;
}

void UartApp_ReadFlow()
{
    uint8_t rdBuffer[255] = {0};
    uint8_t u8ParseTxBuffer[255] = {0};
    uint8_t u8ParseRxBuffer[255] = {0};
    uint8_t u8CmdLength = 0U;
    uint8_t result = UART_RX_EMPTY;
    result = UartDriver_Receive(&rdBuffer[0],sizeof(rdBuffer));
    if(result == UART_SUCCESS)
    {
        if(rdBuffer[0] != 0x00U){
            if(rdBuffer[1]=='{' && rdBuffer[2]=='F' && rdBuffer[3]=='1' && rdBuffer[4]=='2' && rdBuffer[5]=='3' && rdBuffer[6]=='}')
            {
                switch (rdBuffer[UART_MARK_POS])
                {
                case 0x57U:
                    /* Write cmd code */
                    u8CmdLength = rdBuffer[0] - UART_CMD_W_DATA_POS;
                    for(uint8_t index = 0U; index < u8CmdLength;index++)
                    {
                        u8ParseTxBuffer[index] = rdBuffer[index+UART_CMD_W_DATA_POS];
                    }
                    I2C4MDriver_Write(rdBuffer[UART_CMD_ADDR_POS],&u8ParseTxBuffer[0],u8CmdLength);
                    break;

                case 0x52U:
                    /* Read cmd code */
                    I2C4MDriver_Read(rdBuffer[UART_CMD_ADDR_POS],&u8ParseRxBuffer[0],rdBuffer[UART_CMD_R_DATA_POS]);
                    break;
                case 0xFEU:
                    /* Control GPIO*/
                    uint8_t u8temp[1] = {0U};
                    if(rdBuffer[UART_CTRL_PORT_POS] < 7U && rdBuffer[UART_CTRL_PIN_POS] < 8U)
                    {
                        if (rdBuffer[UART_CTRL_SET_POS] == 0x01)
                        {
                            PortDriver_PinSet(((GPIO_PRT_Type*) &GPIO->PRT[rdBuffer[UART_CTRL_PORT_POS]]),(uint32_t)rdBuffer[UART_CTRL_PIN_POS]);
                        }else if(rdBuffer[UART_CTRL_SET_POS] == 0x02){
                            PortDriver_PinClear(((GPIO_PRT_Type*) &GPIO->PRT[rdBuffer[UART_CTRL_PORT_POS]]),(uint32_t)rdBuffer[UART_CTRL_PIN_POS]);
                        }else if(rdBuffer[UART_CTRL_SET_POS] == 0x03){
                            PortDriver_PinToggle(((GPIO_PRT_Type*) &GPIO->PRT[rdBuffer[UART_CTRL_PORT_POS]]),(uint32_t)rdBuffer[UART_CTRL_PIN_POS]);
                        }else if(rdBuffer[UART_CTRL_SET_POS] == 0x04){
                            u8temp[0] = (uint8_t)PortDrvier_PinRead(((GPIO_PRT_Type*) &GPIO->PRT[rdBuffer[UART_CTRL_PORT_POS]]),(uint32_t)rdBuffer[UART_CTRL_PIN_POS]);
                            UartDriver_TxWriteString((uint8_t *)"\r\nGPIO Read : ");
                            UartDriver_TxWriteArray(u8temp,1U);
                            UartDriver_TxWriteString((uint8_t *)"\r\n");
                        }else{
                            /* Do nothing*/
                        }
                    }else{
                        /* Do nothing*/
                    }
                    break;
                case 0xFFU:
                    /* Write-Read code */
                    if(rdBuffer[0] > UART_CMD_WR_DATA_POS)
                    {
                        u8CmdLength = rdBuffer[0] - UART_CMD_WR_DATA_POS;
                        for(uint8_t index = 0U; index < u8CmdLength;index++)
                        {
                            u8ParseTxBuffer[index] = rdBuffer[index+UART_CMD_WR_DATA_POS];
                        }
                        I2C4MDriver_WriteRead(rdBuffer[UART_CMD_ADDR_POS],&u8ParseTxBuffer[0],u8CmdLength,u8ParseRxBuffer,rdBuffer[UART_CMD_WR_LEN_POS]);
                        if(rdBuffer[UART_CMD_WR_LEN_POS] > 0U)
                        {
                            UartDriver_TxWriteString((uint8_t *)"\r\nI2C Master Read : ");
                            UartDriver_TxWriteArray(u8ParseRxBuffer,(uint32_t)rdBuffer[UART_CMD_WR_LEN_POS]);
                            UartDriver_TxWriteString((uint8_t *)"\r\n");
                        }else{
                            /* No read need*/
                        }
                    }else{
                        /* No read need*/
                    }
                    break;

                default:
                    UartDriver_TxWriteString((uint8_t *)"Wrong Uart Message!\r\n");
                    break;
                }
            }
        }
    }else{
        /* No data received*/
    }
    /* Clean all data buffer*/
    UartDriver_AbortReceive();
}
