#!/bin/sh
. ./unittests_common.sh

temp_config_file=$(mktemp)
echo "lang=fra" > "$temp_config_file"
echo "lang=eng" >> "$temp_config_file"

result=`$tatoparser_bin --regex '.* pain .*' --config-path "$temp_config_file" | wc -l`

rm -f "$temp_config_file"

expected_result=2

displayResult $result $expected_result $test_number
