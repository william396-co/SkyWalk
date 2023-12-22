#!/bin/bash

master=0
taskid=0
slavelist=''

if [ $# -lt 3 ]; then
	echo $0" <taskid> <master> <slave0...slaveN>"
	exit 0
fi

taskid=$1
master=$2

index=0
for arg in $*
do
	if [ $index -gt 1 ]; then
		slavelist=$slavelist" "$arg
	fi
	let index+=1
done

# 准备工作
SERVICE_URL="http://172.21.161.138:3351" python mergezone.py $taskid $master

# 导入主服
SERVICE_URL="http://172.21.161.138:3351" python datahelper.py $taskid $master $master

# 导入从服
for slave in $slavelist
do
	SERVICE_URL="http://172.21.161.138:3351" python datahelper.py $taskid $master $slave
done

# 合服后处理 
SERVICE_URL="http://172.21.161.138:3351" python afterprocess.py $taskid $master $slavelist

# 统计
SERVICE_URL="http://172.21.161.138:3351" python statistics.py $taskid $master $slavelist
