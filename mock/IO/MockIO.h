//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------
#ifndef _IO_MOCK_IO_H_
#define _IO_MOCK_IO_H_

#include "gmock/gmock.h"
#include "IO/Io.h"

namespace IO {

class IO {
public:
    virtual ~IO() {}
    virtual IoData Read(IoAddress offset) = 0;
    virtual void Write(IoAddress offset, IoData data) = 0;
};

class MockIO: public IO {
public:
    MOCK_METHOD1(Read, IoData(IoAddress offset));
    MOCK_METHOD2(Write, void(IoAddress offset, IoData data));
};

extern IO *gMockIoPtr;

} // namespace IO

#endif  // _IO_MOCK_IO_H_
