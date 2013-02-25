#!/bin/sh
. ./unittests_common.sh

mv lists.csv lists.csv.back
touch lists.csv
result=`$tatoparser_bin --in-list abc`
expected_result=0
rm lists.csv
mv lists.csv.back lists.csv

displayResult $result $expected_result $test_number
