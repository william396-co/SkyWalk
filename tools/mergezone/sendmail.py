#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import time
import json

reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append( 'lib/PyMySQL-1.0.2-py3.7.egg' )

import pymysql as MySQLdb

class MailSender() :
    def __init__( self, zoneid, hostid, host, port, username, password, charset, db ) :
        self.zoneid = zoneid
        self.hostid = hostid
        self.conn = MySQLdb.connect(
                host = host,
                user=username, passwd = password, port = port, charset=charset)
        self.conn.select_db( db )
        self.mailid = self.load_mailid()

    def disconnect( self ) :
        self.conn.close()

    def generate( self ) :
        self.mailid = self.mailid + 1
        handler = self.conn.cursor()
        handler.execute( "UPDATE `Variable` SET `mailid`=%d WHERE `id`=%d" % (self.mailid, self.hostid) )
        self.conn.commit()
        handler.close()
        gen_id = self.mailid
        gen_id = gen_id | ( self.hostid << 42 )
        gen_id = gen_id | ( self.zoneid << 48 )
        gen_id = gen_id | ( 3 << 60 )
        return gen_id

    def load_mailid( self ) :
        mailid = 0
        handler = self.conn.cursor()
        handler.execute( "SELECT `id`,`mailid` FROM `Variable` WHERE `id`=%d LIMIT 1" % self.hostid )
        result = handler.fetchone()
        if result :
            mailid = result[1]
        handler.close()
        return mailid

    def compose( self, sender, receiver, title, content, attachments ) :
        mailid = self.generate()
        sendername = ""
        sendtime = int( time.time() )
        json_attachments = json.dumps( attachments )
        # escape
        sendername = MySQLdb.escape_string( sendername )
        title = MySQLdb.escape_string( title )
        content = MySQLdb.escape_string( content )
        json_attachments = MySQLdb.escape_string( json_attachments )
        sqlcmd = "INSERT INTO `Mail` \
                (`mailid`,`type`,`sendtime`,`status`,`sender`,`sendername`,`roleid`,`tag`,`keepdays`,`title`,`content`,`attachment`) \
                VALUES ('%d','2','%d','1','%d','%s','%d','1','7','%s','%s','%s')" % ( mailid, sendtime, sender, sendername, receiver, title, content, json_attachments )
        handler = self.conn.cursor()
        handler.execute( sqlcmd )
        self.conn.commit()
        handler.close()
