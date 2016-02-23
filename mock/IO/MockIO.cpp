//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "MockIO.h"

namespace IO {

IO *gMockIOPtr = nullptr;

} // namespace IO


void IO_Write(IoAddress addr, IoData value)
{
    IO::gMockIOPtr->Write(addr, value);
}

IoData IO_Read(IoAddress addr)
{
   return IO::gMockIOPtr->Read(addr);
}
