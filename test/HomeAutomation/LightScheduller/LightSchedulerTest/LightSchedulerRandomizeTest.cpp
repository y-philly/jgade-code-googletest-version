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

#include "HomeAutomation/MockRandomMinute.h"
#include "LightSchedulerTest.h"


namespace HomeAutomation {
namespace LightSchedulerTest {

class LightSchedulerRandomize : public LightScheduler {
protected:
    virtual void SetUp()
    {
        LightScheduler::SetUp();
        gMockRandomMinutePtr = &mockRandomMinute_;
    }

    static constexpr int kFirstRandomMinute_ = 5;

    MockRandomMinute mockRandomMinute_;
};


TEST_F(LightSchedulerRandomize, turns_on_early)
{
    LightScheduler_ScheduleTurnOn(4, EVERYDAY, 600);

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(-10));

    LightScheduler_Randomize(4, EVERYDAY, 600);

    SetTimeTo(MONDAY, (600 - 10));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (600 - 10)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState(4, LightState::kOn);

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(0));

    LightScheduler_WakeUp();
}


TEST_F(LightSchedulerRandomize, enable_randomize)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, EVERYDAY, kScheduledMinute_);

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(kFirstRandomMinute_));

    LightScheduler_Randomize(kLightNumber_, EVERYDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_ + kFirstRandomMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + kFirstRandomMinute_)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(0));

    CheckLightState(4, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightSchedulerRandomize, different_randmom_factor_the_second_day)
{
    LightScheduler_ScheduleTurnOn(kLightNumber_, EVERYDAY, kScheduledMinute_);

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(kFirstRandomMinute_));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + kFirstRandomMinute_)))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    LightScheduler_Randomize(kLightNumber_, EVERYDAY, kScheduledMinute_);

    SetTimeTo(MONDAY, kScheduledMinute_ + kFirstRandomMinute_);

    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(1)
            .WillOnce(Return(0));

    CheckLightState(4, LightState::kOn);

    LightScheduler_WakeUp();
}

TEST_F(LightSchedulerRandomize, multiple_random_controls)
{
    EXPECT_CALL(mockRandomMinute_, Get())
            .Times(4)
            .WillRepeatedly(Return(kFirstRandomMinute_));

    LightScheduler_ScheduleTurnOn(kLightNumber_, EVERYDAY, kScheduledMinute_);
    LightScheduler_Randomize(kLightNumber_, EVERYDAY, kScheduledMinute_);
    LightScheduler_ScheduleTurnOn((kLightNumber_ + 1), EVERYDAY, (kScheduledMinute_ + 3));
    LightScheduler_Randomize((kLightNumber_ + 1), EVERYDAY, (kScheduledMinute_ + 3));

    SetTimeTo(MONDAY, kScheduledMinute_ + kFirstRandomMinute_);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + kFirstRandomMinute_)))
            .Times(1)
            .WillOnce(Return(true));
    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(
                                    NotNull(),
                                    (kScheduledMinute_ + kFirstRandomMinute_ + 3)))
            .Times(1)
            .WillOnce(Return(false));

    CheckLightState(kLightNumber_, LightState::kOn);

    LightScheduler_WakeUp();


    SetTimeTo(MONDAY, kScheduledMinute_ + kFirstRandomMinute_ + 3);

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(
                                    NotNull(),
                                    (kScheduledMinute_ + kFirstRandomMinute_)))
            .Times(1)
            .WillOnce(Return(false));

    EXPECT_CALL(mockTimeService_, MatchesMinuteOfDay(NotNull(), (kScheduledMinute_ + kFirstRandomMinute_ + 3)))
            .Times(1)
            .WillOnce(Return(true));
    EXPECT_CALL(mockTimeService_, MatchesDayOfWeek(NotNull(), EVERYDAY))
            .Times(1)
            .WillOnce(Return(true));

    CheckLightState((kLightNumber_ + 1), LightState::kOn);

    LightScheduler_WakeUp();
}


#if 0 //To be implemented
TEST_F(LightSchedulerRandomize, EnableRandomizeEarlyOnlyOneEventFires)
{
    UT_PTR_SET(RandomMinute_Get, FakeRandomMinute_Get);
    FakeRandomMinute_SetFirstAndIncrement(-10, 1);

    LightScheduler_ScheduleTurnOn(4, EVERYDAY, 600);
    LightScheduler_Randomize(4, EVERYDAY, 600);

    FakeTimeService_SetMinute(590);
    LightScheduler_WakeUp();
    LightController_Create();
    FakeTimeService_SetMinute(591);
    LightScheduler_WakeUp();

    checkLightState(LIGHT_ID_UNKNOWN, LIGHT_STATE_UNKNOWN);
}
#endif


} // namespace LightSchedulerTest
} // namespace HomeAutomation
