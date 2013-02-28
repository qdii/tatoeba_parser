#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --has-tag HSK | wc -l`
expected_result=1

displayResult $result $expected_result $test_number
