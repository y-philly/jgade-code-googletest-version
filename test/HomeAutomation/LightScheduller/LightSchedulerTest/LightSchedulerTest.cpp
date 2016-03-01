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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "util/MockTimeService.h"
#include "Device/MockLightDriver.h"
#include "HomeAutomation/MockRandomMinute.h"
#include "HomeAutomation/LightController.h"
#include "HomeAutomation/LightScheduler.h"


namespace HomeAutomation {
namespace LightSchedulerTest {


using ::testing::_;
using ::testing::Eq;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;


using LightDriverStructVector = std::vector<LightDriverStruct>;


enum class LightState {
    kUnknown = -1,
    kOff = 0,
    kOn = 1
};


class LightScheduler : public ::testing::Test {
public:
    LightScheduler() : lightDriverStructVector_(MAX_LIGHTS) {}

protected:
    virtual void SetUp()
    {
        util::gMockTimeServicePtr = &mockTimeService_;
        Device::gLightDriverPtr = &mockLightDriver_;
        HomeAutomation::gMockRandomMinutePtr = &mockRandomMinute_;

        LightController_Create();

        for (int i = 0; i < MAX_LIGHTS; i++) {
            EXPECT_CALL(mockLightDriver_, Destroy(_))
                .Times(1);
            LightController_Add(i, &lightDriverStructVector_[i]);
        }

        LightScheduler_Create();
    }

    virtual void TearDown()
    {
        LightScheduler_Destroy();

        EXPECT_CALL(mockLightDriver_, Destroy(_))
            .Times(MAX_LIGHTS);

        LightController_Destroy();
    }

    void CheckLightState(int id, LightState state)
    {
        if (id == kIdUnknown_) {
            EXPECT_CALL(mockLightDriver_, TurnOn(_)).Times(0);
            EXPECT_CALL(mockLightDriver_, TurnOff(_)).Times(0);
        } else {
            if (state == LightState::kOn) {
                EXPECT_CALL(mockLightDriver_, TurnOn(&lightDriverStructVector_[id])).Times(1);
            } else if (state == LightState::kOff) {
                EXPECT_CALL(mockLightDriver_, TurnOff(&lightDriverStructVector_[id])).Times(1);
            } else {
                // do nothing
            }
        }
    }

    void SetTimeTo(Day day, int minute)
    {
        Time time{ 0 };

        time.minuteOfDay = minute;
        time.dayOfWeek = day;

        EXPECT_CALL(mockTimeService_, GetTime(NotNull()))
            .Times(1)
            .WillOnce(SetArgPointee<0>(time));
    }

    static constexpr int kIdUnknown_ = -1;
    static constexpr int kScheduledMinute_ = 1234;
    static constexpr int kLightNumber_ = 4;

    LightDriverStructVector lightDriverStructVector_;

    util::MockTimeService mockTimeService_;
    Device::MockLightDriver mockLightDriver_;
    HomeAutomation::MockRandomMinute mockRandomMinute_;
};

#if 0
TEST_GROUP(LightScheduler)
{
    int scheduledMinute;
    int expectedId;
    int expectedLevel;
    int lightNumber;
    int seed;
    int pseudoRandomIncrement;


    void setup()
    {
        LightDriverSpy_Reset();
        LightController_Create();
        LightDriverSpy_AddSpiesToController();
        LightScheduler_Create();

        scheduledMinute = 1234;
        checkLightState(LIGHT_ID_UNKNOWN, LIGHT_STATE_UNKNOWN);
        lightNumber = 4;

        UT_PTR_SET(RandomMinute_Get, FakeRandomMinute_Get);
        FakeRandomMinute_Reset();
        seed = -10;
        pseudoRandomIncrement = 5;
    }

    void teardown()
    {
    }

    void checkLightState(int id, int level)
    {
        if (id == LIGHT_ID_UNKNOWN)
            LONGS_EQUAL(LIGHT_STATE_UNKNOWN, LightDriverSpy_GetLastState())
        else
            LONGS_EQUAL(level, LightDriverSpy_GetState(id))
    }

    void setTimeTo(int day, int minute)
    {
        FakeTimeService_SetDay(day);
        FakeTimeService_SetMinute(minute);
    }

};
#endif

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
