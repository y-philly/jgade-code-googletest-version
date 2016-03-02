//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------
#ifndef _DEVICE_LIGHT_DRIVER_H_
#define _DEVICE_LIGHT_DRIVER_H_

#include "gmock/gmock.h"
#include "Device/LightDriver.h"

namespace Device {

class LightDriver {
public:
    virtual ~LightDriver() {}
    virtual void Destroy(::LightDriver self) = 0;
    virtual void TurnOn(::LightDriver self) = 0;
    virtual void TurnOff(::LightDriver self) = 0;
    virtual const char * GetType(::LightDriver driver) = 0;
    virtual int GetId(::LightDriver driver) = 0;
};

class MockLightDriver: public LightDriver {
public:
    MOCK_METHOD1(Destroy, void(::LightDriver self));
    MOCK_METHOD1(TurnOn, void(::LightDriver self));
    MOCK_METHOD1(TurnOff, void(::LightDriver self));
    MOCK_METHOD1(GetType, const char * (::LightDriver driver));
    MOCK_METHOD1(GetId, int(::LightDriver driver));
};

extern LightDriver *gLightDriverPtr;

} // namespace Device

#endif  // _DEVICE_LIGHT_DRIVER_H_
