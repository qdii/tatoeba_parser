#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --is-translatable-in eng | wc -l`
expected_result=0

displayResult $result $expected_result $test_number
