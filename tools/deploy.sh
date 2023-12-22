#!/bin/bash

set -eux

SRCPATH="/data/codes"
nprocessors=`cat /proc/cpuinfo |grep "processor"|wc -l`

init_ope()
{
	mkdir -p /data/ope/config
	cd /data/codes
	git clone git@git.wekeystudio.com:techcenter/ope-v2.git
	cp -r ope-v2/ope-back/static ../ope
	cp ope-v2/ope-back/config/settings.dev.yml /data/ope/config/settings.yml
	sed 's/8090/9001/' /data/ope/config/settings.yml
	sed "s/172.28.1.12/$3/g" /data/ope/config/settings.yml
	sed "s/password:/password: 'zRAAosw38_'/g" /data/ope/config/settings.yml
}

init_service()
{
	mkdir -p /data/service
	cd /data/codes
	git clone git@git.wekeystudio.com:techcenter/service.git
	cp service/config.toml /data/service
	sed 's/9091/9002/' /data/service/config.toml
	sed "s/172.28.1.12/$3/g" /data/service/config.toml
	sed 's/password = ""/password = "zRAAosw38_"/g' /data/service/config.toml
}

init_loginserver()
{
	mkdir -p /data/loginserver
	cd /data/codes
	git clone git@git.wekeystudio.com:techcenter/loginserver.git
	cp -r loginserver/config /data/loginserver
	sed 's/10831/8001/' /data/loginserver/config/loginserver.json
	sed 's/9090/8098/' /data/loginserver/config/loginserver.json
	sed 's/5/100000/' /data/loginserver/config/loginserver.json
	sed "s/172.28.1.12/$3/g" /data/loginserver/config/loginserver.json
	sed "s/ope/ope, xxx/" /data/loginserver/config/loginserver.json
	sed 's/password: ""/password: "zRAAosw38_"/' /data/loginserver/loginserver.json
}

install_opeserver()
{
	cd /data/codes/ope-v2
	git pull
	cd ope-back
	go mod tidy
	go mod download
	go build -o bin/ope main.go
	sudo cp bin/ope /data/ope
	sudo cp -r static /data/ope
	cd /data/ope
	nohup ./ope server -c=./config/settings.yml -a=true > ope.log 2>&1 &
}

install_opeweb()
{
	cd /data/codes/ope-v2
	git pull
	cd ope-front
	npm install
	npm run build:stage
	sudo cp -r dist /usr/local/nginx/html
	sudo /usr/local/nginx/sbin/nginx -s reload
}

install_service()
{
	cd /data/codes/service
	git pull
	go mod tidy
	go mod download
	go build -o bin/service main.go
	sudo cp bin/service /data/service
	cd /data/service
	nohup ./service > service.log 2>&1 &
}

install_loginserver()
{
	cd /data/codes/loginserver
	git pull
	go mod tidy
	go mod download
	go build -o bin/loginserver main.go
	sudo cp bin/loginserver /data/loginserver
	cd /data/loginserver
	nohup ./loginserver > loginserver.log 2>&1 &
}

import_db()
{
	cd /data/codes/ope-v2
	mysql -udevelop -pzRAAosw38_ -h $3 < build_database.sql
	mysql -udevelop -pzRAAosw38_ -h $3 < modify_database.sql
}

install_nginx()
{
	cd
	wget -ic https://nginx.org/download/nginx-1.9.9.tar.gz
	tar -zxvf nginx-1.9.9.tar.gz
	cd nginx-1.9.9
	./configure
    sed -i '36d' src/os/unix/ngx_user.c
	make CFLAGS='-Wno-implicit-fallthrough' -j${nprocessors}
	sudo make install
	sudo cp /data/codes/server/tools/nginx/nginx.conf /usr/local/nginx/conf
	sudo /usr/local/nginx/sbin/nginx -t
	sudo /usr/local/nginx/sbin/nginx
}

install_nodejs()
{
	cd
	wget -ic https://nodejs.org/download/release/v14.17.3/node-v14.17.3.tar.gz
	tar -zxvf node-v14.17.3.tar.gz
	cd node-v14.17.3
	./configure
	make -j${nprocessors}
	sudo make install
}

# 参数
if [ $# != 3 ]; then
    echo 'params error: param1(ope or login), param2(ope ip) param3(mysql ip)'
	exit 1
fi

# go mod
go env -w GO111MODULE=on

# 安装
if [ ! -d $SRCPATH ]; then
	mkdir -p $SRCPATH
	if [ "$1" == "ope" ]; then
		import_db
		install_nginx
		install_nodejs
		init_ope
		init_service	
		install_opeserver
		install_opeweb
		install_service
	elif [ "$1" == "login" ]; then
		init_loginserver
		install_loginserver
	else
		echo "params error: param1 must ope or login"
	fi
fi

exit $?
