#!/bin/sh
status=$1
if [ "$status" = "ok" ]
then
echo "led has been tested, it is ok"
fi

if [ "$status" = "err" ]
then
echo "led has been tested, it is fail"
fi