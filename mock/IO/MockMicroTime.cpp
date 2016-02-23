//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "IO/MockMicroTime.h"

namespace IO {

MicroTime *gMockMicroTimePtr;

} // namespace IO


uint32_t MicroTime_Get(void)
{
    return IO::gMockMicroTimePtr->Get();
}

void MicroTime_Delay(uint32_t delay)
{
    IO::gMockMicroTimePtr->Delay(delay);
}
