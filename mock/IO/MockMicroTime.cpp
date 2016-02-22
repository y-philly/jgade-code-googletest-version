//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "IO/MockMicroTime.h"

uint32_t MicroTime_Get(void)
{
    return 0U;
}

#if 0
static uint32_t time = 0;
static uint32_t increment = 1;
static uint32_t totalDelay = 0;

void FakeMicroTime_Init(uint32_t start, uint32_t incr)
{
    time = start;
    increment = incr;
    totalDelay = 0;
}
uint32_t MicroTime_Get(void)
{
    uint32_t t = time;
    time += increment;
    return t;
}

void MicroTime_Delay(uint32_t delay)
{
    time += delay;
    totalDelay += delay;
}

uint32_t FakeMicroTime_GetDelayDuration(void)
{
    return totalDelay;
}
#endif
