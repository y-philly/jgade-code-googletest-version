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

#include "gmock/gmock.h"

#include "util/CircularBuffer.h"
//#include "FormatOutputSpy.h"

using ::testing::Eq;

namespace CircularBufferTest {

class CircularBuffer : public ::testing::Test {
protected:
    virtual void SetUp()
    {
      buffer = CircularBuffer_Create(10);
    }

    virtual void TearDown()
    {
       ASSERT_THAT(CircularBuffer_VerifyIntegrity(buffer), Eq(true));
       CircularBuffer_Destroy(buffer);
    }

    void PutManyInTheBuffer(int seed, int howMany)
    {
        for (int i = 0; i < howMany; i++)
            CircularBuffer_Put(buffer, i + seed);
    }

    ::CircularBuffer buffer;
};

TEST_F(CircularBuffer, create) {}

TEST_F(CircularBuffer, empty_after_creation)
{
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
}

TEST_F(CircularBuffer, not_full_after_creation)
{
    ASSERT_THAT(CircularBuffer_IsFull(buffer), Eq(false));
}

TEST_F(CircularBuffer, not_empty)
{
    CircularBuffer_Put(buffer, 10046);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(false));
}

TEST_F(CircularBuffer, not_empty_then_empty)
{
    CircularBuffer_Put(buffer, 4567);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(false));
    CircularBuffer_Get(buffer);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
}

TEST_F(CircularBuffer, get_put_one_value)
{
    CircularBuffer_Put(buffer, 4567);
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(4567));
}

TEST_F(CircularBuffer, get_put_a_few)
{
    CircularBuffer_Put(buffer, 1);
    CircularBuffer_Put(buffer, 2);
    CircularBuffer_Put(buffer, 3);
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(1));
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(2));
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(3));
}

TEST_F(CircularBuffer, capacity)
{
    ::CircularBuffer b = CircularBuffer_Create(2);
    ASSERT_THAT(CircularBuffer_Capacity(b), Eq(2));
    CircularBuffer_Destroy(b);
}


TEST_F(CircularBuffer, is_full)
{
    for (int i = 0; i < CircularBuffer_Capacity(buffer); i++)
        CircularBuffer_Put(buffer, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer), Eq(true));
}

TEST_F(CircularBuffer, empty_to_full_to_empty)
{
    for (int i = 0; i < CircularBuffer_Capacity(buffer); i++)
        CircularBuffer_Put(buffer, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer), Eq(true));

    for (int j = 0; j < CircularBuffer_Capacity(buffer); j++)
        ASSERT_THAT(CircularBuffer_Get(buffer), Eq(j + 100));

    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
    ASSERT_THAT(CircularBuffer_IsFull(buffer),  Eq(false));
}

TEST_F(CircularBuffer, wrap_around)
{
    int capacity = CircularBuffer_Capacity(buffer);
    for (int i = 0; i < capacity; i++)
        CircularBuffer_Put(buffer, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer),  Eq(true));
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(100));
    ASSERT_THAT(CircularBuffer_IsFull(buffer),  Eq(false));
    CircularBuffer_Put(buffer, 1000);
    ASSERT_THAT(CircularBuffer_IsFull(buffer),  Eq(true));

    for (int j = 1; j < capacity; j++)
        ASSERT_THAT(CircularBuffer_Get(buffer), Eq(j + 100));

    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(1000));
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
}

TEST_F(CircularBuffer, put_to_full_throws)
{
    PutManyInTheBuffer(900, CircularBuffer_Capacity(buffer));
    ASSERT_THAT(CircularBuffer_Put(buffer, 9999), Eq(false));
}

TEST_F(CircularBuffer, put_to_full_does_not_damage_contents)
{
    PutManyInTheBuffer(900, CircularBuffer_Capacity(buffer));
    CircularBuffer_Put(buffer, 9999);

    for (int i = 0; i < CircularBuffer_Capacity(buffer); i++)
        ASSERT_THAT(CircularBuffer_Get(buffer), Eq(i + 900));

    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
}

TEST_F(CircularBuffer, get_from_empty_returns0)
{
    ASSERT_THAT(CircularBuffer_Get(buffer), Eq(0));
}

#if 0

//START: PrintSetupAndTearDownDuplication
TEST_F(CircularBuffer, PrintEmpty)
{
    const char* expectedOutput = "Circular buffer content:\n<>\n";
    FormatOutputSpy_Create(100);
    UT_PTR_SET(FormatOutput, FormatOutputSpy);

    CircularBuffer_Print(buffer);

    STRCMP_EQUAL(expectedOutput, FormatOutputSpy_GetOutput());
    FormatOutputSpy_Destroy();
}

TEST_F(CircularBuffer, PrintAfterOneIsPut)
{
    const char* expectedOutput = "Circular buffer content:\n<17>\n";
    FormatOutputSpy_Create(100);
    UT_PTR_SET(FormatOutput, FormatOutputSpy);

    CircularBuffer_Put(buffer, 17);
    CircularBuffer_Print(buffer);

    STRCMP_EQUAL(expectedOutput, FormatOutputSpy_GetOutput());
    FormatOutputSpy_Destroy();
}

#endif

} // namespace CircularBufferTest
