#!/bin/sh
. ./unittests_common.sh

result=`$tatoparser_bin --has-id 3`
expected_result='你在干什麼啊？'

displayResult $result $expected_result $test_number
