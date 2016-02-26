#!/bin/sh

ut_framework_dir=$1

lcov=${ut_framework_dir}/tool/lcov/bin/lcov
genhtml=${ut_framework_dir}/tool/lcov/bin/genhtml
lcovrc=${ut_framework_dir}/tool/setting/lcovrc

lcov_tracefile=lcov.info

${lcov} --capture \
        --directory ./obj \
        --output-file ${lcov_tracefile} \
        --config-file ${lcovrc}

${lcov} --remove lcov.info \
        "/usr/lib/*" "/usr/include/*" "*gtest*" "*gmock*" \
        "*mock*" "*Test*" \
        --output-file lcov.info \
        --config-file ${lcovrc}

${genhtml} ${lcov_tracefile} --output-directory coverage-report --config-file ${lcovrc}
