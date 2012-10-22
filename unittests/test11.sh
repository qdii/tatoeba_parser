#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --regex '.* pain .*' --lang 'fra' --lang 'eng' | wc -l`
expected_result=2

displayResult $result $expected_result $test_number
