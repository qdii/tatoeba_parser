#!/bin/sh
failure=0
for file in test*
do
	if ! sh "$file"
	then
		failure=1
		break;
	fi
done
exit $failure
