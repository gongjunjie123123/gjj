#!/bin/bash

#usage:		./nkil8.sh 1

ss=`ps aux |grep ne8k|grep gongjun`
echo $ss
var=`echo $ss|awk -F ' ' '{print $2}'`
echo $var
if [ $1 -eq 1 ]; then
echo "kill $var"
kill -9 $var
fi

