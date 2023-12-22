#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys
import time
import json

from datahelper import *
import prettytable as pt

def get_sqlfile_lines( sqlfile ) :
    lines = 0
    f = open( sqlfile, 'r' )
    for l in f.readlines() :
        xx = re.findall('\((.*?)\)', l)
        lines = lines + len(xx) - 1
    return lines
    #return sum(1 for _ in open(sqlfile) )

def statistics( dbs ) :
    alltables = []
    filetables = []
    for d in dbs :
        host,port,username,password,charset,db = get_db_info( d[0] )
        tables = get_table_lines( host, port, username, password, charset, d[1] )
        filetables.append( (d[1], tables) )
        for key,value in tables.items() :
            if key not in alltables : alltables.append(key)
    alltables.sort()
    filetables.sort()
    tb = pt.PrettyTable()
    fields = []
    fields.append( "Table" )
    for db in filetables :
        fields.append( db[0] )
    for table in alltables :
        d = []
        d.append( table )
        for db,counts in filetables :
            count = 0
            if counts.has_key( table ) : count = counts[table]
            d.append( count )
        tb.add_row( d )
    tb.field_names = fields
    print(tb)

# 主函数
if __name__ == "__main__" :
    if len( sys.argv ) < 3 :
        print( "statistics.py <taskid> <masterid> [slaveid1 ... slaveidN] ." )
        sys.exit()
    dbs = []
    taskid = int( sys.argv[1] )
    master_zoneid = int( sys.argv[2] )
    workpath = get_workpath( taskid )
    # 创建临时文件夹
    host,port,username,password,charset,db = get_db_info( master_zoneid )
    merged_dbname = get_merged_dbname( db, taskid )
    export_from_db( workpath, master_zoneid, master_zoneid, merged_dbname )
    dbs.append( (master_zoneid, db) )
    dbs.append( (master_zoneid, merged_dbname) )
    for i in range( 3, len(sys.argv) ) :
        slaveid = int( sys.argv[i] )
        host,port,username,password,charset,db = get_db_info( slaveid )
        dbs.append( (slaveid, db) )
    statistics( dbs )
