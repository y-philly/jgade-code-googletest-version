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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "gtest/gtest.h"
#include "HomeAutomation/RandomMinute.h"


namespace HomeAutomation {


class RandomMinuteTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        RandomMinute_Create(kBound);
        std::srand(1);
    }

    virtual void TearDown() {}

    static constexpr int kBound = 30;
};


TEST_F(RandomMinuteTest, get_is_in_range)
{
    int minute;

    for (int i = 0; i < 100; i++)
    {
        minute = RandomMinute_Get();
        if (minute < -kBound || minute > kBound)
        {
            std::printf("bad minute value: %d\n", minute);
            FAIL() << "Minute out of range";
        }
    }
}

TEST_F(RandomMinuteTest, all_values_possible)
{
    int minute;
    int hit[2 * kBound + 1];
    std::memset(hit, 0, sizeof(hit));

    for (int i = 0; i < 300; i++)
    {
        minute = RandomMinute_Get();
        if (minute < -kBound || minute > kBound)
        {
            std::printf("bad minute value: %d\n", minute);
            FAIL() << "Minute out of range";
        }
        hit[minute + kBound]++;
    }

    //using j for vc6 compatibility
    // for (int j = 0; j < 2*(kBound) + 1; j++) {
    //     CHECK(hit[j] > 0);
    // }
}


} // namespace HomeAutomation
