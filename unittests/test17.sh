#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --translates 1111111 | wc -l`
expected_result=0

displayResult $result $expected_result $test_number
