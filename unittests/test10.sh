#!/bin/sh
source ./unittests_common.sh

result=`$tatoparser_bin -p '.*bonjour.*' | wc -l`
expected_result=1

displayResult $result $expected_result $test_number
