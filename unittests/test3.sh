#!/bin/sh
source ./unittests_common.sh

result=`$tatoparser_bin --is-linked-to 4 | wc -l`
expected_result=0

displayResult $result $expected_result $test_number
