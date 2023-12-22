#!/bin/bash
# $1 - 项目目录
# $2 - 导出的目录
# $3 - 项目配置文件

# 项目配置中读取SVN的地址和用户名密码
while read line; do
	eval "$line"
done < $1/$3

CONFIGURE=/tmp/design_`whoami`_`date +%s`
SVNPARAMS="--non-interactive --trust-server-cert-failures="unknown-ca,cn-mismatch,expired,not-yet-valid,other" --username $SVNUSER --password $SVNPASSWORD"

# 导出到指定的目录中
svn export --depth files --force $SVNPARAMS $SVNURL/$SVNBRANCH/ $CONFIGURE

# 生成代码
python3 generate.py $CONFIGURE $2

rm -rf $CONFIGURE
