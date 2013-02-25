#!/bin/sh
. ./unittests_common.sh

mv lists.csv lists.csv.back
touch lists.csv
result=`$tatoparser_bin --in-list abc &>/dev/null`
if [ $? -eq 0 ]
then
	result=0
	expected_result=0
else
	result=0
	expected_result=1
fi
rm lists.csv
mv lists.csv.back lists.csv

displayResult $result $expected_result $test_number
