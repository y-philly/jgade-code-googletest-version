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
//-    Yasuhiro SHIMIZU ported this code to GoogleTest.
//- ------------------------------------------------------------------

#include "util/MockRuntimeError.h"

#include "LedDriver/LedDriver.h"


using ::testing::_;
using ::testing::Eq;
using ::testing::NotNull;
using ::testing::ByRef;
using ::testing::Sequence;


namespace LedDriverTest {

class LedDriver : public ::testing::TestWithParam<int> {
protected:
    virtual void SetUp()
    {
        util::gMockRuntimerErrorPtr = &mockRuntimeError_;

        LedDriver_Create(&virtualLeds_);
    }

    virtual void TearDown()
    {
        LedDriver_Destroy();
    }

    uint16_t virtualLeds_;

    util::MockRunuimeError mockRuntimeError_;

    Sequence sequence_;
};


TEST_F(LedDriver, leds_are_off_after_create)
{
    LedDriver_Destroy();
    LedDriver_Create(&virtualLeds_);
    ASSERT_THAT(virtualLeds_, Eq(0u));
}

TEST_F(LedDriver, turn_on_led_one)
{
    LedDriver_TurnOn(1);
    ASSERT_THAT(virtualLeds_, Eq(1u));
}

TEST_F(LedDriver, turn_off_led_one)
{
    LedDriver_TurnOn(1);
    LedDriver_TurnOff(1);
    ASSERT_THAT(virtualLeds_, Eq(0u));
}

TEST_F(LedDriver, turn_on_multiple_leds)
{
    LedDriver_TurnOn(9);
    LedDriver_TurnOn(8);
    ASSERT_THAT(virtualLeds_, Eq(0x180u));
}

TEST_F(LedDriver, turn_off_any_led)
{
    LedDriver_TurnAllOn();
    LedDriver_TurnOff(8);
    ASSERT_THAT(virtualLeds_, Eq(0xff7fu));
}

TEST_F(LedDriver, led_memory_is_not_readable)
{
    virtualLeds_ = 0xffffu;
    LedDriver_TurnOn(8);
    ASSERT_THAT(virtualLeds_, Eq(0x80u));
}

TEST_F(LedDriver, UpperAndLowerBounds)
{
    LedDriver_TurnOn(1);
    LedDriver_TurnOn(16);
    ASSERT_THAT(virtualLeds_, Eq(0x8001u));
}

TEST_P(LedDriver, out_of_bounds_turn_on_does_no_harm)
{
    const int n = GetParam();

    EXPECT_CALL(mockRuntimeError_, AddLog(_, n, _, _))
        .Times(1)
        .InSequence(sequence_);

    LedDriver_TurnOn(n);

    ASSERT_THAT(virtualLeds_, Eq(0u));
}

TEST_P(LedDriver, out_of_bounds_turn_off_does_no_harm)
{
    LedDriver_TurnAllOn();

    const int n = GetParam();

    EXPECT_CALL(mockRuntimeError_, AddLog(_, n, _, _))
        .Times(1)
        .InSequence(sequence_);

    LedDriver_TurnOff(n);

    ASSERT_THAT(virtualLeds_, Eq(0xffffu));
}

INSTANTIATE_TEST_CASE_P(
    OutOfBoundaries,
    LedDriver,
    ::testing::Values(-1, 0u, 17u, 3141u));

#if 0
TEST_F(LedDriver, out_of_bounds_to_do)
{
    //TODO: what should we do during runtime?
}
#endif

TEST_F(LedDriver, out_of_bounds_produces_runtime_error)
{
    const std::string kExpectLog{ "LED Driver: out-of-bounds LED" };

    EXPECT_CALL(mockRuntimeError_, AddLog(kExpectLog, _, _, _))
        .Times(1)
        .InSequence(sequence_);

    LedDriver_TurnOn(-1);
}

TEST_F(LedDriver, IsOn)
{
    ASSERT_THAT(LedDriver_IsOn(1), Eq(false));
    LedDriver_TurnOn(1);
    ASSERT_THAT(LedDriver_IsOn(1), Eq(true));
}

TEST_F(LedDriver, IsOff)
{
    ASSERT_THAT(LedDriver_IsOff(12), Eq(true));
    LedDriver_TurnOn(12);
    ASSERT_THAT(LedDriver_IsOff(12), Eq(false));
}

TEST_F(LedDriver, out_of_bounds_leds_are_always_off)
{
    EXPECT_CALL(mockRuntimeError_, AddLog(_, _, _, _))
        .Times(4);

    ASSERT_THAT(LedDriver_IsOff(0),  Eq(true));
    ASSERT_THAT(LedDriver_IsOff(17), Eq(true));
    ASSERT_THAT(LedDriver_IsOn(0),   Eq(false));
    ASSERT_THAT(LedDriver_IsOn(17),  Eq(false));
}

TEST_F(LedDriver, all_on)
{
    LedDriver_TurnAllOn();
    ASSERT_THAT(virtualLeds_, Eq(0xffffu));
}

TEST_F(LedDriver, all_off)
{
    LedDriver_TurnAllOn();
    LedDriver_TurnAllOff();
    ASSERT_THAT(virtualLeds_, Eq(0u));
}

} // namespace LedDriver

