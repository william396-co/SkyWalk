#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import json
from mergezone import *
from sendmail import MailSender

reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append( 'lib/PyMySQL-1.0.2-py3.7.egg' )

import pymysql as MySQLdb

class NameChecker() :
    def __init__( self, host, port, username, password, charset, db ) :
        self.zonelist = {}
        self.namepool = {}
        self.conn = MySQLdb.connect(
                host = host,
                user=username, passwd = password, port = port, charset=charset)
        self.conn.select_db( db )

    def __del__( self ) :
        self.conn.close()

    def get_zone( self, zoneid ) :
        zone = zoneid
        name = "unknown"
        hostid = 0
        if self.zonelist.has_key( zoneid ) :
            zone, name, hostid = self.zonelist[ zoneid ]
        else :
            zone, name, hostid = get_zone_info( zoneid )
            self.zonelist[ zoneid ] = (zone, name, hostid)
        return zone, name, hostid

    def loads( self, sqlcmd ) :
        handler = self.conn.cursor()
        handler.execute( sqlcmd )
        for row in handler.fetchall() :
            namelist = []
            name = row[1]
            id = int( row[0] )
            zone = int( row[2] )
            if self.namepool.has_key( name ) :
                namelist = self.namepool[ name ]
            namelist.append( (id, zone) )
            self.namepool[ name ] = namelist
        handler.close()

    def shrink( self ) :
        for key,value in self.namepool.items() :
            if len(value) == 1 :
                del self.namepool[ key ]

    def ownerid( self, guildid ) :
        roleid = 0
        handler = self.conn.cursor();
        handler.execute( "SELECT `roleid` FROM `GuildMember` WHERE `guildid` = %d AND `type`=1" % guildid )
        result = handler.fetchone()
        if result :
            roleid = result[0]
        handler.close()
        return roleid

    def replace( self, sqlcmds ) :
        for key, value in self.namepool.items() :
            index = 0
            for v in value :
                index = index + 1
                zone, zonename, hostid = self.get_zone( v[1] );
                name = "s%d_%d.%s" % (zone, index, key)
                handler = self.conn.cursor()
                for sqlcmd in sqlcmds :
                    handler.execute( sqlcmd % (name, v[0]) )
                handler.close()
                print( "Change %d's Name '%s' to '%s' ..." % (v[0], key, name) )
            self.conn.commit()

    def sendmail( self, tag, sender, attachments ) :
        for key, value in self.namepool.items() :
            for v in value :
                if tag == 'role' :
                    sender.compose( 40001, v[0], "合服补偿", "合服补偿邮件", attachments )
                else :
                    sender.compose( 40001, self.ownerid(v[0]), "合服补偿", "合服补偿邮件", attachments )

    def save2( self, filename ) :
        patchfile = open( filename, 'w' )
        for key,value in self.namepool.items() :
            for v in value :
                patchfile.write( '\'%s\',%d,%d\n' % (key, v[0], v[1]) )
        patchfile.close()

class VariableProcessor() :
    def __init__( self, host, port, username, password, charset, db ) :
        self.roleid = 0
        self.hostid = 0
        self.conn = MySQLdb.connect(
                host = host,
                user=username, passwd = password, port = port, charset=charset)
        self.conn.select_db( db )
        self.load()

    def __del__( self ) :
        self.conn.close()

    def load( self ) :
        handler = self.conn.cursor()
        handler.execute( "SELECT `roleid`,`id` FROM `Variable`" )
        for row in handler.fetchall() :
            curid = int( row[0] )
            if self.roleid < curid :
                self.roleid = curid
                self.hostid = int( row[1] )
        handler.close()

    def update( self, roleid ) :
        if self.roleid < roleid :
            self.roleid = roleid
            handler = self.conn.cursor()
            handler.execute( "UPDATE `Variable` set `roleid=`%d WHERE `id=`%d" % ( self.roleid, self.hostid ) )
            handler.close()
            self.conn.commit()

def check_role_name( taskid, sender, zoneid, database ) :
    query_sql = "SELECT `roleid`, `name`, `zone` FROM `CharBrief`"
    update_sql1 = "UPDATE `CharBase` set `name`='%s' WHERE `roleid`=%d"
    update_sql2 = "UPDATE `CharBrief` set `name`='%s' WHERE `roleid`=%d"
    # 根据zoneid获得数据库信息
    host,port,username,password,charset,db = get_db_info( zoneid )
    checker = NameChecker( host, port, username, password, charset, database )
    checker.loads( query_sql )
    checker.shrink()
    checker.replace( [update_sql1, update_sql2] )
    # 不用发改名卡
    #checker.sendmail( 'role', sender,
    #        [ {'type':1,'object':600001,'count':20}, {'type':1,'object':10003,'count':100} ] )
    checker.save2( '%s/rolename.patch' % get_workpath(taskid) )

def check_guild_name( taskid, sender, zoneid, database ) :
    query_sql = "SELECT `guildid`, `name`, `zone` FROM `Guild`"
    update_sql = "UPDATE `Guild` set `name`='%s' WHERE `guildid`=%d"
    # 根据zoneid获得数据库信息
    host,port,username,password,charset,db = get_db_info( zoneid )
    checker = NameChecker( host, port, username, password, charset, database )
    # 查询
    checker.loads( query_sql )
    checker.shrink()
    checker.replace( [update_sql] )
    # 不用发改名卡
    #checker.sendmail( 'guild', sender,
    #        [ {'type':1,'object':600001,'count':20}, {'type':1,'object':10003,'count':100} ] )
    checker.save2( '%s/guildname.patch' % get_workpath(taskid) )

# 主函数
if __name__ == "__main__" :
    if len( sys.argv ) < 3 :
        print( "afterprocess.py <taskid> <zoneid> [slaveid1 ... slaveidN] ." )
        sys.exit()
    taskid = int( sys.argv[1] )
    zoneid = int( sys.argv[2] )
    zone, zonename, hostid = get_zone_info( zoneid )
    host,port,username,password,charset,db = get_db_info( zoneid )
    dbname = get_merged_dbname( db, taskid )
    # 初始化邮件发送器
    sender = MailSender( zoneid, hostid,
            host, port, username, password, charset, dbname )
    # 找出重名的玩家
    check_role_name( taskid, sender, zoneid, dbname )
    # 找出重名的公会
    check_guild_name( taskid, sender, zoneid, dbname )
    # 更新变量表
    updater = VariableProcessor( zoneid, hostid,
            host, port, username, password, charset, dbname )
    for i in range( 3, len(sys.argv) ) :
        slaveid = int( sys.argv[i] )
        slave_processor = VariableProcessor( slaveid, get_db_info(slaveid) )
        updater.update( slave_processor.roleid )
    # 关闭邮件发送其
    sender.disconnect()
