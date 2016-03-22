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

#include "Device/MockLightDriver.h"
#include "HomeAutomation/LightController.h"


namespace HomeAutomation {


using ::testing::_;
using ::testing::Eq;
using ::testing::NotNull;
using ::testing::Sequence;


class LightControllerTest : public ::testing::TestWithParam<int> {
public:
    LightControllerTest() : lightDriverVector_(MAX_LIGHTS)
    {
    }

protected:
    virtual void SetUp()
    {
        Device::gLightDriverPtr = &mockLightDriver_;

        LightController_Create();
    }

    virtual void TearDown()
    {
        EXPECT_CALL(mockLightDriver_, Destroy(_))
            .Times(MAX_LIGHTS);

        LightController_Destroy();
    }

    bool id_is_valid(int id) const { return (0 <= id) && (id < MAX_LIGHTS); }

    std::vector<::LightDriverStruct> lightDriverVector_;

    Device::MockLightDriver mockLightDriver_;

    Sequence sequence_;
};


TEST(LifeCycleOfLightController, create_destroy)
{
    Device::MockLightDriver mockLightDriver;
    Device::gLightDriverPtr = &mockLightDriver;

    LightController_Create();

    EXPECT_CALL(mockLightDriver, Destroy(_))
        .Times(MAX_LIGHTS);

    LightController_Destroy();
}


TEST(LifeCycleOfLightController, driver_is_destroyed_by_light_controller)
{
    Device::MockLightDriver mockLightDriver;
    Device::gLightDriverPtr = &mockLightDriver;

    LightController_Create();

    EXPECT_CALL(mockLightDriver, Destroy(_))
        .Times(1);

    ::LightDriverStruct aLightDriver;
    LightController_Add(1, &aLightDriver);

    EXPECT_CALL(mockLightDriver, Destroy(_))
        .Times(MAX_LIGHTS - 1);

    EXPECT_CALL(mockLightDriver, Destroy(&aLightDriver))
        .Times(1);

    LightController_Destroy();
}


TEST_F(LightControllerTest, turn_on)
{
    EXPECT_CALL(mockLightDriver_, Destroy(_))
        .Times(1);

    LightController_Add(7, &lightDriverVector_[7]);

    EXPECT_CALL(mockLightDriver_, TurnOn(&lightDriverVector_[7]))
        .Times(1);

    LightController_TurnOn(7);
}


TEST_F(LightControllerTest, turn_off)
{
    EXPECT_CALL(mockLightDriver_, Destroy(_))
        .Times(1);

    LightController_Add(1, &lightDriverVector_[1]);

    EXPECT_CALL(mockLightDriver_, TurnOff(&lightDriverVector_[1]))
        .Times(1);

    LightController_TurnOff(1);
}

TEST_F(LightControllerTest, all_drivers_destroyed)
{
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        EXPECT_CALL(mockLightDriver_, Destroy(_))
            .Times(1);

        ASSERT_THAT(LightController_Add(i, &lightDriverVector_[i]), Eq(true));
    }
}

TEST_P(LightControllerTest, can_be_added_light_when_id_is_in_bounds)
{
    const int id = GetParam();
    const bool expectedResult = id_is_valid(id);

    if (expectedResult) {
        EXPECT_CALL(mockLightDriver_, Destroy(_))
            .Times(1);
    }
    ASSERT_THAT(LightController_Add(id, &lightDriverVector_[id]), Eq(expectedResult));
}

INSTANTIATE_TEST_CASE_P(
    InRange,
    LightControllerTest,
    ::testing::Values(0, (MAX_LIGHTS - 1), MAX_LIGHTS));

INSTANTIATE_TEST_CASE_P(
    OutOfRange,
    LightControllerTest,
    ::testing::Values(MAX_LIGHTS));


TEST_F(LightControllerTest, remove_existing_light_driver_succeeds)
{
    EXPECT_CALL(mockLightDriver_, Destroy(_))
        .Times(1);

    LightController_Add(10, &lightDriverVector_[10]);

    EXPECT_CALL(mockLightDriver_, Destroy(&lightDriverVector_[10]))
        .Times(1);

    LightController_Remove(10);
}

TEST_F(LightControllerTest, removed_light_does_nothing)
{
    EXPECT_CALL(mockLightDriver_, Destroy(_))
        .Times(1);

    LightController_Add(1, &lightDriverVector_[1]);

    EXPECT_CALL(mockLightDriver_, Destroy(&lightDriverVector_[1]))
        .Times(1);

    LightController_Remove(1);

    ASSERT_THAT(LightController_TurnOn(1), Eq(false));
    ASSERT_THAT(LightController_TurnOff(1), Eq(false));
}

TEST_F(LightControllerTest, rejects_null_drivers)
{
    ASSERT_THAT(LightController_Add(1, NULL), Eq(false));
}

TEST_F(LightControllerTest, RemoveNonExistingLightDriverFails)
{
    ASSERT_THAT(LightController_Remove(10), Eq(false));
}

// TEST(LightController, turnOnDifferentDriverTypes)
// {
//     LightDriver otherDriver = CountingLightDriver_Create(5);
//     LightController_Add(5, otherDriver);
//     LightController_TurnOn(7);
//     LightController_TurnOn(5);
//     LightController_TurnOff(5);
//
//     LONGS_EQUAL(LIGHT_ON, LightDriverSpy_GetState(7));
//     LONGS_EQUAL(2, CountingLightDriver_GetCallCount(otherDriver));
// }


} // namespace HomeAutomation
