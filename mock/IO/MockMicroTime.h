//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#ifndef _IO_MOCK_MICRO_TIME_H_
#define _IO_MOCK_MICRO_TIME_H_

#include <cstdint>
#include "gmock/gmock.h"
#include "IO/MicroTime.h"

namespace IO {

class MicroTime {
public:
    virtual ~MicroTime() {}
    virtual uint32_t Get() = 0;
    virtual void Delay(uint32_t delay) = 0;
};

class MockIO: public MicroTime {
public:
    MOCK_METHOD0(Get, uint32_t());
    MOCK_METHOD1(Delay, void(uint32_t delay));
};

extern MicroTime *gMockMicroTimePtr;

} // namespace IO



#endif  // _IO_MOCK_MICRO_TIME_H_
