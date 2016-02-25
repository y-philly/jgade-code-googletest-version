//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "util/MockRunTimeError.h"

namespace util {

RuntimeError *gMockRuntimerErrorPtr;

} // namespace util


void RuntimeError(const char * message,
                  int parameter,
                  const char * file,
                  int line)
{
    std::string m{ message }, f{ file };

    util::gMockRuntimerErrorPtr->AddLog(m, parameter, f, line);
}
