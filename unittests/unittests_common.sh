#!/bin/sh

displayResult()
{
	if [ "$1" = "$2" ]
	then
		printf "[..] test %-2s [OK]\n"  "$3"
	else
		printf "[..] test %-2s [FAIL]\n" "$3"
	fi
}

tatoparser_bin="${1-./tatoparser}"
test_number=$(echo $0 | sed -e 's/test\([0-9]*\).sh/\1/')
