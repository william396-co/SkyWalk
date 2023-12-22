#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import xlrd
import json

from xlsdoc import *
from xml.etree import ElementTree as etree
from xml.etree.ElementTree import Element, SubElement, ElementTree

# Xml文档
class XmlDocument() :
    def __init__( self, path ) :
        self._path = path
        self._root = Element( 'config' )

    def pretty( self, elem, indent='\n', level=0 ):
        i = str(indent) + level*'\t'
        if len(elem):
            if not elem.text or not elem.text.strip():
                elem.text = i + '\t'
            for e in elem:
                self.pretty(e, indent, level+1)
            if not e.tail or not e.tail.strip():
                e.tail = i
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
        return elem

    def add( self, tag, text = "", attrib = {} ) :
        n = SubElement( self._root, tag, attrib )
        n.text = text
        return n

    def add2( self, node, tag, text, attrib = {} ) :
        n = SubElement( node, tag, attrib )
        n.text = text
        return n

    def save2( self ) :
        self.pretty( self._root )
        tree = ElementTree( self._root )
        tree.write( self._path, encoding='UTF-8' )

    def xmlroot( self ) :
        return self._root

# Excel's Sheet to Xml's Label
class Sheet2Label() :
    def __init__( self, root, label, iscreatelabel=True ) :
        self._xmlroot = root
        self._labelname = label
        if not iscreatelabel :
            self._label = root
        else :
            self._label = SubElement( self._xmlroot, label )

    def addrecord( self, line ) :
        record = SubElement( self._label, self._labelname, line )

# Excel文件导出Xml文件函数
# xlsfile       - Excel文件
# xmlfile       - 导出的Xml文件
class XlsExport2Xml() :
    def __init__( self, xlsfile, xmlfile ) :
        self._xlsfile = xlsfile
        self._xmlfile = xmlfile
        self._xlsdoc = XlsDocument( xlsfile )
        self._xmldoc = None
        if xmlfile != "" :
            self._xmldoc = XmlDocument( xmlfile )

    # version < value 返回真
    def check( self, version, value ) :
        if version == '' or value == '':
            return False
        v2 = value.split( '.' )
        v1 = version.split( '.' )
        for i in range( max( len(v1), len(v2) ) ) :
            iv1 = 0
            iv2 = 0
            if i < len(v1) :
                iv1 = int( v1[i] )
            if i < len(v2) :
                iv2 = int( v2[i] )
            if iv1 != iv2 :
                return iv1 < iv2
        return False

    def is_skip( self, version, sheet, row ) :
        # 忽略空行
        # 忽略注释行(#)
        # 忽略版本小的行
        idvalue = self._xlsdoc.format_value( sheet.cell(row, 1) )
        vervalue = self._xlsdoc.format_value( sheet.cell(row, 0) )
        if idvalue == '' :
            return True
        if vervalue == '#' :
            return True
        return self.check( version, str(vervalue) )

    def do_line( self, language, sheet, scheme, row, lines ) :
        # 定义行数据, 表结构索引
        line = {}
        elem = []; field = []
        schidx = 0; ncols = 0; joinvalue = ''
        schkeys = self._xlsdoc.get_scheme_keys( scheme )
        for col in range( sheet.ncols ) :
            if schidx >= len( scheme ) :
                break
            if self._xlsdoc.is_skip( scheme[schidx][0] ) :
                # 跳过
                schidx = schidx + 1
            else :
                # 从excel中读取这个单元格的内容
                step = scheme[schidx][1]
                number = scheme[schidx][2]
                value = self._xlsdoc.format_value( sheet.cell(row, col) )
                valuetype = scheme[schidx][3]
                if valuetype.upper() == 'LANG' :
                    value = self.get_language( language, value )
                if step == 0 :
                    # 单个格子
                    ncols = -1
                    joinvalue = str(value)
                elif step == 1 :
                    # 一维数组
                    if value != '' :
                        field.append( value )
                else :
                    # 二维数组
                    if ncols % step != step - 1 :
                        elem.append( value )
                    else :
                        # 此处允许二维数组退化为一维数组
                        elem.append( value )
                        for e in elem :
                            if e != '' :
                                field.append( elem )
                                break
                        elem = []
                # 内部列数
                ncols = ncols + 1
                # 重置
                if ncols == number :
                    if step != 0 :
                        joinvalue = json.dumps( field, separators=(',',':') )
                    line[ scheme[schidx][0] ] = joinvalue
                    ncols = 0
                    joinvalue = ''
                    field = []
                    elem = []
                    schidx = schidx + 1
        for pos,ol in enumerate(lines) :
            if ol[schkeys[1]] == line[schkeys[1]] :
                version = line['!']
                if version == '-1' or self.check( ol[schkeys[0]], version ) :
                    lines.remove( ol )
                    lines.insert( pos, line )
                return
        lines.append( line )

    def get_language( self, lang, value ) :
        # 获取语言包的内容
        if value in lang :
            return lang[ value ]
        return value

    def export( self, language, patchfile, version = '', patchversion = '' ) :
        # 取出所有的sheets
        for sheet in self._xlsdoc.get_sheets() :
            # 过滤非法的Sheet
            if not self._xlsdoc.filter( sheet ) : continue
            # 合法的Sheet
            if len(self._xlsdoc.get_sheets()) > 1 :
                basename = os.path.basename(self._xmlfile)
                #print("\tExport %s::%s ..." % ( basename[:basename.find('.')], sheet.name.encode("utf-8")))
                print("\tExport %s::%s ..." % ( basename[:basename.find('.')], sheet.name))
            # 所有行
            lines = []
            # 获取scheme
            scheme = self._xlsdoc.get_scheme( sheet )
            # 逐行取出数据[dataindex, nrows]
            for row in range( self._xlsdoc._dataidx, sheet.nrows ) :
                # 判断是否需要读取当前行
                if not self.is_skip( version, sheet, row ) :
                    self.do_line( language, sheet, scheme, row, lines )
            # 补丁
            if patchfile != "" :
                lines = self.patch( patchfile, sheet.name, patchversion, language, lines )
            # 创建label
            label = Sheet2Label( self._xmldoc.xmlroot(), str( sheet.name ) )
            for line in lines :
                del line['!']
                label.addrecord( line )
            self._xmldoc.save2()

    # 导入语言包
    def importlang( self, language ) :
        lang = {}
        if language == 'cn' :
            return lang
        for sheet in self._xlsdoc.get_sheets() :
            if sheet.name != 'language' : continue
            index = 0
            scheme = self._xlsdoc.get_scheme( sheet )
            for s in scheme :
                if s[0] == 'transText_' + language :
                    break
                index = index + 1
            for row in range( self._xlsdoc._dataidx, sheet.nrows ) :
                value = ''
                key = self._xlsdoc.format_value( sheet.cell( row, 1 ) )
                if index == len(scheme) :
                    valdict = {}
                    for i in range( 0, index ) :
                        pos = scheme[i][0].find('_')
                        if pos < 0 : continue
                        l = scheme[i][0][pos+1:]
                        v = sheet.cell_value( row, i )
                        if v != "" :
                            valdict[ l ] = v
                    value = json.dumps( valdict, separators=(',',':'), ensure_ascii=False )
                else :
                    value = str(sheet.cell_value( row, index ))
                # print( row, str(key), value )
                lang[ str(key) ] = value
        return lang

    # 补丁
    def patch( self, patchfile, sheetname, version, language, lines ) :
        is_cover = False
        newlines = []
        patchlines = []
        partshname = '%'+sheetname
        xlsdoc = XlsDocument( patchfile )
        for sheet in xlsdoc.get_sheets() :
            if sheet.name != sheetname and sheet.name != partshname : continue
            if sheet.nrows < self._xlsdoc._schemeidx or sheet.nrows < self._xlsdoc._dataidx : continue
            #每个sheet第一个単位格必须是$
            if self._xlsdoc.format_value(sheet.cell(0,0)) != '$' : continue
            # 所有行
            # 获取scheme
            scheme = xlsdoc.get_scheme( sheet )
            schkeys = xlsdoc.get_scheme_keys( scheme )
            basename = os.path.basename( patchfile )
            print( "\tPatch %s::%s ..." % ( basename, sheet.name.encode("utf-8")) )
            # 逐行取出数据[dataindex, nrows]
            if sheet.name == sheetname : is_cover = True
            for row in range( self._xlsdoc._dataidx, sheet.nrows ) :
                # 判断是否需要读取当前行
                if not self.is_skip( version, sheet, row ) :
                    self.do_line( language, sheet, scheme, row, patchlines )
        if len( patchlines ) == 0 :
            newlines = lines
        else :
            # 打补丁
            if is_cover :
                newlines = patchlines
            else :
                for ol in lines :
                    line = ol
                    for pol in patchlines :
                        if pol[schkeys[1]] == ol[schkeys[1]] :
                            line = pol
                            if pol[schkeys[0]] == '-1' : line = ''
                            break
                    if line != '' : newlines.append( line )
        return newlines

if __name__ == "__main__" :
    language = {}
    op = XlsExport2Xml( sys.argv[1], sys.argv[2] )
    if len( sys.argv ) == 3 :
        op.export( language, '', '', '' )
    else :
        op.export( language, sys.argv[3], '', '' )
