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

#include "IO/MockIO.h"
#include "IO/MockMicroTime.h"

#include "IO/m28w160ect.h"
#include "IO/Flash.h"


using ::testing::Eq;
using ::testing::Return;
using ::testing::Sequence;


namespace FlashTest {


class Flash : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        IO::gMockIOPtr = &mockIO_;
        IO::gMockMicroTimePtr = &mockMicroTime_;

        Flash_Create();
    }

    virtual void TearDown()
    {
        Flash_Destroy();
    }

    void ExpectCommand(IoData command)
    {
        EXPECT_CALL(mockIO_, Write(CommandRegister, command))
            .Times(1)
            .InSequence(sequence_);
    }

    void ExpectWriteData(IoAddress address, IoData data)
    {
        EXPECT_CALL(mockIO_, Write(address, data))
            .Times(1)
            .InSequence(sequence_);
    }

    void SimulateDeviceStatus(IoData status)
    {
        EXPECT_CALL(mockIO_, Read(StatusRegister))
            .Times(1)
            .InSequence(sequence_)
            .WillOnce(Return(status));
    }

    void SimulateDeviceStatusWithRepeat(
            IoData status,
            int repeatCount,
            uint32_t timeStampOffset,
            uint32_t increment = 1U)
    {
        for (int i = 0; i < repeatCount; i++) {
            EXPECT_CALL(mockIO_, Read(StatusRegister))
                .Times(1)
                .InSequence(sequence_)
                .WillOnce(Return(status));

            EXPECT_CALL(mockMicroTime_, Get())
                .Times(1)
                .InSequence(sequence_)
                .WillOnce(Return(timeStampOffset + (i * increment)));
        }
    }

    void SimulateReadback(IoAddress address, IoData data)
    {
        EXPECT_CALL(mockIO_, Read(address))
            .Times(1)
            .InSequence(sequence_)
            .WillOnce(Return(data));
    }

    static constexpr IoAddress kAddress_ = 0xfeed;
    static constexpr IoData kData_ = 0x1dea;

    IO::MockIO mockIO_;
    IO::MockMicroTime mockMicroTime_;

    Sequence sequence_;
};

TEST_F(Flash, write_success_immediately)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit);
    SimulateReadback(kAddress_, kData_);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_SUCCESS));
}

TEST_F(Flash, program_succeeds_not_immediately)
{
   EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatusWithRepeat(0, 3, 2U);
    SimulateDeviceStatus(ReadyBit);
    SimulateReadback(kAddress_, kData_);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_SUCCESS));
}

TEST_F(Flash, write_fails_vpp_error)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit | VppErrorBit);
    ExpectCommand(Reset);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_VPP_ERROR));
}

TEST_F(Flash, write_fails_program_error)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit | ProgramErrorBit);
    ExpectCommand(Reset);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_PROGRAM_ERROR));
}

TEST_F(Flash, write_fails_protected_block_error)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit | BlockProtectionErrorBit);
    ExpectCommand(Reset);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_PROTECTED_BLOCK_ERROR));
}

TEST_F(Flash, write_fails_flash_unknown_program_error)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit |  EraseSuspendBit | EraseErrorBit | ProgramSuspendBit | ReservedBit);
    ExpectCommand(Reset);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_UNKNOWN_PROGRAM_ERROR));
}

TEST_F(Flash, write_fails_flash_read_back_error)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatus(ReadyBit);
    SimulateReadback(kAddress_, kData_ - 1U);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_READ_BACK_ERROR));
}

TEST_F(Flash, write_succeeds_ignores_other_bits_until_ready)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);

    SimulateDeviceStatus(~ReadyBit);

    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(2U));

    SimulateDeviceStatus(ReadyBit);

    SimulateReadback(kAddress_, kData_);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_SUCCESS));
}

TEST_F(Flash, write_fails_timeout)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(1U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);
    SimulateDeviceStatusWithRepeat(~ReadyBit, 10, 1000U, 500);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_TIMEOUT_ERROR));
}

TEST_F(Flash, write_fails_timeout_at_end_of_time)
{
    EXPECT_CALL(mockMicroTime_, Get())
        .Times(1)
        .InSequence(sequence_)
        .WillOnce(Return(0xffffffff + 500U));

    ExpectCommand(ProgramCommand);
    ExpectWriteData(kAddress_, kData_);

    SimulateDeviceStatusWithRepeat(~ReadyBit, 10, (0xffffffff + 1000U), 500);

    int result = Flash_Write(kAddress_, kData_);

    ASSERT_THAT(result, Eq(FLASH_TIMEOUT_ERROR));
}

} // namespace FlashTest
