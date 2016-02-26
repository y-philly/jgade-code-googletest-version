/***
 * Excerpted from "Test-Driven Development for Embedded C",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material,
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose.
 * Visit http://www.pragmaticprogrammer.com/titles/jgade for more book information.
***/
/*- ------------------------------------------------------------------ -*/
/*-    Copyright (c) James W. Grenning -- All Rights Reserved          -*/
/*-    For use by owners of Test-Driven Development for Embedded C,    -*/
/*-    and attendees of Renaissance Software Consulting, Co. training  -*/
/*-    classes.                                                        -*/
/*-                                                                    -*/
/*-    Available at http://pragprog.com/titles/jgade/                  -*/
/*-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3                -*/
/*-                                                                    -*/
/*-    Authorized users may use this source code in your own           -*/
/*-    projects, however the source code may not be used to            -*/
/*-    create training material, courses, books, articles, and         -*/
/*-    the like. We make no guarantees that this source code is        -*/
/*-    fit for any purpose.                                            -*/
/*-                                                                    -*/
/*-    www.renaissancesoftware.net james@renaissancesoftware.net       -*/
/*- ------------------------------------------------------------------ -*/
/*- ------------------------------------------------------------------ -*/
/*-    Modified by Yasuhiro SHIMIZU.                                   -*/
/*- ------------------------------------------------------------------ -*/


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "HomeAutomation/LightController.h"
#include "Device/LightDriver.h"

static LightDriver lightDrivers[MAX_LIGHTS] = {NULL};

void LightController_Create(void)
{
    memset(lightDrivers, 0, sizeof(lightDrivers));
}

void LightController_Destroy(void)
{
    int i;
    for (i = 0; i < MAX_LIGHTS; i++)
    {
        LightDriver driver = lightDrivers[i];
        LightDriver_Destroy(driver);
        lightDrivers[i] = NULL;
    }
}

static bool isIdInBounds(int id)
{
    return id < 0 || id >= MAX_LIGHTS;
}

bool LightController_Add(int id, LightDriver lightDriver)
{
    if (isIdInBounds(id))
        return false;

    if (lightDriver == NULL)
        return false;

    LightDriver_Destroy(lightDrivers[id]);

    lightDrivers[id] = lightDriver;
    return true;
}

bool LightController_Remove(int id)
{
    if (isIdInBounds(id))
        return false;

    if (lightDrivers[id] == NULL)
        return false;

    LightDriver_Destroy(lightDrivers[id]);

    lightDrivers[id] = NULL;
    return true;
}

bool LightController_TurnOn(int id)
{
    if(lightDrivers[id] == NULL) return false;

    LightDriver_TurnOn(lightDrivers[id]);
    return true;
}

bool LightController_TurnOff(int id)
{
    if(lightDrivers[id] == NULL) return false;

    LightDriver_TurnOff(lightDrivers[id]);
    return true;
}






