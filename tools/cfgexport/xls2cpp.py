#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys
import xlrd
import json

from xlsdoc import *

# Excel文件导出Xml文件函数
# xlsfile       - Excel文件
# cppfile       - 导出的cpp文件
class XlsExport2Cpp() :
    def __init__( self, xlsfile, cppfile ) :
        self._cppfile = cppfile
        self._xlsfile = xlsfile
        self._xlsdoc = XlsDocument( xlsfile )

    def detailtype( self, rawtype ) :
        extratype = []
        p1 = rawtype.find( '<' )
        p2 = rawtype.find( '>' )
        if p1 != -1 and p2 != -1 :
            extratype = rawtype[p1+1:p2].split(',')
            rawtype = rawtype[0:p1].strip()
        return rawtype.lower(), extratype

    def generate_optionfile( self, sheet, scheme, lines ) :
        # OPTION文件
        # 逐行取出数据[dataindex, nrows]
        metaindex = 0
        fields = []
        allfields = {}
        fieldindex = [
                self._xlsdoc.get_colidx( sheet, "id" ),
                2,
                self._xlsdoc.get_colidx( sheet, "valuetype" ),
                self._xlsdoc.get_colidx( sheet, "value" ) ]
        lines.append( "struct %s {" % sheet.name )
        for row in range( self._xlsdoc._dataidx, sheet.nrows ) :
            field = str(self._xlsdoc.format_value( sheet.cell(row, fieldindex[0]) ))
            comment = str(self._xlsdoc.format_value( sheet.cell(row, fieldindex[1]) ))
            comment = comment.replace( '\r', '' )
            comment = comment.replace( '\n', ',' )
            fieldtype, extratype = self.detailtype( str(self._xlsdoc.format_value(sheet.cell(row, fieldindex[2])) ).strip() )
            allfields[ field ] = ( fieldtype, extratype, comment )
        for k,v in allfields.items() :
            r, lines = self.append( lines, v[0], k, v[1], v[2], metaindex )
            if r :
                metaindex = metaindex + 1
                fields.append( k )
        lines.append( '\n' )
        lines.append( "void name( const std::string & name ) {}" )
        lines.append( "std::string name() const { return \"%s\"; }\n" % sheet.name )
        lines.append( "void clear() {" )
        for field in fields :
            cpptype, value = self._xlsdoc.convert( allfields[field][0] )
            if cpptype == '' :
                lines.append( "foreach_tuple(%s, []( auto & v ) { v = {}; } );" % field )
            elif value == '' :
                lines.append( "%s.clear();" % ( field ) )
            else :
                lines.append( "%s = %s;" % ( field, value ) )
        lines.append( "}\n" )
        lines.append( "bool parse( XmlSheet * node, const ErrorHandler & handler, const std::function<bool ()> & postparser = nullptr ) {" )
        lines.append( "for ( auto child = node->first_node();")
        lines.append( "    child != nullptr; child = child->next_sibling() ) {")
        lines.append( "auto key = child->first_attribute(\"%s\");" % scheme[fieldindex[0]][0] )
        lines.append( "auto value = child->first_attribute(\"%s\");" % scheme[fieldindex[3]][0] )
        lines.append( "if ( key == nullptr || value == nullptr ) {" )
        lines.append( "continue;" )
        lines.append( "}" )
        index = 0
        for field in fields :
            if index == 0 :
                lines.append( "if ( strcmp ( key->value(), \"%s\" ) == 0 ) detail::parse( value->value(), %s );" % ( field, field ) )
            else :
                lines.append( "else if ( strcmp ( key->value(), \"%s\" ) == 0 ) detail::parse( value->value(), %s );" % ( field, field ) )
            index = index + 1
        lines.append( "}" )
        lines.append( "if ( postparser && !postparser() ) {" )
        lines.append( "return false;" )
        lines.append( "}" )
        lines.append( 'return true;' )
        lines.append( "}" )
        lines.append( "}; // %s, FieldsCount: %d\n" % ( sheet.name, metaindex ) )

    def generate_configfile( self, sheet, scheme, lines ) :
        metaindex = 0
        reflection = ""
        lines.append( "struct %s {" % sheet.name )
        reflection += "REFLECTION( %s" % sheet.name
        for s in scheme :
            ftype = ''
            extratype = []
            isbuiltin = self._xlsdoc.isbuiltin(s[3])
            ftype, extratype = self.detailtype(s[3])
            # 一维数组
            decaytype = ftype
            if s[1] == 1 and isbuiltin :
                ftype = 'array'
                extratype.append( decaytype )
            # 二维数组
            if s[1] > 1 :
                ftype = 'array'
                if not isbuiltin :
                    extratype.append( decaytype )
                else :
                    for i in range(s[1]) :
                        extratype.append( str(self._xlsdoc.format_value( sheet.cell(0, s[4]+i) )) )
            r, lines = self.append( lines, ftype, s[0], extratype, s[5], metaindex )
            if r :
                metaindex = metaindex + 1
                reflection += ", "
                reflection += s[0]
        reflection += " );\n"
        lines.append( "}; // %s, FieldsCount: %d" % ( sheet.name, metaindex ) )
        lines.append( reflection )
        return metaindex

    def export( self ) :
        basename = os.path.basename(self._cppfile)
        name,_ = os.path.splitext( basename )
        lines = []
        lines.append( '// Code generated by tool. DO NOT EDIT.' )
        lines.append( "#ifndef __SRC_CONFIGURE_META_%s_HPP__" % name.upper() )
        lines.append( "#define __SRC_CONFIGURE_META_%s_HPP__\n" % name.upper() )
        lines.append( "#include <tuple>" )
        lines.append( "#include <string>" )
        lines.append( "#include <vector>" )
        lines.append( "#include <cstdint>" )
        lines.append( "#include <cstring>\n" )
        lines.append( "#include \"types.h\"" )
        lines.append( "#include \"domain/metasheet.h\"\n" )
        lines.append( "namespace meta {" )
        lines.append( "namespace %s {\n" % name )
        lines.append( "#define %s_CONFIGFILE \"%s.xml\"\n" % (name.upper(), name) )
        # 取出所有的sheets
        is_single = len(self._xlsdoc.get_sheets()) == 1
        for sheet in self._xlsdoc.get_sheets() :
            # 过滤非法的Sheet
            if not self._xlsdoc.filter( sheet ) : continue
            # 合法的Sheet
            if len(self._xlsdoc.get_sheets()) > 1 :
                print("-- Generate Config Code(s) %s::%s ..." % ( basename[:basename.find('.')], sheet.name))
            # 获取scheme
            scheme = self._xlsdoc.get_scheme( sheet )
            # 导出
            if sheet.name.upper().find( "OPTION" ) < 0 :
                # 配置表
                fields = self.generate_configfile( sheet, scheme, lines )
                if fields > 120 :
                    if is_single :
                        print( "\033[31mThrow a Exception : %s::%s has too many Fields(COUNT:%d)\033[0m" % ( name, sheet.name, fields ) )
                    else :
                        print( "-- \033[31mThrow a Exception : %s::%s has too many Fields(COUNT:%d)\033[0m" % ( name, sheet.name, fields ) )
            else :
                # 参数表
                self.generate_optionfile( sheet, scheme, lines )
        lines.append( "} // namespace %s" % name )
        lines.append( "} // namespace meta\n" )
        lines.append( "#endif" )
        self.save2( self._cppfile, lines )

    def append( self, lines, ftype, field, extratype, comment, index = 0 ) :
        subtype = ''
        cpptype = ''
        value = ''
        for t in extratype :
            subcpptype, subvalue = self._xlsdoc.convert( t )
            if subtype != '' :
                subtype += ', '
            subtype += subcpptype
        if len(extratype) > 1 : subtype = "std::tuple<%s>" % subtype
        if subtype != '' :
            if ftype == 'array' :
                cpptype = 'std::vector<%s>' % subtype
            elif ftype == '' :
                cpptype = subtype
        if cpptype == '' :
            cpptype, value = self._xlsdoc.convert( ftype )
            if cpptype == '' :
                return False, lines
        if value == '' :
            lines.append( "%s %s; // Index: %d, %s" % ( cpptype, field, index, comment ) )
        else :
            lines.append( "%s %s = %s; // Index: %d,  %s" % ( cpptype, field, value, index, comment ) )
        return True, lines

    def save2( self, path, lines ) :
        tofile = open( path, 'w' )
        tofile.write( '\n' )
        self.pretty( tofile, lines )
        tofile.close()

    def pretty( self, cppfile, lines, indent = '\n' ) :
        space  = ''
        for line in lines :
            if line[0] == indent :
                cppfile.write( indent )
                continue
            if line[0] == '}' :
                space = space[4:]
            if line.startswith( 'public' ) or line.startswith( 'private' ) or line.startswith( 'protected' ) :
                cppfile.write( line + indent )
            else :
                cppfile.write( space + line + indent )
            if line[-1] == '{' or ( line[0] == '{' and len(line) == 1 ) :
                if line.find( 'namespace' ) < 0 :
                    space = space + '    '

if __name__ == "__main__" :
    language = {}
    op = XlsExport2Cpp( sys.argv[1], sys.argv[2] )
    op.export()
