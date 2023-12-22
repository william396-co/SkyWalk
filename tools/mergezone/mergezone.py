#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import shutil

reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append( 'lib/PyMySQL-1.0.2-py3.7.egg' )

import json
import urllib3
import MySQLdb

def get_service() :
    env = os.environ
    if env.has_key( "SERVICE_URL" ) :
        return env["SERVICE_URL"]
    return 'http://172.21.161.138:3351'

# 获取区服信息
def get_zone_info( zoneid ) :
    http = urllib3.PoolManager()
    r = http.request( 'GET',
            '%s/game_service/get_zone_info?zoneid=%d' % (get_service(), zoneid) );
    zone = json.loads(r.data.decode('utf-8'))['zone_info']['zoneid']
    name = json.loads(r.data.decode('utf-8'))['zone_info']['zonename']
    hostid = json.loads(r.data.decode('utf-8'))['zone_info']['hid']
    return zone, name, hostid

# 获取DB信息
def get_db_info( zoneid ) :
    http = urllib3.PoolManager()
    r = http.request( 'GET',
            '%s/game_service/get_db_info?zoneid=%d' % (get_service(), zoneid) );
    host = json.loads(r.data.decode('utf-8'))['db_info']['host']
    port = json.loads(r.data.decode('utf-8'))['db_info']['port']
    username = json.loads(r.data.decode('utf-8'))['db_info']['user']
    password = json.loads(r.data.decode('utf-8'))['db_info']['pw']
    charset = json.loads(r.data.decode('utf-8'))['db_info']['enc']
    db = json.loads(r.data.decode('utf-8'))['db_info']['db_name']
    return host,port,username,password,charset,db

# 获取合服后的DB名字
def get_merged_dbname( db, taskid ) :
    txt = db.split( '_' )
    if len(txt) > 2 :
        return '%s_%s_%d' % ( txt[0], txt[1], taskid )
    return '%s_%d' % ( db, taskid )

# 获取工作目录
def get_workpath( taskid ) :
    workpath = "vardata_%d" % taskid
    return workpath

# 主函数
if __name__ == "__main__" :
    if len( sys.argv ) != 3 :
        print( "mergezone.py <taskid> <dstzone> ." )
        sys.exit()
    taskid = int( sys.argv[1] )
    zoneid = int( sys.argv[2] )
    workpath = get_workpath( taskid )
    if os.path.exists( workpath ) :
        shutil.rmtree( workpath )
    os.mkdir( workpath )
    host,port,username,password,charset,db = get_db_info( zoneid )
    drop_db_sql = 'DROP DATABASE IF EXISTS %s' % get_merged_dbname(db, taskid)
    os.system( "mysql -h%s -u%s -p%s -P%d -e '%s'" % (host, username, password, port, drop_db_sql) )
