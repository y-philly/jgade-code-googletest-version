#------------------------------------------------------------------------------------
# MakefileWorker.mk
#
# ユニットテストプロジェクト用Makefileの支援ファイルです.
# プロジェクトのMakefileからインクルードして下さい.
#
# プロジェクトのMakefileでは以下の変数を設定してください.
#
# UNIT_NAME            : テスト実行ファイルの名前
# PRJ_ROOT             : ユニットテストフレームワークのルートディレクトリ
# TEST_CODE_DIR        : ユニットテストコードが格納されたディレクトリ
# MOCK_CODE_DIR        : モックコードが格納されたディレクトリ
# OVERRIDE_INCLUDE_DIR : 優先させたいインクルードファイルが格納されたディレクトリ
#------------------------------------------------------------------------------------

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

# Debug off by default
ifndef UT_ENABLE_DEBUG
  UT_ENABLE_DEBUG := n
endif

# Default warnings
ifndef UT_WARNINGFLAGS
  UT_WARNINGFLAGS := -Wall \
                     -Wextra \
                     -Wshadow \
                     -Wswitch-default \
                     -Wconversion \
                     -Wno-missing-field-initializers

# UT_WARNINGFLAGS += -pedantic-errors \
#                    -Wpedantic \
#                    -Wswitch-enum \
#                    -Werror
endif
compile_flags += $(UT_WARNINGFLAGS)

LD_LIBRARLIES +=  -lpthread -lstdc++

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

ifeq ($(UT_USE_CCACHE), y)
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

CFLAGS = $(compile_flags) -std=c99
CXXFLAGS = $(compile_flags) -std=gnu++1y

# Cygwin環境ではGTEST_HAS_PTHREADが自動で1にならないので、明示的に指定する.
CPPFLAGS += $(include) \
            -DGTEST_HAS_PTHREAD=1
ifeq ($(UT_ENABLE_DEBUG), y)
  CPPFLAGS += -D_DEBUG_
endif


# Helper functions
get_src_from_dir = $(wildcard $1/*.cpp) $(wildcard $1/*.cc) $(wildcard $1/*.c)
get_dirs_from_dirspec = $(wildcard $1)
get_src_from_dir_list = $(foreach dir, $1, $(call get_src_from_dir,$(dir)))
__src_to = $(subst .c,$1, $(subst .cpp,$1, $(subst .cc,$1,$2)))
src_to = $(addprefix obj/,$(call __src_to,$1,$2))
src_to_o = $(call src_to,.o,$1)
src_to_d = $(call src_to,.d,$1)
src_to_gcda = $(call src_to,.gcda,$1)
src_to_gcno = $(call src_to,.gcno,$1)
get_lib_from_dir = $(wildcard $1/*.a)
debug_print_list = $(foreach word,$1,echo "  $(word)";) echo;

test_target := $(UNIT_NAME)Test

gmock_dir := $(PRJ_ROOT)/framework/googletest/googlemock
gmock_src := googlemock/src/gmock-all.cc googlemock/src/gmock_main.cc
gmock_obj := $(call src_to_o,$(gmock_src))

gtest_dir := $(PRJ_ROOT)/framework/googletest/googletest
gtest_src := googletest/src/gtest-all.cc
gtest_obj := $(call src_to_o,$(gtest_src))

include += -I$(gmock_dir) -I$(gtest_dir) \
           -I$(gmock_dir)/include -I$(gtest_dir)/include \
           $(foreach dir, $(call get_dirs_from_dirspec, $(include_dir)), -I$(dir))

obj      = $(call src_to_o,$(SRC))
test_obj = $(call src_to_o,$(TEST_CODE))
mock_obj = $(call src_to_o,$(MOCK_CODE))

all_src = $(SRC) $(TEST_CODE) $(MOCK_CODE) $(gmock_src) $(gtest_src)
dependents = $(call src_to_d,$(all_src))

#
# Coverage settings
#
lcov    := $(ut_framework_dir)/tools/lcov-1.11/bin/lcov
genhtml := $(ut_framework_root)/tools/lcov-1.11/bin/genhtml
lcovrc  := $(ut_framework_dir)/tools/lcovrc
lcov_tracefile := lcov.info
lcov_remove_pattern := "/usr/lib/*" "/usr/include/*" "*gtest*" "*gmock*" "*mock*" "*Test*"

# Test coverage with gcov
gcov_gcda_files = $(call src_to_gcda, $(all_src))
gcov_gcno_files = $(call src_to_gcno, $(all_src))
stuff_to_clean += $(gcov_gcda_files) \
                  $(gcov_gcno_files)

#The gcda files for gcov need to be deleted before each run
#To avoid annoying messages.
covfile_clean = rm -f $(gcov_gcda_files) $(gcov_output) $(gcov_report) $(gcov_error) ; \
                rm -rf $(LCOV_TRACEFILE) coverage-report
run_test_target = $(covfile_clean) ; ./$(test_target) ;
gen_covfile = $(lcov) --capture --directory ./obj --output-file $(lcov_tracefile) --config-file $(LCOVRC); \
              $(lcov) --remove $(lcov_tracefile) $(lcov_remove_pattern) --output-file $(lcov_tracefile) --config-file $(lcovrc); \
              $(genhtml) $(lcov_tracefile) --output-directory coverage-report --config-file $(lcovrc)

stuff_to_clean = $(obj) $(test_obj) $(mock_obj) $(dependents) $(gmock_obj) $(gtest_obj)

vpath %.c $(src_dir)
vpath %.cpp $(mock_dir)
vpath %.cc $(PRJ_ROOT)/framework/googletest

.PHONY: all
all: $(test_target)
	$(run_test_target)

.PHONY: coverage
coverage: all
	$(gen_covfile)

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

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(dependents)
endif

.PHONY: clean
clean:
	echo Making clean
	rm -rf $(stuff_to_clean)
	rm -rf gcov obj
	find . -name "*.gcno" | xargs rm -f
	find . -name "*.gcda" | xargs rm -f
	find . -name "*.exe"  | xargs rm -rf

ctags_option = -R --c++-kinds=+pl --fields=+iaS --extra=+q

.PHONY: tags
tags:
	ctags $(ctags_option) $(PRODUCT_CODE_DIR)
	ctags $(ctags_option) --append=yes .

.PHONY: cscope
cscope:
	cscope -R -q -b -s $(include_dir) $(src_dir)

.PHONY: include_paths.vim
include_paths.vim:
	@echo "let include_paths = [" $(foreach path,$(include_dir),\"$(shell cygpath -m $(path))\",) > /tmp/include_paths.txt
	@sed -e "s/,$$/ ]/g" /tmp/include_paths.txt > include_paths.vim
	@echo "let g:marching_include_paths += include_paths" >> include_paths.vim
	@rm -f /tmp/include_paths.txt

debug:
	@echo
	@echo "Target Source code files:"
	@$(call debug_print_list,$(SRC))
	@echo "Target obj files:"
	@$(call debug_print_list,$(obj))
	@echo "Test code files:"
	@$(call debug_print_list,$(TEST_CODE))
	@echo "Test obj files:"
	@$(call debug_print_list,$(test_obj))
	@echo "Mock code files:"
	@$(call debug_print_list,$(MOCK_CODE))
	@echo "Mock obj files:"
	@$(call debug_print_list,$(mock_obj))
	@echo "All Input Dependency files:"
	@$(call debug_print_list,$(dep))
	@echo Stuff to clean:
	@$(call debug_print_list,$(stuff_to_clean))
	@echo Includes:
	@$(call debug_print_list,$(include))
