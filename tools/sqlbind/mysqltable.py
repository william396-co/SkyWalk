#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys
import pymysql as MySQLdb

class MySQLTable() :
    def __init__( self, table, sharding ) :
        self._indexs = []
        self._prikeys = []
        self._allkeys = []
        self._fields = []
        self._aifield = ''
        self._table = table
        self._firstindexs = []
        self._sharding = sharding

    def tablename( self ) :
        return self._table

    # 所有索引(主键+第一索引+常规索引)
    def indexs( self ) :
        return self._indexs

    # 主键
    def prikeys( self ) :
        return self._prikeys

    # 第一索引
    def firstindexs( self ) :
        return self._firstindexs

    # 是否是主键+第一索引
    def iskey( self, field ) :
        for k in self._allkeys :
            if k == field:
                return True
        return False

    # 主键+第一索引
    def allkeys( self ) :
        return self._allkeys

    def fields( self ) :
        return self._fields

    def aifield( self ) :
        return self._aifield

    def sharding( self ) :
        return self._sharding

    def loads( self, db ) :
        self.load_desc( db )
        self.load_showindex( db )
        return self._firstindexs == self._prikeys

    def load_desc( self, db ) :
        index = 0
        if self._sharding == 0 :
            db.execute( 'desc %s' % self._table )
        else :
            db.execute( 'desc %s_0' % ( self._table ) )
        results = db.fetchall()
        for result in results :
            field = result[0]
            cpptype,length = self.cpptype( result[1] )
            self._fields.append( (field, cpptype, index, result[4], length) )
            index = index+1
            if result[5] == 'auto_increment' :
                self._aifield = field

    def load_showindex( self, db ) :
        if self._sharding == 0 :
            db.execute( 'show index from %s' % self._table )
        else :
            db.execute( 'show index from %s_0' % self._table )
        firstindex = ""
        results = db.fetchall()
        for result in results :
            self.add_index( result[4] )
            if result[2] == 'PRIMARY' :
                self.add_key( result[4] )
                self._prikeys.append( result[4] )
            else :
                if firstindex == "" : firstindex = result[2]
                if firstindex == result[2] :
                    self.add_key( result[4] )
                    self._firstindexs.append( result[4] )

    def get_field( self, key ) :
        for field in self._fields :
            if field[0] == key:
                return field
        return ( key, 'std::string', 0 )

    def add_key( self, key ) :
        for k in self._allkeys :
            if k == key : return
        self._allkeys.append( key )

    def add_index( self, index ) :
        for i in self._indexs :
            if i == index : return
        self._indexs.append( index )

    def generate_getter_setter( self, lines, field ) :
        lines.append( '// Field: %s, Index: %d' % (field[0], field[2]) )
        if field[1] == 'std::string' :
            lines.append( 'size_t get_db_%s_length() const { return %d; }' % ( field[0], field[4] ) )
            lines.append( 'const std::string & get_db_%s() const { return m_db_%s; }' % ( field[0], field[0] ) )
            lines.append( 'void set_db_%s( const std::string & value ) { if ( value.size() <= %d ) { m_db_%s = value; m_dirty[%d] = 1; } }\n' % ( field[0], field[4], field[0], field[2] ) )
        else :
            lines.append( '%s get_db_%s() const { return m_db_%s; }' % ( field[1], field[0], field[0] ) )
            lines.append( 'void set_db_%s( %s value ) { m_db_%s = value; m_dirty[%d] = 1; }\n' % ( field[0], field[1], field[0], field[2] ) )

    def generate_js_getter_setter( self, lines, field ) :
        lines.append( '// Field: %s, Index: %d' % (field[0], field[2]) )
        if self.iskey( field[0] ) :
            lines.append( 'this.get_db_%s = function() { return m_db_%s; }\n' % ( field[0], field[0] ) )
        else :
            lines.append( 'this.get_db_%s = function() { return m_db_%s; }' % ( field[0], field[0] ) )
            lines.append( 'this.set_db_%s = function( value ) { m_db_%s = value; m_dirty[%d] = 1; }\n' % ( field[0], field[0], field[2] ) )

    def generate_go_getter_setter( self, lines, field ) :
        lines.append( '// Field: %s, Index: %d' % (field[0], field[2]) )
        lines.append( 'func (m *%s) Get%s() %s {' % ( self._table, field[0].capitalize(), self.gotype( field[1] ) ) )
        lines.append( '    return m.%s' % field[0].capitalize() )
        lines.append( '}' )
        lines.append( '\n' )
        lines.append( 'func (m *%s) Set%s(%s %s) {' % ( self._table, field[0].capitalize(), field[0].capitalize(), self.gotype( field[1] ) ) )
        lines.append( '    m.dirty[%d] = 1' % field[2] )
        lines.append( '    m.%s = %s' % ( field[0].capitalize(), field[0].capitalize() ) )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_getter_setter_index( self, lines, field ) :
        if not field[0] in self._indexs:
            self.generate_getter_setter( lines, field )
        else :
            lines.append( '// Field: %s, Index: %d' % (field[0], field[2]) )
            if field[1] == 'std::string' :
                lines.append( 'const std::string & get_db_%s() const { return m_db_%s; }' % ( field[0], field[0] ) )
                lines.append( 'void set_db_%s( const std::string & value, bool index ) { m_db_%s = value; m_dirty[%d] = index ? 2 : 1; }\n' % ( field[0], field[0], field[2] ) )
            else :
                lines.append( '%s get_db_%s() const { return m_db_%s; }' % ( field[1], field[0], field[0] ) )
                lines.append( 'void set_db_%s( %s value, bool index ) { m_db_%s = value; m_dirty[%d] = index ? 2 : 1; }\n' % ( field[0], field[1], field[0], field[2] ) )

    def get_keys_values( self, tag = 'db' ) :
        keys = ''
        values = ''
        valueslen = ''
        for i in range( len(self._prikeys) ) :
            field = self.get_field( self._prikeys[i] )
            if i != 0 :
                keys += ' AND '
                if field[1] == 'std::string' :
                    valueslen += ' + '
                else :
                    values += ', '
                    valueslen += ' + '
            if field[1] == 'std::string' :
                valueslen += '1';
                keys += '`%s`=\'?\'' % field[0]
            else :
                valueslen += '32';
                values += 'm_%s_%s' % (tag, field[0])
                keys += '`%s`=%s' % ( field[0], self.placeholder(field[1]) )
        return keys, values, valueslen

    def generate_shardcodes( self, lines ) :
        if len(self._firstindexs) == 1 :
            idxtype = self.get_field( self._firstindexs[0] )[1]
            if idxtype != 'std::string' :
                lines.append( "uint32_t shard = m_db_%s %s %d;" % ( self._firstindexs[0], "%", self._sharding ) )
                return
        lines.append( "std::string idxstring;")
        lines.append( "indexstring( eCodec_All, idxstring );")
        lines.append( "uint32_t shard = utils::HashFunction::djb(idxstring) %s %d;" % ( "%", self._sharding ) )

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
            if line[0] == '{' and len(line) == 1 :
                space = space + '    '

    # MYSQL to CPP
    def cpptype( self, dbtype ) :
        length = 0
        cpptype = ''
        mytype = re.findall(r'[^()]+', dbtype )
        if len(mytype) == 2 : length = int(mytype[1])
        if mytype[0] == 'tinyint' : cpptype = 'int8_t'
        elif mytype[0] == 'smallint' : cpptype = 'int16_t'
        elif mytype[0] == 'int' : cpptype = 'int32_t'
        elif mytype[0] == 'bigint' : cpptype = 'int64_t'
        elif mytype[0] == 'tinyint unsigned' : cpptype = 'uint8_t'
        elif mytype[0] == 'smallint unsigned' : cpptype = 'uint16_t'
        elif mytype[0] == 'int unsigned' : cpptype = 'uint32_t'
        elif mytype[0] == 'bigint unsigned' : cpptype = 'uint64_t'
        elif mytype[0] == 'timestamp' : cpptype = 'int64_t'
        elif mytype[0] == 'tinytext' :
            cpptype = 'std::string'
            if length == 0 : length = 255
        elif mytype[0] == 'text' :
            cpptype = 'std::string'
            if length == 0 : length = 65535
        elif mytype[0] == 'mediumtext' :
            cpptype = 'std::string'
            if length == 0 : length = 16777215
        elif mytype[0] == 'longtext' :
            cpptype = 'std::string'
            if length == 0 : length = 4294967296
        else :
            cpptype = 'std::string'
        return cpptype, length

    # place holder
    def placeholder( self, cpptype ) :
        holder = ''
        if cpptype == 'int8_t' : holder = '%d'
        elif cpptype == 'uint8_t' : holder = '%u'
        elif cpptype == 'int16_t' : holder = '%d'
        elif cpptype == 'uint16_t' : holder = '%u'
        elif cpptype == 'int32_t' : holder = '%d'
        elif cpptype == 'uint32_t' : holder = '%u'
        elif cpptype == 'int64_t' : holder = '%" PRId64 "'
        elif cpptype == 'uint64_t' : holder = '%" PRIu64 "'
        else : holder = '%s'
        return holder

    def gotype( self, cpptype ) :
        gotype = ''
        if cpptype == 'int8_t' :
            gotype = 'int8'
        elif cpptype == 'uint8_t':
            gotype = 'uint8'
        elif cpptype == 'int16_t' :
            gotype = 'int16'
        elif cpptype == 'uint16_t' :
            gotype = 'uint16'
        elif cpptype == 'int32_t' :
            gotype = 'int32'
        elif cpptype == 'uint32_t' :
            gotype = 'uint32'
        elif cpptype == 'int64_t':
            gotype = 'int64'
        elif cpptype == 'uint64_t':
            gotype = 'uint64'
        elif cpptype == 'std::string' :
            gotype = 'string'
        return gotype
