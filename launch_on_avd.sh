#!/bin/sh

default_avd_name="mon_avd"
time_in_seconds_before_installing_apk=90
apk_path="./src/java/tatoparser.apk"

# if the APK is not there, then create it
if [ ! -f "$apk_path" ]
then	
	cd src/java
	make all
	cd ../..
fi

if [ ! -f "$apk_path" ] 
then
	exit 1
fi

avd=${1-"$default_avd_name"}
android_sdk_path=$(grep '^ANDROID_SDK_PATH =' src/java/Makefile | sed 's/ANDROID_SDK_PATH = //')

# launching the emulator
${android_sdk_path}/tools/emulator -wipe-data -avd "$avd"&

sleep $time_in_seconds_before_installing_apk

#copying the apk in the emulator
${android_sdk_path}/platform-tools/adb -e install "$apk_path"
