#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys
import csv
import xlrd

from xml.etree import ElementTree as etree
from xml.etree.ElementTree import Element, SubElement, ElementTree

modules = { "role":"玩家", "quest":"任务", "battle":"战斗", "item":"物品", "utils":"工具", "activity":"玩法", "arena":"竞技场", \
        "guild":"公会", "building":"建筑", "mail":"邮件", "operation":"运营活动", "social":"社交", "dungeon":"关卡", "scene":"场景" }

class ProtocolFile() :
    def __init__( self, path ) :
        self._path = path
        tmpp, filename = os.path.split( path )
        self._module, suffix = os.path.splitext( filename )

    def process( self, alldesc ) :
        fp = open( self._path, encoding="utf-8" )
        proto_data = fp.read()
        pattern = re.compile( r"enum Errorcode.*\s+\{([^{}]*)\};" )
        match = pattern.search( proto_data )
        if match :
            lines = match.group(1).split('\n')
            for line in lines :
                self.get_code_string( line, alldesc )

    def get_code_string( self, data, alldesc ) :
        pattern = re.compile( r'=|;|//' )
        a = pattern.split( data )
        if len(a) == 4 :
            code = int( a[1], 16 )
            desc = a[3].strip(' ')
            if code > 0 :
                alldesc[code] = ( self._module, desc )

# CSV文档
class CSVDocument() :
    def __init__( self, path ) :
        self._handler = csv.writer( open(path, "w", newline='') )

    def append2( self, data ) :
        self._handler.writerow( data )

    def writetitle( self ) :
        data = []
        data.append( 'int' )
        data.append( 'string' )
        data.append( 'string' )
        self.append2( data )
        data = []
        data.append( "错误码" )
        data.append( "模块" )
        data.append( "标题" )
        self.append2( data )
        data = []
        data.append( 'code' )
        data.append( 'module' )
        data.append( 'title' )
        data.append( 'rawdesc' )
        self.append2( data )

# 打开错误码xlsm
class ErrorcodeMerge() :
    def __init__( self, xlsfile ) :
        self._errorcodes = {}
        self._xlsfile = xlsfile
        self._workbook = xlrd.open_workbook( xlsfile )

    def format_value( self, cell ) :
        value = cell.value
        if cell.ctype == xlrd.XL_CELL_NUMBER :
            # 如果是整数的话, 直接取整
            if cell.value == int( cell.value ) :
                value = int( cell.value )
        elif cell.ctype == xlrd.XL_CELL_DATE :
            datetuple = xlrd.xldate_as_tuple(value, self._workbook.datemode)
            # time only no date component
            if datetuple[0] == 0 and datetuple[1] == 0 and datetuple[2] == 0:
                value = "%02d:%02d:%02d" % datetuple[3:]
            # date only, no time
            elif datetuple[3] == 0 and datetuple[4] == 0 and datetuple[5] == 0:
                value = "%04d/%02d/%02d" % datetuple[:3]
            else: # full date
                value = "%04d/%02d/%02d %02d:%02d:%02d" % datetuple
        return value

    def parse( self, dataindex ) :
        for sheet in self._workbook.sheets() :
            for row in range( dataindex, sheet.nrows ) :
                errorcode = int( self.format_value( sheet.cell( row, 1 ) ) )
                description = self.format_value( sheet.cell( row, 4 ) )
                self._errorcodes[ errorcode ] = description

    def merge( self, alldesc ) :
        newdesc = {}
        for key,value in alldesc.items() :
            if key in self._errorcodes :
                continue
            newdesc[ key ] = value
        return sorted( newdesc.items(), key=lambda x:x[0] )

def get_alldesc( path ) :
    alldesc = {}
    for filename in os.listdir( path ) :
        a = filename.split( '.' )
        if a[-1] != 'proto':
            continue
        f = ProtocolFile( path + "/" + filename )
        f.process( alldesc )
    return alldesc

def main() :
    protopath = sys.argv[1]
    importfile = sys.argv[2]
    # 获取所有描述
    alldesc = get_alldesc( protopath )
    # 打开文件
    merger = ErrorcodeMerge( importfile )
    merger.parse(3)
    newdesc = merger.merge( alldesc )
    # 重新写入
    doc = CSVDocument( 'new_errorcode.csv' )
    doc.writetitle()
    for desc in newdesc :
        data = []
        if desc[1][0] in modules :
            module = modules[ desc[1][0] ]
        else :
            module = desc[1][0]
        data.append( desc[0] )
        data.append( module )
        data.append( "标题" )
        data.append( desc[1][1] )
        doc.append2( data )

if __name__ == "__main__" :
    main()
