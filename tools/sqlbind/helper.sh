#!/bin/bash
# $1:操作
# $2:运行目录
# $3:项目配置文件

DBHOST=
DBPORT=
DBUSER=
DBPASSWORD=
DBDATABASE=

GOSCHEME=scheme

while read line;do
    eval "$line"
done < $2/$3 

if [ "$1" == "dump" ]; then
	# 执行数据库变更脚本
	MYSQL_PWD=$DBPASSWORD mysql -f -h$DBHOST -u$DBUSER -P$DBPORT \
		--default-character-set=utf8mb4 $DBDATABASE < $2/tools/modify_database.sql
	# 导出数据库DDL
	MYSQL_PWD=$DBPASSWORD mysqldump --compact --add-drop-table --default-character-set=utf8mb4 \
		-h$DBHOST -u$DBUSER -P$DBPORT -d $DBDATABASE > $2/tools/build_database.sql
fi

if [ "$1" == "bind" ]; then
	python3 sqlbind.py --ip=$DBHOST --port=$DBPORT \
		--username=$DBUSER --password=$DBPASSWORD --database=$DBDATABASE --signature=$2/tools/build_database.sql --cpp_out=$2
elif [ "$1" == "bindgo" ]; then
	mkdir -p $GOSCHEME/scheme/utils
	cp StreamBuf.go $GOSCHEME/scheme/utils/streambuf.go
	python3 sqlbind.py --ip=$DBHOST --port=$DBPORT \
		--username=$DBUSER --password=$DBPASSWORD --database=$DBDATABASE --signature=$2/tools/build_database.sql --go_out=$2/tools/sqlbind/$GOSCHEME
	rm -rf $GOSCHEME/scheme
fi
