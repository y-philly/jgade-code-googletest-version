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

#include "gmock/gmock.h"

#include "util/CircularBuffer.h"
//#include "FormatOutputSpy.h"


namespace util {


using ::testing::Eq;

//
// Creation test.
//
TEST(create, empty_after_creation)
{
    CircularBuffer buffer = CircularBuffer_Create(10);;
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer), Eq(true));
}

TEST(create, not_full)
{
    CircularBuffer buffer = CircularBuffer_Create(10);;
    ASSERT_THAT(CircularBuffer_IsFull(buffer), Eq(false));
}

TEST(create, capacity)
{
    CircularBuffer b = CircularBuffer_Create(2);
    ASSERT_THAT(CircularBuffer_Capacity(b), Eq(2));
    CircularBuffer_Destroy(b);
}


//
// Base test class.
//
class CircularBufferTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
      buffer_ = CircularBuffer_Create(10);
    }

    virtual void TearDown()
    {
       ASSERT_THAT(CircularBuffer_VerifyIntegrity(buffer_), Eq(true));
       CircularBuffer_Destroy(buffer_);
    }

    void PutManyInTheBuffer(int seed, int howMany)
    {
        for (int i = 0; i < howMany; i++)
            CircularBuffer_Put(buffer_, i + seed);
    }

    CircularBuffer buffer_;
};


class buffer_is_empty : public CircularBufferTest {};

TEST_F(buffer_is_empty, not_empty)
{
    CircularBuffer_Put(buffer_, 10046);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(false));
}

TEST_F(buffer_is_empty, not_empty_then_empty)
{
    CircularBuffer_Put(buffer_, 4567);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(false));
    CircularBuffer_Get(buffer_);
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(true));
}

TEST_F(buffer_is_empty, get_put_one_value)
{
    CircularBuffer_Put(buffer_, 4567);
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(4567));
}

TEST_F(buffer_is_empty, get_put_a_few)
{
    CircularBuffer_Put(buffer_, 1);
    CircularBuffer_Put(buffer_, 2);
    CircularBuffer_Put(buffer_, 3);
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(1));
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(2));
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(3));
}

TEST_F(buffer_is_empty, is_full)
{
    for (int i = 0; i < CircularBuffer_Capacity(buffer_); i++)
        CircularBuffer_Put(buffer_, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer_), Eq(true));
}

TEST_F(buffer_is_empty, empty_to_full_to_empty)
{
    for (int i = 0; i < CircularBuffer_Capacity(buffer_); i++)
        CircularBuffer_Put(buffer_, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer_), Eq(true));

    for (int j = 0; j < CircularBuffer_Capacity(buffer_); j++)
        ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(j + 100));

    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(true));
    ASSERT_THAT(CircularBuffer_IsFull(buffer_),  Eq(false));
}

TEST_F(buffer_is_empty, wrap_around)
{
    int capacity = CircularBuffer_Capacity(buffer_);
    for (int i = 0; i < capacity; i++)
        CircularBuffer_Put(buffer_, i + 100);

    ASSERT_THAT(CircularBuffer_IsFull(buffer_),  Eq(true));
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(100));
    ASSERT_THAT(CircularBuffer_IsFull(buffer_),  Eq(false));
    CircularBuffer_Put(buffer_, 1000);
    ASSERT_THAT(CircularBuffer_IsFull(buffer_),  Eq(true));

    for (int j = 1; j < capacity; j++)
        ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(j + 100));

    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(1000));
    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(true));
}

TEST_F(buffer_is_empty, put_to_full_throws)
{
    PutManyInTheBuffer(900, CircularBuffer_Capacity(buffer_));
    ASSERT_THAT(CircularBuffer_Put(buffer_, 9999), Eq(false));
}

TEST_F(buffer_is_empty, put_to_full_does_not_damage_contents)
{
    PutManyInTheBuffer(900, CircularBuffer_Capacity(buffer_));
    CircularBuffer_Put(buffer_, 9999);

    for (int i = 0; i < CircularBuffer_Capacity(buffer_); i++)
        ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(i + 900));

    ASSERT_THAT(CircularBuffer_IsEmpty(buffer_), Eq(true));
}

TEST_F(buffer_is_empty, get_from_empty_returns0)
{
    ASSERT_THAT(CircularBuffer_Get(buffer_), Eq(0));
}

#if 0

//START: PrintSetupAndTearDownDuplication
TEST_F(CircularBuffer, PrintEmpty)
{
    const char* expectedOutput = "Circular buffer content:\n<>\n";
    FormatOutputSpy_Create(100);
    UT_PTR_SET(FormatOutput, FormatOutputSpy);

    CircularBuffer_Print(buffer_);

    STRCMP_EQUAL(expectedOutput, FormatOutputSpy_GetOutput());
    FormatOutputSpy_Destroy();
}

TEST_F(CircularBuffer, PrintAfterOneIsPut)
{
    const char* expectedOutput = "Circular buffer content:\n<17>\n";
    FormatOutputSpy_Create(100);
    UT_PTR_SET(FormatOutput, FormatOutputSpy);

    CircularBuffer_Put(buffer_, 17);
    Circularbuffer_Print(buffer_);

    STRCMP_EQUAL(expectedOutput, FormatOutputSpy_GetOutput());
    FormatOutputSpy_Destroy();
}

#endif

} // namespace util
