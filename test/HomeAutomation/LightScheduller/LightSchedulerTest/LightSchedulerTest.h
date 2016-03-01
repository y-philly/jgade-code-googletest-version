//- ------------------------------------------------------------------
//-    Copyright (c) Yasuhiro SHIMIZU -- All Rights Reserved
//- ------------------------------------------------------------------

#ifndef _HOME_AUTOMATION_LIGHT_SCHEDULER_H_
#define _HOME_AUTOMATION_LIGHT_SCHEDULER_H_

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "util/MockTimeService.h"
#include "Device/MockLightDriver.h"
#include "HomeAutomation/LightController.h"
#include "HomeAutomation/LightScheduler.h"
#include "HomeAutomation/MockRandomMinute.h"


namespace HomeAutomation {
namespace LightSchedulerTest {


using ::testing::_;
using ::testing::NotNull;
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



} // namespace LightSchedulerTest
} // namespace HomeAutomation

#endif  // _HOME_AUTOMATION_LIGHT_SCHEDULER_H_
