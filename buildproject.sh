#!/bin/bash

# ./buildproj.sh
# ./buildproj.sh -cCestrpdoi

params=""
targets=""
buildpath="build"
projprofile=".projectprofile"
if [ "`uname`" == "Darwin" ]; then
	nprocessors=`sysctl -n machdep.cpu.thread_count`
else
	nprocessors=`cat /proc/cpuinfo |grep "processor"|wc -l `
fi

config_project()
{
	echo -n "SVNURL: "
	read svnurl
	echo -n "SVNUser: "
	read svnuser
	echo -n "SVNPassword: "
	read svnpassword
	echo -n "SVNBranch: "
	read svnbranch
	echo -n "DBHost: "
	read dbhost
	echo -n "DBPort: "
	read dbport
	echo -n "DBUser: "
	read dbuser
	echo -n "DBPassword: "
	read dbpassword
	echo -n "DBDatabase: "
	read dbdatabase

	echo "ROOT"=`pwd` >> $projprofile
	echo "SVNURL"=$svnurl >> $projprofile
	echo "SVNUSER"=$svnuser >> $projprofile
	echo "SVNPASSWORD"=$svnpassword >> $projprofile
	echo "SVNBRANCH"=$svnbranch >> $projprofile
	echo "DBHOST="$dbhost >> $projprofile
	echo "DBPORT="$dbport >> $projprofile
	echo "DBUSER="$dbuser>> $projprofile
	echo "DBPASSWORD="$dbpassword >> $projprofile
	echo "DBDATABASE="$dbdatabase >> $projprofile
}

install_env()
{
	mkdir -p bin
	mkdir -p bin/run
	mkdir -p bin/log
	mkdir -p bin/meta
	mkdir -p bin/debug
	mkdir -p lib
	ln -s `pwd`/lib bin/lib
	ln -s `pwd`/scripts bin/scripts
	cp /usr/local/lib/libevlite.so.9 lib
}

while getopts 'cCevstimrbp:o:' OPT; do
	case $OPT in
		c) rm -rf $buildpath ;;
		C) cmake --build $buildpath --target clean ;;
		p) params=$params" -D BUILD_PLATFORM=$OPTARG";;
		v) params=$params" -D CMAKE_VERBOSE_MAKEFILE=ON";;
		s) targets=$targets" sqlbind";;
		e) targets=$targets" cfggenerate";;
		t) params=$params" -D BUILD_EXAMPLES=ON" ;;
		i) rm -rf $projprofile; config_project; install_env; ;;
		m) params=$params" -D CMAKE_BUILD_MEMCHK=OFF" ;;
		r) params=$params" -D CMAKE_BUILD_TYPE=Release -D CMAKE_BUILD_MEMCHK=OFF" ;;
		b) params=$params" -D CMAKE_BUILD_TYPE=Beta -D CMAKE_BUILD_MEMCHK=OFF" ;;
		o) params=$params" -D PROJECT_OUTPUT_PATH=$OPTARG"; mkdir -p $OPTARG;;
		?) echo "Usage: `basename $0` [-cCvsetrmbi] [-p PLATFORM] [-o OUTPUT]"; exit 1;;
	esac
done

shift $(($OPTIND - 1))

#  重新编译CMAKE
if [ ! -d $buildpath ]; then
	params=$params" "
	mkdir -p $buildpath
fi

# CMAKE参数变化的情况下
if [ "$params" != "" ]; then
	cd $buildpath; cmake $params ..; cd $OLDPWD
fi

#  有其他目标的情况下
if [ "$targets" != "" ] ; then
	cmake --build $buildpath --target $targets
	exit 0
fi

cd $buildpath
make -j `expr $nprocessors / 2 + 1`
