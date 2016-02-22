//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------
#include "MockIO.h"

namespace IO {

IO *gMockIoPtr;

} // namespace IO


void IO_Write(IoAddress addr, IoData value)
{
    // mock_c()->actualCall("IO_Write")
    //         ->withIntParameters("addr", addr)
    //         ->withIntParameters("value", value);
}

IoData IO_Read(IoAddress addr)
{
   //  mock_c()->actualCall("IO_Read")
   //          ->withIntParameters("addr", addr);
   //
   // return mock_c()->returnValue().value.intValue;
   return 0U;
}
