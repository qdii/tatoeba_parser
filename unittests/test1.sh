#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --lang cmn | wc -l`
expected_result=5

displayResult $result $expected_result $test_number
