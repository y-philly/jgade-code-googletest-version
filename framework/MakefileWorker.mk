#-------------------------------------------------------------------------------
# MakefileWorker.mk
#
# ユニットテストプロジェクト用Makefileの支援ファイルです.
# プロジェクトのMakefileからインクルードして下さい.
#
# プロジェクトのMakefileでは以下の変数を設定してください.
#
# UNIT_NAME       : テスト実行ファイルの名前
# PRODUCT_CODE_DIR     : プロダクトコードのルートディレクトリ
# UTEST_FRAMEWORK_HOME : ユニットテストフレームワークのルートディレクトリ
# SOURCES              : テスト対象のプロダクトコード
# TEST_CODE_DIRS       : ユニットテストコードが格納されたディレクトリ
# MOCK_CODE_DIRS       : モックコードが格納されたディレクトリ
# INCLUDE_DIRS         : インクルードファイルが格納されたディレクトリ
#
#-------------------------------------------------------------------------------
ifndef UTEST_FRAMEWORK_HOME
  $(error Please set UTEST_FRAMEWORK_HOME)
endif
ifndef PRODUCT_CODE_DIR
  $(error Please set PRODUCT_CODE_DIR)
endif

LCOV := $(UTEST_FRAMEWORK_HOME)/tools/lcov-1.11/bin/lcov
LCOVRC := $(UTEST_FRAMEWORK_HOME)/tools/lcovrc
LCOV_REMOVE_PATTERN := "/usr/lib/*" "/usr/include/*" "*gtest*" "*gmock*" "*mock*" "*Test*"
LCOV_TRACEFILE := lcov.info

GENHTML := $(UTEST_FRAMEWORK_HOME)/tools/lcov-1.11/bin/genhtml

ifndef UNIT_NAME
  $(error Please set UNIT_NAME)
endif

# Debug off by default
ifndef UTEST_ENABLE_DEBUG
  UTEST_ENABLE_DEBUG := n
endif

# Default warnings
ifndef UTEST_WARNINGFLAGS
  UTEST_WARNINGFLAGS := -Wall \
                        -Wextra \
                        -Wshadow \
                        -Wswitch-default \
                        -Wconversion \
                        -Wno-missing-field-initializers

# UTEST_WARNINGFLAGS += -pedantic-errors \
#                       -Wpedantic \
#                       -Wswitch-enum \
#                       -Werror
endif
compile_flags += $(UTEST_WARNINGFLAGS)

# Default dir for temporary files (d, o)
ifndef UTEST_OBJECTS_DIR
  UTEST_OBJECTS_DIR := objects
endif


LD_LIBRARLIES +=  -lpthread -lstdc++

ifeq ($(UTEST_ENABLE_DEBUG), y)
  compile_flags += -g -O0
else
  compile_flags += -O2
endif

ifeq ($(CC), clang)
  CXX := clang++
  CPPFLAGS += -U__STRICT_ANSI__
  compile_flags += -fno-exceptions
endif

ifeq ($(UTEST_USE_CCACHE), y)
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
CPPFLAGS += $(includes) \
            -DGTEST_HAS_PTHREAD=1
ifeq ($(UTEST_ENABLE_DEBUG), y)
  CPPFLAGS += -D_DEBUG_
endif


# Helper functions
get_src_from_dir = $(wildcard $1/*.cpp) $(wildcard $1/*.cc) $(wildcard $1/*.c)
get_dirs_from_dirspec = $(wildcard $1)
get_src_from_dir_list = $(foreach dir, $1, $(call get_src_from_dir,$(dir)))
__src_to = $(subst .c,$1, $(subst .cpp,$1, $(subst .cc,$1,$2)))
src_to = $(addprefix $(UTEST_OBJECTS_DIR)/,$(call __src_to,$1,$2))
src_to_o = $(call src_to,.o,$1)
src_to_d = $(call src_to,.d,$1)
src_to_gcda = $(call src_to,.gcda,$1)
src_to_gcno = $(call src_to,.gcno,$1)
get_lib_from_dir = $(wildcard $1/*.a)
debug_print_list = $(foreach word,$1,echo "  $(word)";) echo;

test_target := $(UNIT_NAME)Test

gmock_ver := 1.7.0
gmock_dir := $(UTEST_FRAMEWORK_HOME)/gmock/gmock-$(gmock_ver)
gmock_sources := gmock-$(gmock_ver)/src/gmock-all.cc gmock-$(gmock_ver)/src/gmock_main.cc
gmock_objects := $(call src_to_o,$(gmock_sources))

gtest_dir := $(UTEST_FRAMEWORK_HOME)/gmock/gmock-$(gmock_ver)/gtest
gtest_sources := gmock-$(gmock_ver)/gtest/src/gtest-all.cc
gtest_objects := $(call src_to_o,$(gtest_sources))

includes += -I$(gmock_dir) -I$(gtest_dir) \
            -I$(gmock_dir)/include -I$(gtest_dir)/include \
            $(foreach dir, $(call get_dirs_from_dirspec, $(INCLUDE_DIRS)), -I$(dir))

sources = $(SOURCES)
objects = $(call src_to_o,$(sources))

test_sources = $(call get_src_from_dir_list, $(TEST_CODE_DIRS))
test_objects = $(call src_to_o,$(test_sources))

mock_sources = $(subst ../,,$(call get_src_from_dir_list, $(MOCK_CODE_DIRS)))
mock_objects = $(call src_to_o,$(mock_sources))

all_sources = $(sources) $(test_sources) $(mock_sources) $(gmock_sources) $(gtest_sources)
dependents = $(call src_to_d,$(all_sources))

# Test coverage with gcov
gcov_gcda_files = $(call src_to_gcda, $(all_sources))
gcov_gcno_files = $(call src_to_gcno, $(all_sources))
stuff_to_clean += $(gcov_gcda_files) \
                  $(gcov_gcno_files)

#The gcda files for gcov need to be deleted before each run
#To avoid annoying messages.
covfile_clean = $(SILENCE)rm -f $(gcov_gcda_files) $(gcov_output) $(gcov_report) $(gcov_error) ; \
                $(SILENCE)rm -rf $(LCOV_TRACEFILE) coverage-report
run_test_target = $(SILENCE) $(covfile_clean) ; ./$(test_target) ;
gen_covfile = $(LCOV) --capture --directory $(UTEST_OBJECTS_DIR) --output-file $(LCOV_TRACEFILE) --config-file $(LCOVRC); \
              $(LCOV) --remove $(LCOV_TRACEFILE) $(LCOV_REMOVE_PATTERN) --output-file $(LCOV_TRACEFILE) --config-file $(LCOVRC); \
              $(GENHTML) $(LCOV_TRACEFILE) --output-directory coverage-report --config-file $(LCOVRC)

stuff_to_clean = $(objects) $(test_objects) $(mock_objects) $(dependents) $(gmock_objects) $(gtest_objects)

vpath %.c $(PRODUCT_CODE_DIR)
vpath %.cpp $(dir $(MOCK_CODE_DIRS))
vpath %.cc $(UTEST_FRAMEWORK_HOME)/gmock

.PHONY: all
all: $(test_target)
	$(run_test_target)

.PHONY: coverage
coverage: all
	$(gen_covfile)

.PHONY: build
build: $(test_target)

$(test_target): $(objects) $(test_objects) $(mock_objects) $(gmock_objects) $(gtest_objects)
	$(LINK.o) -o $@ $^ $(LD_LIBRARLIES)

$(UTEST_OBJECTS_DIR)/%.o: %.cpp
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.cpp) -MMD -MP $(OUTPUT_OPTION) $<

$(UTEST_OBJECTS_DIR)/%.o: %.cc
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.cc) -MMD -MP $(OUTPUT_OPTION) $<

$(UTEST_OBJECTS_DIR)/%.o: %.c
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.c) -MMD -MP  $(OUTPUT_OPTION) $<

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(dependents)
endif

.PHONY: clean
clean:
	$(SILENCE)echo Making clean
	$(SILENCE)rm -rf $(stuff_to_clean)
	$(SILENCE)rm -rf gcov $(UTEST_OBJECTS_DIR)
	$(SILENCE)find . -name "*.gcno" | xargs rm -f
	$(SILENCE)find . -name "*.gcda" | xargs rm -f
	$(SILENCE)find . -name "$(test_target).*" | xargs rm -rf

ctags_option = -R --c++-kinds=+pl --fields=+iaS --extra=+q

.PHONY: tags
tags:
	ctags $(ctags_option) $(PRODUCT_CODE_DIR)
	ctags $(ctags_option) --append=yes .

.PHONY: cscope
cscope:
	cscope -R -q -b -s $(PRODUCT_CODE_DIR)

.PHONY: include_paths.vim
include_paths.vim:
	@echo "let include_paths = [" $(foreach path,$(INCLUDE_DIRS),\"$(shell cygpath -m $(path))\",) > /tmp/include_paths.txt
	@sed -e "s/,$$/ ]/g" /tmp/include_paths.txt > include_paths.vim
	@echo "let g:marching_include_paths += include_paths" >> include_paths.vim
	@rm -f /tmp/include_paths.txt

debug:
	@echo
	@echo "Target Source files:"
	@$(call debug_print_list,$(sources))
	@echo "Target Object files:"
	@$(call debug_print_list,$(objects))
	@echo "Test Source files:"
	@$(call debug_print_list,$(test_sources))
	@echo "Test Object files:"
	@$(call debug_print_list,$(test_objects))
	@echo "Mock Source files:"
	@$(call debug_print_list,$(mock_sources))
	@echo "Mock Object files:"
	@$(call debug_print_list,$(mock_objects))
	@echo "All Input Dependency files:"
	@$(call debug_print_list,$(dependents))
	@echo Stuff to clean:
	@$(call debug_print_list,$(stuff_to_clean))
	@echo Includes:
	@$(call debug_print_list,$(includes))
