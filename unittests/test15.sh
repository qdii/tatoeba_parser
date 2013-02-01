#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --in-list bla | wc -l`
expected_result=2

displayResult $result $expected_result $test_number
