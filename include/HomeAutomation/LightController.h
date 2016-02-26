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

#ifndef D_LightController_H
#define D_LightController_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "Device/LightDriver.h"

enum {MAX_LIGHTS = 32};

void LightController_Create(void);
void LightController_Destroy(void);
bool LightController_TurnOn(int id);
bool LightController_TurnOff(int id);
bool LightController_Add(int id, LightDriver);
bool LightController_Remove(int id);

#ifdef __cplusplus
}
#endif

#endif  /* D_LightController_H */
