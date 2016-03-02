//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#include "MockLightDriver.h"

namespace Device {

LightDriver *gLightDriverPtr = nullptr;

} // namespace IO


void LightDriver_Destroy(LightDriver self)
{
    Device::gLightDriverPtr->Destroy(self);
}

void LightDriver_TurnOn(LightDriver self)
{
    Device::gLightDriverPtr->TurnOn(self);
}

void LightDriver_TurnOff(LightDriver self)
{
    Device::gLightDriverPtr->TurnOff(self);
}

const char * LightDriver_GetType(LightDriver driver)
{
    return Device::gLightDriverPtr->GetType(driver);
}

int LightDriver_GetId(LightDriver driver)
{
    return Device::gLightDriverPtr->GetId(driver);
}
