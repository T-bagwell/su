#/usr/bin/bash

URL=$1
BASENAME=`echo "$URL" | awk -F"index" '{ print $1 }'`
declare -a TSFILE

echo "$BASENAME"
echo "======="
#wget $URL -O index.m3u8
TSFILE=grep ".ts" index.m3u8


sleep 1
