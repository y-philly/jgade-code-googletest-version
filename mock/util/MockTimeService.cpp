//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "util/MockTimeService.h"

namespace util {

TimeService *gMockTimeServicePtr = nullptr;

} // namespace util


void TimeService_Create(void)
{
    util::gMockTimeServicePtr->Create();
}

void TimeService_Destroy(void)
{
    util::gMockTimeServicePtr->Destory();
}

int TimeService_GetMinute(void)
{
    return util::gMockTimeServicePtr->GetMinute();
}

int TimeService_GetDay(void)
{
    return util::gMockTimeServicePtr->GetDay();
}

void TimeService_GetTime(Time *timePtr)
{
    util::gMockTimeServicePtr->GetTime(timePtr);
}

bool Time_MatchesDayOfWeek(Time *timePtr, Day day)
{
    return util::gMockTimeServicePtr->MatchesDayOfWeek(timePtr, day);
}

bool Time_MatchesMinuteOfDay(Time *timePtr, int minute)
{
    return util::gMockTimeServicePtr->MatchesMinuteOfDay(timePtr, minute);
}
