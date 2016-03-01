UNIT_NAME := LightScheduler
PRJ_ROOT  := ../../../..

#
# srcディレクトリからの相対パスでテストするソースコードを列挙する.
#
SRC := HomeAutomation/LightScheduler.c \
       HomeAutomation/LightController.c

#
# カレントディレクトリからの相対パスでテストコードを列挙する.
#
TEST_CODE := LightSchedulerTest.cpp

#
# mockディレクトリからの相対パスでモックコードを列挙する.
#
MOCK_CODE := util/MockTimeService.cpp \
             Device/MockLightDriver.cpp \
             HomeAutomation/MockRandomMinute.cpp

include $(PRJ_ROOT)/framework/MakefileWorker.mk
