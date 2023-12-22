#!/bin/bash
#
# 开服脚本 build_zone.sh <JenkinsHost> [ZoneHome]
#

# 定义
ROOT_HOME=/data/game
GAMEDC_HOME=/data/gamedc

# $1 - JENKISHOST
# $2 - from
# $3 - to
remote_copy()
{
	scp -o StrictHostKeyChecking=no -qrB game@$1:$2 $3
}


# 创建系统目录
mkdir -p $GAMEDC_HOME
# 第三方目录
if [ ! -d $ROOT_HOME/lib ]; then
	echo "Install Thirdparty libs ..."
	mkdir -p $ROOT_HOME/lib
	remote_copy $1 $ROOT_HOME/lib/* $ROOT_HOME/lib/
fi

if [ "$2" != "" ]; then
	echo "Install Zone $2 ..."
	mkdir -p $ROOT_HOME/$2
	mkdir -p $ROOT_HOME/$2/log
	mkdir -p $ROOT_HOME/$2/run
	mkdir -p $ROOT_HOME/$2/meta
	mkdir -p $ROOT_HOME/$2/scripts
	mkdir -p $ROOT_HOME/$2/config
	mkdir -p $ROOT_HOME/$2/config/gameconfig
	if [ ! -d $ROOT_HOME/$2/lib ]; then
		ln -s $ROOT_HOME/lib $ROOT_HOME/$2/lib
	fi
fi
