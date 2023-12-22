#!/bin/bash

date=$1
number=0
newborn=0

if [ -f log/worlded-$date.log ]; then
	newborn=`grep "newborn" log/worlded-$date.log  | cut -d ":" -f 11 | cut -d "," -f 1 | sort`
	number=`grep "newborn" log/worlded-$date.log  | cut -d ":" -f 11 | cut -d "," -f 1 | sort | wc -l`
fi

echo "Date: "$date", CreateRoles: "$number

if [ "$number" == "0" ]; then
	exit 1
fi

for day in {1..7}; do
	login_number=0
	nextday=`date -d"$date +$day day" +%Y%m%d`
	filename=log/worlded-$nextday.log
	if [ -f $filename ]; then
		for id in $newborn; do
			login_res=`grep "$id" $filename`
	if [ "$login_res" != "" ]; then
		((login_number++))
	fi
		done
	fi
	rate=`awk 'BEGIN{printf "%.2f%\n",('$login_number'/'$number')*100}'`
	echo RR$day: $nextday, $login_number"("$rate")"
done
