//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#ifndef _UTIL_RUNTIME_ERROR_H_
#define _UTIL_RUNTIME_ERROR_H_

#include <string>
#include "gmock/gmock.h"
#include "util/RuntimeError.h"

namespace util {

class RuntimeError {
public:
    virtual ~RuntimeError() {}
    virtual void AddLog(const std::string &message,
                        int parameter,
                        const std::string &file,
                        int line) = 0;
};

class MockRunuimeError : public RuntimeError {
public:
    MOCK_METHOD4(AddLog, void(const std::string &message,
                              int parameter,
                              const std::string &file,
                              int line));
};

extern RuntimeError *gMockRuntimerErrorPtr;

} // namespace util

#endif  // _UTIL_RUNTIME_ERROR_H_
