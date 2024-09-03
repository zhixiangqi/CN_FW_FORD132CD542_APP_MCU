#ifndef APP_INC_StackTaskApp_H__
#define APP_INC_StackTaskApp_H__

#include "main.h"

#define StackTaskMaxSize 200U
#define MissionUnderFlow 0xFFU

#define MISSION_NUM_ONE     1U
#define MISSION_NUM_TWO     2U
#define MISSION_NUM_THREE   3U

#define TASK_DEBUGINFO  0U
#define TASK_MONITOR    1U
#define TASK_BLTFLOW    2U
#define TASK_DIMMING    3U
#define TASK_BATFLOW    4U
#define TASK_PWGFLOW    5U
#define TASK_LEDFLOW    6U
#define TASK_IOCHECK    7U
#define TASK_LCDFLOW    8U
#define TASK_TSCINTLOW   9U
#define TASK_SYNCCHECKLOW   10U
#define TASK_UPDATE_ERASE   0xE6U
#define TASK_UPDATE_TRANS   0xE7U
#define TASK_UPDATE_CRCSM   0xE8U
#define TASK_UPDATE_RESET   0xF8U

typedef struct{
    volatile uint8_t data[StackTaskMaxSize];
    volatile int top;
}Stack;

extern uint16_t u16SYNCVolatge;

uint8_t StackTaskApp_TaskNumberReturn(void);

void StackTaskApp_Global_MissionInitial(void);

void StackTaskApp_MissionPush(uint8_t u8TaskNumber);

void StackTaskApp_MissionAction(void);

#endif

