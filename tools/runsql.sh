#!/bin/bash

get_inifile_value()
{
	file=$1
	section=$2
	key=$3
	echo `awk -F '=' '/\['$section'\]/{a=1}a==1&&$1~/'$key'/{print $2;exit}' $file`
}

host=$(get_inifile_value config/dataserver.conf Database host)
port=$(get_inifile_value config/dataserver.conf Database port)
username=$(get_inifile_value config/dataserver.conf Database username)
password=$(get_inifile_value config/dataserver.conf Database password)
database=$(get_inifile_value config/dataserver.conf Database database)

echo mysql://$username:$password@$host:$port/$database

if [ "$1" == "" ]; then
	mysql -f -h$host -u$username -p$password $database
else
	mysql -f -h$host -u$username -p$password $database < $1
fi
