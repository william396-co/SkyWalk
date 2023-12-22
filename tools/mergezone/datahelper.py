#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import pymysql as MySQLdb

reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append( 'lib/PyMySQL-1.0.2-py3.7.egg' )

import json
import urllib3
from mergezone import *

# 需要跳过/丢弃的表
use_master_tables = [ 'Activity', 'SystemVariable', 'Variable', 'GMMail' ]
drop_tables = [ 'CombatVideo', 'DarkArenaLog', 'GuildJournal', 'ImportZone', 'OfflineMessage', 'ZoneGift', 'ZoneVisit', 'DungeonSystem', 'DAPlayoffBet', 'DAPlayoffGroup', 'DAPlayoffMagazine', 'DAPlayoffRankUser' ]

def get_tables( host,port,username,password,charset,db ) :
    tables = []
    conn = MySQLdb.connect( host=host,
            user=username, passwd=password, port=port, charset=charset )
    conn.select_db( db )
    handler = conn.cursor()
    handler.execute( "SHOW TABLES" )
    for row in handler.fetchall() :
        tables.append( row[0] )
    handler.close()
    handler = conn.cursor()
    handler.execute( "show VARIABLES like 'net_buffer_length'" )
    length = int( handler.fetchone()[1] )
    handler.close()
    conn.close()
    return tables, min( [length,4*1024*1024] )

def get_table_lines( host, port, username, password, charset, db ) :
    tables = {}
    conn = MySQLdb.connect( host=host,
            user=username, passwd=password, port=port, charset=charset )
    conn.select_db( db )
    handler = conn.cursor()
    handler.execute( "SHOW TABLES" )
    for row in handler.fetchall() :
        h = conn.cursor()
        h.execute( "SELECT COUNT(1) FROM %s" % row[0] )
        tables[ row[0] ] = h.fetchone()[0]
        h.close()
    handler.close()
    conn.close()
    return tables

def export_from_db( workpath, srcid, dstid, dbname = '' ) :
    ignore_tables = []
    # 获取数据主机信息
    host,port,username,password,charset,db = get_db_info( srcid )
    if dbname != '' : db = dbname
    mysqlurl = 'mysql://%s:%s@%s:%d/%s' %( username, password, host, port, db )
    srcpath = "%s/%s" % ( workpath, db )
    if not os.path.exists( srcpath ) : os.mkdir( srcpath )
    tables, packet_length = get_tables( host,port,username,password,charset,db )
    if dbname == '' :
        for t in drop_tables : ignore_tables.append( t )
        # 导出表结构
        if srcid == dstid :
            cmd='mysqldump --compact --add-drop-table \
                -h%s -u%s -p%s -P%d --default-character-set=%s -d %s >> %s/build_database.sql' % (host, username, password, port, charset, db, workpath)
            os.system( cmd )
        else :
            for t in use_master_tables : ignore_tables.append( t )
    # 导出数据
    for table in tables :
        if table in ignore_tables :
            print( 'export %s:%s to \'%s/%s.sql\' ... [Skiped]' % ( mysqlurl, table, srcpath, table ) )
            continue
        cmd='mysqldump --compact -t --complete-insert --net_buffer_length=%d \
                -h%s -u%s -p%s -P%d --default-character-set=%s %s %s >> %s/%s.sql' % (packet_length, host, username, password, port, charset, db, table, srcpath, table)
        print( 'export %s:%s to \'%s/%s.sql\' ...' % ( mysqlurl, table, srcpath, table ) ),
        sys.stdout.flush()
        os.system( cmd )
        print( '\rexport %s:%s to \'%s/%s.sql\' ... [Succeed]' % ( mysqlurl, table, srcpath, table ) )
        sys.stdout.flush()
    return "%s/build_database.sql" % workpath, srcpath

# 主函数
if __name__ == "__main__" :
    if len( sys.argv ) != 4 :
        print( "datahelper.py <taskid> <dstzone> <srczone> ." )
        sys.exit()
    taskid = int( sys.argv[1] )
    dstzoneid = int( sys.argv[2] )
    srczoneid = int( sys.argv[3] )
    # 导出
    buildsql, sqlpath = export_from_db( get_workpath(taskid), srczoneid, dstzoneid )
    # 获取数据主机信息
    host,port,username,password,charset,db = get_db_info( dstzoneid )
    merge_db = get_merged_dbname( db, taskid )
    mysqlurl = 'mysql://%s:%s@%s:%d/%s' %( username, password, host, port, merge_db )
    #  创建合并数据库
    create_db_sql = 'CREATE DATABASE IF NOT EXISTS %s Character Set %s' % ( merge_db, charset )
    os.system( "mysql -h%s -u%s -p%s -P%d --default-character-set=%s -e '%s'" % (host, username, password, port, charset, create_db_sql) )
    # 导入表结构
    if dstzoneid == srczoneid :
        cmd = 'mysql -h%s -u%s -p%s -P%d --default-character-set=%s < %s %s' % (host, username, password, port, charset, buildsql, merge_db )
        os.system( cmd )
        print( 'import \'%s\' to %s [Succeed]' % (buildsql, mysqlurl) )
    sys.stdout.flush()
    # 导入数据库
    allfiles = os.listdir( sqlpath )
    allfiles.sort()
    for filename in allfiles :
        cmd = 'mysql -h%s -u%s -p%s -P%d --default-character-set=%s < %s/%s %s' % (host, username, password, port, charset, sqlpath, filename, merge_db )
        print( 'import \'%s/%s\' to %s ...' % (sqlpath, filename, mysqlurl) ),
        sys.stdout.flush()
        os.system( cmd )
        print( '\rimport \'%s/%s\' to %s ... [Succeed]' % (sqlpath, filename, mysqlurl) )
        sys.stdout.flush()
