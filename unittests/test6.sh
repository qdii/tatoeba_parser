#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --regex '.*我.*' --lang fra | wc -l`
expected_result=0

displayResult $result $expected_result $test_number
