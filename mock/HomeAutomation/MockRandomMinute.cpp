//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "HomeAutomation/MockRandomMinute.h"

namespace HomeAutomation {

RandomMinute *gMockRandomMinutePtr = nullptr;

} // namespace HomeAutomation


void RandomMinute_Create(void)
{
    HomeAutomation::gMockRandomMinutePtr->Create();
}

int RandomMinute_Get(void)
{
    return HomeAutomation::gMockRandomMinutePtr->Get();
}
