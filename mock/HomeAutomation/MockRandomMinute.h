//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#ifndef _HOME_AUTOMATION_RAMDOM_MINUTE_H_
#define _HOME_AUTOMATION_RAMDOM_MINUTE_H_

#include "gmock/gmock.h"
#include "HomeAutomation/RandomMinute.h"

namespace HomeAutomation {

class RandomMinute {
public:
    virtual ~RandomMinute() {}
    virtual void Create() = 0;
    virtual int Get() = 0;
};

class MockRandomMinute : public RandomMinute {
public:
    MOCK_METHOD0(Create, void());
    MOCK_METHOD0(Get, int());
};

extern RandomMinute *gMockRandomMinutePtr;

} // namespace HomeAutomation


#endif  // _HOME_AUTOMATION_RAMDOM_MINUTE_H_
