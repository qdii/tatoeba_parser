#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin -u 'qdii' | wc -l`
expected_result=1

displayResult $result $expected_result $test_number
