//- ------------------------------------------------------------------
//-    Copyright (c) James W. Grenning -- All Rights Reserved
//-    For use by owners of Test-Driven Development for Embedded C,
//-    and attendees of Renaissance Software Consulting, Co. training
//-    classes.
//-
//-    Available at http://pragprog.com/titles/jgade/
//-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3
//-
//-    Authorized users may use this source code in your own
//-    projects, however the source code may not be used to
//-    create training material, courses, books, articles, and
//-    the like. We make no guarantees that this source code is
//-    fit for any purpose.
//-
//-    www.renaissancesoftware.net james@renaissancesoftware.net
//- ------------------------------------------------------------------
//- ------------------------------------------------------------------
//-    Ported to GoogleTest by Yasuhiro SHIMIZU .
//- ------------------------------------------------------------------

#include "LightSchedulerTest.h"


namespace HomeAutomation {
namespace LightSchedulerTest {


TEST_F(LightScheduler, create_does_not_change_the_lights)
{
    // Do nothing google test version.
}

TEST_F(LightScheduler, schedule_everyday_not_time_yet)
{
    LightScheduler_ScheduleTurnOn(3, EVERYDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, (kScheduledMinute_ - 1));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(false));

    CheckLightState(kIdUnknown_, LightState::kUnknown);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_today_its_time)
{
    LightScheduler_ScheduleTurnOn(3, EVERYDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(3, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_tuesday_and_its_not_tuesday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(3, TUESDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), TUESDAY))
            .Times(1)
            .WillOnce(Return(false));

    CheckLightState(kIdUnknown_, LightState::kUnknown);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_tuesday_and_its_tuesday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(3, TUESDAY, kScheduledMinute_);

    SetTimeTo(TUESDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), TUESDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(3, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_off_tuesday_and_its_tuesday_and_its_time)
{
    LightScheduler_ScheduleTurnOff(kLightNumber_, TUESDAY, kScheduledMinute_);

    SetTimeTo(TUESDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), TUESDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOff);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_off_weekend_and_its_saturday_and_its_time)
{
    LightScheduler_ScheduleTurnOff(kLightNumber_, WEEKEND, kScheduledMinute_);

    SetTimeTo(SATURDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKEND))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOff);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekend_and_its_sunday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, WEEKEND, kScheduledMinute_);

    SetTimeTo(SUNDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKEND))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekend_and_its_monday_and_its_time)
{
    LightScheduler_ScheduleTurnOff(kLightNumber_, WEEKEND, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKEND))
            .Times(1)
            .WillOnce(Return(false));

    CheckLightState(kIdUnknown_, LightState::kUnknown);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekday_and_its_sunday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, WEEKEND, kScheduledMinute_);

    SetTimeTo(SUNDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKEND))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekday_and_its_monday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, WEEKDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekday_and_its_friday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, WEEKDAY, kScheduledMinute_);

    SetTimeTo(FRIDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(kLightNumber_, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, schedule_on_weekday_and_its_saturday_and_its_time)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, WEEKDAY, kScheduledMinute_);

    SetTimeTo(SATURDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), WEEKDAY))
            .Times(1)
            .WillOnce(Return(false));

    CheckLightState(kIdUnknown_, LightState::kUnknown);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, RemoveScheduledEvent)
{
    LightScheduler_ScheduleTurnOn(6, MONDAY, kScheduledMinute_);
    LightScheduler_ScheduleRemove(6, MONDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    CheckLightState(kIdUnknown_, LightState::kUnknown);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, multiple_scheduled_events_same_time)
{
    LightScheduler_ScheduleTurnOff(4, MONDAY, kScheduledMinute_);
    LightScheduler_ScheduleTurnOn(3, MONDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(2)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), MONDAY))
            .Times(2)
            .WillRepeatedly(Return(true));

    CheckLightState(4, LightState::kOff);
    CheckLightState(3, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightScheduler, multiple_scheduled_events_different_times)
{
    LightScheduler_ScheduleTurnOff(4, MONDAY, kScheduledMinute_);
    LightScheduler_ScheduleTurnOn(3, MONDAY, (kScheduledMinute_ + 1));

    SetTimeTo(MONDAY, kScheduledMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + 1)))
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), MONDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(4, LightState::kOff);

    LightScheduler_WakeUp();

    SetTimeTo(MONDAY, (kScheduledMinute_ + 1));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), kScheduledMinute_))
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + 1)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), MONDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(3, LightState::kOn);

    LightScheduler_WakeUp();
}


} // namespace LightSchedulerTest
} // namespace HomeAutomation
