//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#ifndef _UTIL_TIME_SERVICE_H_
#define _UTIL_TIME_SERVICE_H_

#include "gmock/gmock.h"
#include "util/TimeService.h"

namespace util {

class TimeService {
public:
    virtual ~TimeService() {}
    virtual void Create() = 0;
    virtual void Destory() = 0;
    virtual int GetMinute() = 0;
    virtual int GetDay() = 0;

    virtual void GetTime(Time *timePtr) = 0;

    virtual bool MatchesDayOfWeek(Time *timePtr, Day day) = 0;
    virtual bool MatchesMinuteOfDay(Time *timePtr, int minute) = 0;
};

class  MockTimeService : public TimeService {
public:
    MOCK_METHOD0(Create, void());
    MOCK_METHOD0(Destory, void());
    MOCK_METHOD0(GetMinute, int());
    MOCK_METHOD0(GetDay, int());

    MOCK_METHOD1(GetTime, void(Time *timePtr));

    MOCK_METHOD2(MatchesDayOfWeek, bool(Time *timePtr, Day day));
    MOCK_METHOD2(MatchesMinuteOfDay, bool(Time *timePtr, int minute));
};

extern TimeService *gMockTimeServicePtr;

} // namespace util

#endif  // _UTIL_RUNTIME_ERROR_H_
