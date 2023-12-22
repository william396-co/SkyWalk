#!/bin/bash

if [ $# -lt 3 ]; then
	echo "$0 <redis_server> <redis_port> <key1...keyN>"
	exit 0
fi

# 参数
host=$1
port=$2
shift 2

# 解析keys
for arg in $@
do
	keys+="$arg "
done

rdblist=`redis-cli -h $host -p $port cluster nodes | grep master | awk -F ' ' '{print $2}' | awk -F '@' '{print $1}'`

for rdb in $rdblist
do
	echo "$rdb ... "
	ip=`echo $rdb | cut -d : -f 1`
	port=`echo $rdb | cut -d : -f 2`
	for key in $keys
	do
		redis-cli -c -h $ip -p $port keys $key | xargs -i redis-cli -h $ip -p $port del {} 
	done
done
