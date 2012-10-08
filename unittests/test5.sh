#!/bin/sh
source ./unittests_common.sh

result=`$tatoparser_bin --regex '.*我.*' | wc -l`
expected_result=2

displayResult $result $expected_result $test_number
