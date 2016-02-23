//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "MockIO.h"

namespace IO {

IO *gMockIoPtr = nullptr;

} // namespace IO


void IO_Write(IoAddress addr, IoData value)
{
    IO::gMockIoPtr->Write(addr, value);
}

IoData IO_Read(IoAddress addr)
{
   return IO::gMockIoPtr->Read(addr);
}
