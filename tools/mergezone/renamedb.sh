#!/bin/bash
# sh renamedb.sh 172.21.161.138 ssdev soul123456 3306 UTF8 yuu_s0001_backup merged_yuu_s0001

if [ "$#" -ne "7"  ]; then
	echo "./renamedb.sh <host> <username> <password> <port> <charset> <srcdb> <dstdb>"
	exit
fi

params="-h$1 -u$2 -p$3 -P$4"

# 创建新库
mysql $params -e "CREATE DATABASE IF NOT EXISTS $7 Character Set $5"

# 重命名所有表
list_table=$(mysql $params -Nse "select table_name from information_schema.TABLES where TABLE_SCHEMA='$6'")
for table in $list_table
do
	mysql $params -e "rename table $6.$table to $7.$table"
done

# 删除老库
mysql $params -e "DROP DATABASE $6"
