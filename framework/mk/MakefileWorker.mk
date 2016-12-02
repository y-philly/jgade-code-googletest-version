#-------------------------------------------------------------------------------
# MakefileWorker.mk
#
# Support Makefile for some unit test projects.
# Please include from your project Makefile.
#
#  Please set below variables in your project's Makefile.
#
# UNIT_NAME  : The name of a unit which you test.
# PRJ_ROOT   : Root directory of your project.
# SRC        : List of product codes.
# MOCK_CODE  : List of mock codes you use.
#-------------------------------------------------------------------------------

ifndef UNIT_NAME
  $(error Please set UNIT_NAME)
endif
ifdef PRJ_ROOT
  src_dir          := $(PRJ_ROOT)/src
  test_dir         := $(PRJ_ROOT)/test
  mock_dir         := $(PRJ_ROOT)/mock
  include_dir      := $(PRJ_ROOT)/include $(PRJ_ROOT)/test $(PRJ_ROOT)/mock
  ut_framework_dir := $(PRJ_ROOT)/framework
else
  $(error Please set PRJ_ROOT)
endif

include $(PRJ_ROOT)/framework/mk/Helper.mk

# Debug off by default
ifndef ENABLE_DEBUG
  ENABLE_DEBUG := n
endif

# Default compiler warnings
ifndef WARNINGFLAGS
  WARNINGFLAGS := -Wall \
                  -Wextra \
                  -Wshadow \
                  -Wswitch-default \
                  -Wconversion \
                  -Wno-missing-field-initializers \

# WARNINGFLAGS += -pedantic-errors \
#                 -Wpedantic \
#                 -Wswitch-enum \
#                 -Werror
endif
compile_flags += $(UT_WARNINGFLAGS)

LD_LIBRARLIES += -lpthread -lstdc++

ifeq ($(UT_ENABLE_DEBUG), y)
  compile_flags += -g -O0
else
  compile_flags += -O2
endif

ifeq ($(CC), clang)
  CXX := clang++
  CPPFLAGS += -U__STRICT_ANSI__
  compile_flags += -fno-exceptions
endif

ifeq ($(USE_CCACHE), y)
  CC := ccache $(CC)
  CXX := ccache $(CXX)
endif

ifeq "$(MAKECMDGOALS)" "coverage"
  ifeq ($(CC), clang)
    $(error, Sorry, you can\'t use coverage when compiler is clang.)
  endif
  CPPFLAGS += -D_COVERAGE_
  compile_flags += --coverage -O0
  LD_LIBRARLIES += -lgcov
endif

CFLAGS   := $(compile_flags) -std=c99
CXXFLAGS := $(compile_flags) -std=gnu++14
CPPFLAGS += -DPATH_MAX=256

# Define GTEST_HAS_PTHREAD, Because default value of this variable is 0 in Cygwin environment.
CPPFLAGS += $(include) -DGTEST_HAS_PTHREAD=1
ifeq ($(UT_ENABLE_DEBUG), y)
  CPPFLAGS += -D_DEBUG_
endif

test_target := $(UNIT_NAME)Test

gmock_dir := $(PRJ_ROOT)/framework/googletest/googlemock
gmock_src := googlemock/src/gmock-all.cc googlemock/src/gmock_main.cc
gmock_obj := $(call src-to-o,$(gmock_src))

gtest_dir := $(PRJ_ROOT)/framework/googletest/googletest
gtest_src := googletest/src/gtest-all.cc
gtest_obj := $(call src-to-o,$(gtest_src))

include += -I$(gmock_dir) -I$(gtest_dir) \
           -I$(gmock_dir)/include -I$(gtest_dir)/include \
           $(foreach dir, $(call get-dirs-from-dirspec, $(include_dir)), -I$(dir))

obj      := $(call src-to-o,$(SRC))
test_obj := $(call src-to-o,$(TEST_CODE))
mock_obj := $(call src-to-o,$(MOCK_CODE))

all_src := $(SRC) $(TEST_CODE) $(MOCK_CODE) $(gmock_src) $(gtest_src)
dep     := $(call src-to-d,$(all_src))

# Test coverage with gcov
gcov_gcda_files := $(call src-to-gcda, $(all_src))
gcov_gcno_files := $(call src-to-gcno, $(all_src))

stuff_to_clean := $(obj) $(test_obj) $(mock_obj) $(dep) \
                  $(gmock_obj) $(gtest_obj) \
                  $(gcov_gcda_files) $(gcov_gcno_files) \
                  coverage-report/* lcov.info \
                  $(test_target)

vpath %.c $(src_dir)
vpath %.cpp $(mock_dir)
vpath %.cc $(PRJ_ROOT)/framework/googletest

all:

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(dep)
endif

.PHONY: all
all: $(test_target)
	./$(test_target)

.PHONY: coverage
coverage: all
	$(ut_framework_dir)/script/coverage-report.sh $(ut_framework_dir)

.PHONY: build
build: $(test_target)

$(test_target): $(obj) $(test_obj) $(mock_obj) $(gmock_obj) $(gtest_obj)
	$(LINK.o) -o $@ $^ $(LD_LIBRARLIES)

obj/%.o: %.cpp
	@echo compiling $(notdir $<)
	mkdir -p $(dir $@)
	$(COMPILE.cpp) -MMD -MP $(OUTPUT_OPTION) $<

obj/%.o: %.cc
	@echo compiling $(notdir $<)
	mkdir -p $(dir $@)
	$(COMPILE.cc) -MMD -MP $(OUTPUT_OPTION) $<

obj/%.o: %.c
	@echo compiling $(notdir $<)
	mkdir -p $(dir $@)
	$(COMPILE.c) -MMD -MP  $(OUTPUT_OPTION) $<

.PHONY: clean
clean:
	echo Making clean
	rm -rf $(stuff_to_clean)

ctags_option = -R --c++-kinds=+pl --fields=+iaS --extra=+q

.PHONY: tags
tags:
	ctags $(ctags_option) $(src_dir)
	ctags $(ctags_option) $(PRJ_ROOT)/include
	ctags $(ctags_option) --append=yes .

comma:= ,
empty:=
space:= $(empty) $(empty)

.PHONY: cscope
cscope:
	cscope -R -q -b -s $(subst $(space),$(comma),$(include_dir)),$(src_dir)

.PHONY: include_paths.vim
include_paths.vim:
	@$(ut_framework_dir)/script/make-vim-marching_include_paths.sh $(include_dir)

debug:
	@echo
	@echo "Target Source code files:"
	@$(call debug-print-list,$(SRC))
	@echo "Target obj files:"
	@$(call debug-print-list,$(obj))
	@echo "Test code files:"
	@$(call debug-print-list,$(TEST_CODE))
	@echo "Test obj files:"
	@$(call debug-print-list,$(test_obj))
	@echo "Mock code files:"
	@$(call debug-print-list,$(MOCK_CODE))
	@echo "Mock obj files:"
	@$(call debug-print-list,$(mock_obj))
	@echo "All Input Dependency files:"
	@$(call debug-print-list,$(dep))
	@echo Stuff to clean:
	@$(call debug-print-list,$(stuff_to_clean))
	@echo Includes:
	@$(call debug-print-list,$(include))
