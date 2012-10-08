#!/bin/sh
source ./unittests_common.sh

result=`$tatoparser_bin --regex '.*' | wc -l`
expected_result=6

displayResult $result $expected_result $test_number
