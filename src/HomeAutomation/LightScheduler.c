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
/*-    Modifed by Yasuhiro SHIMIZU                                     -*/
/*- ------------------------------------------------------------------ -*/

#include <stdlib.h>
#include <string.h>
#include "HomeAutomation/LightScheduler.h"
#include "HomeAutomation/LightController.h"
#include "util/TimeService.h"
#include "HomeAutomation/RandomMinute.h"

enum
{
    TURN_ON, TURN_OFF, DIM, RANDOM_ON, RANDOM_OFF
};

enum
{
    MAX_EVENTS = 64, UNUSED = -1
};

typedef struct
{
    int id;
    Day day;
    long int minuteOfDay;
    int event;
    int randomize;
    int randomMinutes;

} ScheduledLightEvent;

static ScheduledLightEvent eventList[MAX_EVENTS];

void LightScheduler_Create(void)
{
    int i;
    ScheduledLightEvent * event = eventList;

    for (i = 0; i < MAX_EVENTS; i++, event++)
    {
        event->id = UNUSED;
    }
}

void LightScheduler_Destroy(void)
{
}

static void resetRandomize(ScheduledLightEvent * e);

static void setEventSchedule(ScheduledLightEvent * event,
        int id, Day day, long int minute, int control, int randomize)
{
        event->id = id;
        event->day = day;
        event->minuteOfDay = minute;
        event->event = control;
        event->randomize = randomize;
        resetRandomize(event);
}

static bool isInUse(ScheduledLightEvent * event)
{
    return event->id != UNUSED;
}

static ScheduledLightEvent * findUnusedEvent(void)
{
    int i;
    ScheduledLightEvent * event = eventList;

    for (i = 0; i < MAX_EVENTS; i++, event++)
    {
        if (!isInUse(event))
            return event;
    }
    return NULL;
}

static void scheduleEvent(int id, Day day, long int minute, int control,
        int randomize)
{

    ScheduledLightEvent * event = findUnusedEvent();

    if (event)
        setEventSchedule(event, id, day, minute, control, randomize);
}

void LightScheduler_ScheduleTurnOn(int id, Day day, long int minute)
{
    scheduleEvent(id, day, minute, TURN_ON, RANDOM_OFF);
}

void LightScheduler_ScheduleTurnOff(int id, int day, long int minute)
{
    scheduleEvent(id, day, minute, TURN_OFF, RANDOM_OFF);
}

static bool matchEvent(ScheduledLightEvent * e, int id, Day day, long int minute)
{
    return e->id == id && e->day == day && e->minuteOfDay == minute;
}

void LightScheduler_Randomize(int id, Day day, long int minute)
{
    int i;
    ScheduledLightEvent * event = eventList;

    for (i = 0; i < MAX_EVENTS; i++, event++)
    {
        if (matchEvent(event, id, day, minute))
        {
            event->randomize = RANDOM_ON;
            resetRandomize(event);
        }
    }
}

void LightScheduler_ScheduleRemove(int id, Day day, long int minute)
{
    int i;
    ScheduledLightEvent * event = eventList;

    for (i = 0; i < MAX_EVENTS; i++, event++)
    {
        if (matchEvent(event, id, day, minute))
            event->id = UNUSED;
    }
}

static void operateLight(ScheduledLightEvent * event)
{
    if (event->event == TURN_ON)
        LightController_TurnOn(event->id);
    else if (TURN_OFF == event->event)
        LightController_TurnOff(event->id);
}

static void resetRandomize(ScheduledLightEvent * event)
{
    if (event->randomize == RANDOM_ON)
        event->randomMinutes = RandomMinute_Get();
    else
        event->randomMinutes = 0;
}

static bool isEventDueNow(Time * time, ScheduledLightEvent * event)
{
    int todaysMinute = (int)(event->minuteOfDay + event->randomMinutes);
    Day day = event->day;

    if (!Time_MatchesMinuteOfDay(time, todaysMinute))
        return false;

    if (!Time_MatchesDayOfWeek(time, day))
        return false;

    return true;
}

static void processEventsDueNow(Time * time, ScheduledLightEvent * event)
{
    if (isInUse(event))
    {
        if (isEventDueNow(time, event))
        {
            operateLight(event);
            resetRandomize(event);
        }
    }
}

void LightScheduler_WakeUp(void)
{
    int i;
    Time time;

    TimeService_GetTime(&time);

    for (i = 0; i < MAX_EVENTS; i++)
    {
        processEventsDueNow(&time, &eventList[i]);
    }
}
