#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --translates 1 | wc -l`
expected_result=3

displayResult $result $expected_result $test_number
