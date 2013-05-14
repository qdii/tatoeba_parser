#!/bin/sh
. ./unittests_common.sh

mv sentences.csv sentences.csv.backup
mv sentences.csv.corrupted sentences.csv

result=`$tatoparser_bin --fuzzy 1 "bonjour" | wc -l`
expected_result=1

mv sentences.csv sentences.csv.corrupted
mv sentences.csv.backup sentences.csv

displayResult $result $expected_result $test_number
