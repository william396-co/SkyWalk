#!/bin/bash

for file in `ls run`; do
	if [ "$pidlist" == "" ]; then
		pidlist=`cat run/$file`
	else
		pidlist=$pidlist','`cat run/$file`
	fi
done

top -c -p $pidlist

