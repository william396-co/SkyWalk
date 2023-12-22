#!/usr/bin/env python
# -*- coding:utf-8 -*-

from mysqltable import *

class SqlGenerator() :
    def __init__( self, table ) :
        self._table = table
        self._schemeversion = ''

    def keyparams( self ) :
        expression = ''
        allkeys = self._table.allkeys()
        for i in range( len(allkeys) ) :
            field = self._table.get_field( allkeys[i] )
            if i != 0 :
                expression += ', '
            if field[1] == 'std::string' :
                expression += 'const std::string & %s' % field[0]
            else :
                expression += '%s %s' % ( field[1], field[0] )
        return expression

    def keyparams_lua( self ) :
        expression = ''
        allkeys = self._table.allkeys()
        for i in range( len(allkeys) ) :
            field = self._table.get_field( allkeys[i] )
            if i != 0 :
                expression += ', '
            if field[1] == 'std::string' :
                expression += 'const std::string &'
            else :
                expression += '%s' % ( field[1] )
        return expression

    def keyparams_js( self ) :
        expression = ''
        allkeys = self._table.allkeys()
        for i in range( len(allkeys) ) :
            field = self._table.get_field( allkeys[i] )
            if i != 0 :
                expression += ', '
            expression += '%s' % ( field[0] )
        return expression

    def generate( self, language, dstpath, version ) :
        self._schemeversion = version
        if language == '--cpp_out' :
            self.generate_hpp( dstpath )
            self.generate_cpp( dstpath )
        elif language == '--js_out':
            self.generate_js( dstpath )
        elif language == '--go_out' :
            dstpath = '%s/%s' % ( dstpath, 'scheme' );
            self.generate_go( dstpath )

    def generate_hpp( self, dstpath ) :
        lines = []
        lines.append( '#ifndef __SQLBIND_GENERATE_%s_H__' % self._table.tablename().upper() )
        lines.append( '#define __SQLBIND_GENERATE_%s_H__\n' % self._table.tablename().upper() )
        lines.append( '#include <strings.h>' )
        lines.append( '#include \"sqlbind.h\"\n' )
        lines.append( 'namespace data' )
        lines.append( '{\n' )
        lines.append( 'class %s_Data : public ISQLData' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'public :' )
        lines.append( '// Tablename' )
        lines.append( 'static const std::string TABLENAME;' )
        lines.append( "// VARCHAR Length Limit" )
        for field in self._table.fields() :
            if field[1] == 'std::string' :
                lines.append( 'static const size_t MAX_DB_%s_LENGTH;' % field[0].upper() )
        lines.append( "// Lua Interface" )
        lines.append( 'static void registering( kaguya::State * state );\n' )
        lines.append( '%s_Data();' % self._table.tablename() )
        lines.append( '%s_Data( %s );' % ( self._table.tablename(), self.keyparams() ) )
        lines.append( 'virtual ~%s_Data();\n' % self._table.tablename() )
        lines.append( '// Check Object is dirty' )
        lines.append( 'virtual bool isDirty() const final;' )
        lines.append( '// Check Keys is dirty' )
        lines.append( 'virtual bool isKeydirty() const final;' )
        lines.append( '// TableName' )
        lines.append( 'virtual const Tablename & tablename() const final { return TABLENAME; }' )
        lines.append( '// KeyString' )
        lines.append( 'virtual void keystring( std::string & k ) const final;' )
        lines.append( '// IndexString' )
        lines.append( 'virtual void indexstring( uint8_t op, std::string & k ) const final;\n' )
        lines.append( '// Parse' )
        lines.append( 'virtual bool parse( const Slices & result ) final;' )
        lines.append( '// AutoIncrease' )
        lines.append( '// source : eDataSource_Database, eDataSource_Dataserver' )
        lines.append( 'virtual void autoincrease( uint8_t source, const Slice & result ) final;\n' )
        lines.append( '// Splicing sql statement (query, insert, update, remove)' )
        lines.append( 'virtual bool query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all = true ) final;' )
        lines.append( 'virtual bool insert( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;' )
        lines.append( 'virtual bool update( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;' )
        lines.append( 'virtual bool remove( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;' )
        lines.append( 'virtual bool replace( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;\n' )
        lines.append( '// Serialize')
        lines.append( '// op : eCodec_Dirty, eCodec_All, eCodec_Query' )
        lines.append( 'virtual Slice encode( uint8_t op ) final;' )
        lines.append( 'virtual bool decode( const Slice & result, uint8_t op ) final;\n' )
        lines.append( '// Clean Dirty Fields' );
        lines.append( 'virtual void clean() final;' )
        lines.append( '// Clone' );
        lines.append( 'virtual ISQLData * clone( bool isfull ) const final;' )
        lines.append( '// Merge' )
        lines.append( 'virtual void merge( const ISQLData * data ) final;' )
        lines.append( '// Match' );
        lines.append( 'virtual bool match( const ISQLData * data ) const final;' )
        lines.append( '// Lua Invoke' )
        lines.append( 'virtual bool invoke( kaguya::State * state, const std::string & script ) final;\n' )
        lines.append( 'public :' )
        lines.append( "// CopyFrom" )
        lines.append( "void copyfrom( const %s_Data & data );\n" % self._table.tablename() )
        for field in self._table.fields() :
            self._table.generate_getter_setter( lines, field )
        lines.append( 'private :' )
        lines.append( 'enum { NFIELDS = %d };' % len(self._table.fields()) )
        lines.append( 'int8_t m_dirty[ NFIELDS ];\n' )
        lines.append( 'private :' )
        for field in self._table.fields() :
            lines.append( '%s m_db_%s;' % ( field[1], field[0] ) )
        lines.append( '};\n' )
        lines.append( '}\n' )
        lines.append( '#endif' )
        self._table.save2( '%s/%s.hpp' % ( dstpath, self._table.tablename() ), lines )

    def generate_cpp( self, dstpath ) :
        lines = []
        lines.append( '#include <cstdio>' )
        lines.append( '#include <cassert>' )
        lines.append( '#include <cinttypes>' )
        lines.append( '#include <cstdlib>\n' )
        lines.append( '#include \"lua/kaguya.hpp\"' )
        lines.append( '#include \"utils/integer.h\"' )
        lines.append( '#include \"utils/streambuf.h\"\n' )
        lines.append( '#include \"%s.hpp\"\n' % self._table.tablename() )
        lines.append( 'namespace data' )
        lines.append( '{\n' )
        lines.append( '// Tablename' )
        lines.append( 'const std::string %s_Data::TABLENAME = \"%s\";\n' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( "// VARCHAR Length Limit" )
        for field in self._table.fields() :
            if field[1] == 'std::string' :
                lines.append( 'const size_t %s_Data::MAX_DB_%s_LENGTH = %d;' % ( self._table.tablename(), field[0].upper(), field[4] ) )
        lines.append( '\n' )
        lines.append( '%s_Data::%s_Data()' % ( self._table.tablename(), self._table.tablename() )  )
        lines.append( '{' )
        lines.append( 'clean();' )
        lines.append( '// Fields' )
        for field in self._table.fields() :
            if field[3] is not None :
                if field[3] == '' :
                    if field[1] == 'std::string' :
                        lines.append( 'm_db_%s = \"\";' % (field[0]) )
                    else :
                        lines.append( 'm_db_%s = 0;' % (field[0]) )
                else :
                    lines.append( 'm_db_%s = %s;' % (field[0], field[3]) )
            elif field[1] != 'std::string' :
                lines.append( 'm_db_%s = 0;' % field[0] )
        lines.append( '}\n' );
        lines.append( '%s_Data::%s_Data( %s )' % ( self._table.tablename(), self._table.tablename(), self.keyparams() )  )
        lines.append( '{' );
        lines.append( 'clean();' )
        lines.append( '// Fields' )
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                lines.append( 'm_db_%s = %s;' % ( field[0], field[0] ) )
            elif field[3] is not None :
                if field[3] == '' :
                    if field[1] == 'std::string' :
                        lines.append( 'm_db_%s = \"\";' % (field[0]) )
                    else :
                        lines.append( 'm_db_%s = 0;' % (field[0]) )
                else :
                    lines.append( 'm_db_%s = %s;' % (field[0], field[3]) )
            elif field[1] != 'std::string' :
                lines.append( 'm_db_%s = 0;' % field[0] )
        lines.append( '}\n' );
        lines.append( '%s_Data::~%s_Data()' % ( self._table.tablename(), self._table.tablename() )  )
        lines.append( '{' )
        for field in self._table.fields() :
            if field[1] != 'std::string' :
                lines.append( 'm_db_%s = 0;' % field[0] )
            else :
                lines.append( 'm_db_%s.clear();' % field[0] )
        lines.append( '}\n' )
        self.generate_clean( lines )
        self.generate_clone( lines )
        self.generate_dirty( lines )
        self.generate_keydirty( lines )
        self.generate_key( lines )
        self.generate_indexstring( lines )
        self.generate_autoincrease( lines )
        self.generate_query( lines )
        self.generate_insert( lines )
        self.generate_update( lines )
        self.generate_remove( lines )
        self.generate_replace( lines )
        self.generate_encode( lines )
        self.generate_decode( lines )
        self.generate_merge( lines )
        self.generate_match( lines )
        self.generate_parse( lines )
        self.generate_luainterface( lines )
        self.generate_copyfrom( lines )
        lines.append( '}' )
        self._table.save2( '%s/%s.cpp' % ( dstpath, self._table.tablename() ), lines )

    def generate_js( self, dstpath ) :
        lines = []
        lines.append( 'var StreamBuf = require(\'./StreamBuf\');\n' )
        lines.append( 'var %s_Data = function( %s )' % ( self._table.tablename(), self.keyparams_js() ) )
        lines.append( '{' )
        # method
        lines.append( '// TableName' )
        lines.append( 'this.tablename = function() { return m_tablename; }\n' )
        lines.append( '// Clean Dirty Fields' );
        self.generate_js_clean( lines )
        lines.append( '// Check Object is dirty' )
        self.generate_js_dirty( lines )
        self.generate_js_key( lines )
        self.generate_js_encode( lines )
        self.generate_js_decode( lines )

        for field in self._table.fields() :
            self._table.generate_js_getter_setter( lines, field )
        # variable
        lines.append( '// Fields' )
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                lines.append( 'var m_db_%s = %s;' % ( field[0], field[0] ) )
            elif field[3] is not None :
                if field[3] == '' :
                    if field[1] == 'std::string' :
                        lines.append( 'var m_db_%s = \"\";' % (field[0]) )
                    else :
                        lines.append( 'var m_db_%s = 0;' % (field[0]) )
                else :
                    lines.append( 'var m_db_%s = %s;' % (field[0], field[3]) )
            elif field[1] != 'std::string' :
                lines.append( 'var m_db_%s = 0;' % field[0] )
            else :
                lines.append( 'var m_db_%s;' % ( field[0] ) )

        lines.append( '\n' )
        lines.append( 'var NFIELDS = %d;' % len(self._table.fields()) )
        lines.append( 'var m_tablename = \"%s\";' % self._table.tablename() )
        lines.append( 'var m_dirty = new Int8Array(NFIELDS);' )
        lines.append( 'var m_method = DBMethod::None;' )
        lines.append( '}\n' )
        lines.append( '%s_Data.TABLENAME = \"%s\";\n' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( 'module.exports = exports = %s_Data\n' % self._table.tablename() )
        self._table.save2( '%s/%s.js' % ( dstpath, self._table.tablename() ), lines )

    def generate_clean( self, lines ) :
        lines.append( 'void %s_Data::clean()' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'm_method = DBMethod::None;' )
        lines.append( 'bzero( &m_dirty, sizeof(m_dirty) );' )
        lines.append( '}\n' )

    def generate_clone( self, lines ) :
        lines.append( "ISQLData * %s_Data::clone( bool isfull ) const" % self._table.tablename() )
        lines.append( '{' )
        lines.append( '%s_Data * data = new %s_Data();\n' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( 'if ( data != nullptr && isfull )' )
        lines.append( '{' )
        lines.append( 'data->m_method = m_method;' )
        lines.append( 'memcpy( data->m_dirty, m_dirty, NFIELDS );\n' )
        for field in self._table.fields() :
            lines.append( 'data->m_db_%s = m_db_%s;' % (field[0], field[0]) )
        lines.append( '}\n' )
        lines.append( 'return data;' )
        lines.append( '}\n' )

    def generate_dirty( self, lines ) :
        lines.append( "bool %s_Data::isDirty() const" % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] != 0 ) return true;' )
        lines.append( '}\n' )
        lines.append( 'return false;' )
        lines.append( '}\n' )

    def generate_keydirty( self, lines ) :
        expression = ''
        allkeys = self._table.allkeys()
        lines.append( 'bool %s_Data::isKeydirty() const' % self._table.tablename() )
        lines.append( '{' )
        for i in range( len(allkeys) ) :
            field = self._table.get_field( allkeys[i] )
            if i != 0 :
                expression += ' || '
            expression += 'm_dirty[%d] == 1' % ( field[2] )
        lines.append( 'return %s;' % expression )
        lines.append( '}\n' )

    def generate_key( self, lines ) :
        lines.append( 'void %s_Data::keystring( std::string & k ) const' % self._table.tablename() )
        lines.append( '{' )
        for i in range( len(self._table.prikeys()) ) :
            field = self._table.get_field( self._table.prikeys()[i] )
            if i != 0 :
                lines.append( 'k += \"#\";' )
            if field[1] == 'std::string' :
                if i == 0 :
                    lines.append( 'k = m_db_%s;' % field[0] )
                else :
                    lines.append( 'k += m_db_%s;' % field[0] )
            else :
                if i == 0 :
                    lines.append( 'k = utils::Integer::toString( m_db_%s );' % field[0] )
                else :
                    lines.append( 'k += utils::Integer::toString( m_db_%s );' % field[0] )
        lines.append( '}\n' )

    def generate_indexstring( self, lines ) :
        lines.append( 'void %s_Data::indexstring( uint8_t op, std::string & k ) const' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'if ( likely(op == eCodec_All) )' )
        lines.append( '{' )
        for i in range( len(self._table.firstindexs()) ) :
            field = self._table.get_field( self._table.firstindexs()[i] )
            if i != 0 :
                lines.append( 'k += \"#\";' )
            if field[1] == 'std::string' :
                if i == 0 :
                    lines.append( 'k = m_db_%s;' % field[0] )
                else :
                    lines.append( 'k += m_db_%s;' % field[0] )
            else :
                if i == 0 :
                    lines.append( 'k = utils::Integer::toString( m_db_%s );' % field[0] )
                else :
                    lines.append( 'k += utils::Integer::toString( m_db_%s );' % field[0] )
        lines.append( '}' )
        lines.append( 'else' )
        lines.append( '{' )
        for key in self._table.firstindexs() :
            field = self._table.get_field( key )
            lines.append( 'if ( m_dirty[%d] == 1 )' % field[2] )
            lines.append( '{')
            lines.append( 'if ( !k.empty() ) k += \"#\";' )
            if field[1] == 'std::string' :
                lines.append( 'k += m_db_%s;' % field[0] )
            else :
                lines.append( 'k += utils::Integer::toString( m_db_%s );' % field[0] )
            lines.append( '}' )
        lines.append( '}' )
        lines.append( '}\n' )

    def generate_autoincrease( self, lines ) :
        lines.append( 'void %s_Data::autoincrease( uint8_t source, const Slice & value )' % self._table.tablename() )
        lines.append( '{' )
        if self._table.aifield() != '' :
            field = self._table.get_field( self._table.aifield() )
            lines.append( "if ( source == eDataSource_Database )" )
            lines.append( "{" )
            lines.append( 'm_dirty[%d] = 1;' % field[2] )
            if field[1] == 'int64_t' :
                lines.append( 'm_db_%s = (%s)std::atoll( result.c_str() );' % (field[0], field[1]) );
            elif field[1] == 'uint64_t' :
                lines.append( 'm_db_%s = (%s)std::atoll( result.c_str() );' % (field[0], field[1]) );
            elif field[1] == 'std::string' :
                lines.append( 'm_db_%s = result;' % field[0] );
            else :
                lines.append( 'm_db_%s = (%s)std::atoi( result.c_str() );' % (field[0], field[1]) );
            lines.append( "}" )
            lines.append( "else if ( source == eDataSource_Dataserver )" )
            lines.append( "{" )
            lines.append( 'StreamBuf unpack( value.data(), value.size() );' )
            lines.append( 'unpack.decode( m_db_%s );' % self._table.aifield()  )
            lines.append( "}" )
        lines.append( '}\n' )

    def generate_query( self, lines ) :
        lines.append( 'bool %s_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'std::string fields;\n' )
        strlimit = ''
        allfields = ''
        if self._table.firstindexs() == self._table.prikeys() :
            strlimit = " LIMIT 1"
        for i in range( len(self._table.fields()) ) :
            if i != 0 :
                allfields += ','
            allfields += '`'
            allfields += self._table.fields()[i][0]
            allfields += '`'
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        if self._table.firstindexs() == self._table.allkeys() :
            lines.append( 'if ( !fields.empty() )' )
            lines.append( '{' )
            lines.append( 'fields += \" AND \";' )
            lines.append( '}\n' )
            lines.append( 'switch ( i )' )
            lines.append( '{' )
            for k in self._table.firstindexs() :
                field = self._table.get_field( k )
                lines.append( 'case %s :' % field[2] )
                lines.append( '    fields += \"`%s`=\";' % field[0] )
                if field[1] == 'std::string' :
                    lines.append( '    fields += \"\'?\'\";' )
                    lines.append( '    escapevalues.push_back( m_db_%s );' % field[0] )
                else :
                    lines.append( '    fields += utils::Integer::toString( m_db_%s );' % field[0] )
                lines.append( '    break;' )
            lines.append( '}' )
        else :
            indexfilter = ''
            for k in self._table.firstindexs() :
                field = self._table.get_field( k )
                if indexfilter != '' :
                    indexfilter += ' || '
                indexfilter += 'i == %d' % field[2]
            allkeysfilter = ''
            for k in self._table.allkeys() :
                field = self._table.get_field( k )
                if allkeysfilter != '' :
                    allkeysfilter += ' || '
                allkeysfilter += 'i == %d' % field[2]
            lines.append( 'if ( all )' )
            lines.append( '{' )
            if indexfilter == '' :
                lines.append( 'if ( !fields.empty() )' )
            else :
                lines.append( 'if ( !fields.empty() && ( %s ) )' % indexfilter )
            lines.append( '{' )
            lines.append( 'fields += \" AND \";' )
            lines.append( '}\n' )
            lines.append( 'switch ( i )' )
            lines.append( '{' )
            for k in self._table.firstindexs() :
                field = self._table.get_field( k )
                lines.append( 'case %s :' % field[2] )
                lines.append( '    fields += \"`%s`=\";' % field[0] )
                if field[1] == 'std::string' :
                    lines.append( '    fields += \"\'?\'\";' )
                    lines.append( '    escapevalues.push_back( m_db_%s );' % field[0] )
                else :
                    lines.append( '    fields += utils::Integer::toString( m_db_%s );' % field[0] )
                lines.append( '    break;' )
            lines.append( '}' )
            lines.append( '}' )
            lines.append( 'else' )
            lines.append( '{' )
            lines.append( 'if ( !fields.empty() && ( %s ) )' % allkeysfilter )
            lines.append( '{' )
            lines.append( 'fields += \" AND \";' )
            lines.append( '}\n' )
            lines.append( 'switch ( i )' )
            lines.append( '{' )
            for k in self._table.allkeys() :
                field = self._table.get_field( k )
                lines.append( 'case %s :' % field[2] )
                lines.append( '    fields += \"`%s`=\";' % field[0] )
                if field[1] == 'std::string' :
                    lines.append( '    fields += \"\'?\'\";' )
                    lines.append( '    escapevalues.push_back( m_db_%s );' % field[0] )
                else :
                    lines.append( '    fields += utils::Integer::toString( m_db_%s );' % field[0] )
                lines.append( '    break;' )
            lines.append( '}' )
            lines.append( '}' )
        lines.append( '}\n' )
        lines.append( 'if ( fields.empty() )' )
        lines.append( '{' )
        if self._table.sharding() == 0 :
            lines.append( 'sqlcmd = \"SELECT %s FROM `%s`\";' % (allfields, self._table.tablename()) )
        else :
            for i in range( self._table.sharding() ) :
                if i == 0 :
                    lines.append( 'sqlcmd = \"SELECT %s FROM `%s_%d`\";' % (allfields, self._table.tablename(), i) )
                else :
                    lines.append( 'sqlcmd += \" union all \";' )
                    lines.append( 'sqlcmd += \"SELECT %s FROM `%s_%d`\";' % (allfields, self._table.tablename(), i) )
        lines.append( '}' )
        lines.append( 'else' )
        lines.append( '{' )
        lines.append( 'uint32_t len = 32 + %d + %d + %d + fields.size();' % (len(allfields), len(strlimit), len(self._table.tablename())) )
        lines.append( 'sqlcmd.resize( len );' )
        if self._table.sharding() == 0 :
            lines.append( 'len = std::snprintf(')
            lines.append( '    (char *)sqlcmd.data(), len-1,')
            lines.append( '    \"SELECT %s FROM `%s` WHERE %s%s\", fields.c_str() );' % ( allfields, self._table.tablename(), '%s', strlimit ) )
        else :
            self._table.generate_shardcodes( lines )
            lines.append( 'len = std::snprintf(')
            lines.append( '    (char *)sqlcmd.data(), len-1,')
            lines.append( '    \"SELECT %s FROM `%s_%s` WHERE %s%s\", shard, fields.c_str() );' % ( allfields, self._table.tablename(), '%u', '%s', strlimit ) )
        lines.append( 'sqlcmd.resize( len );' )
        lines.append( '}\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_insert( self, lines ) :
        keystring = ''
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                if keystring != '' :
                    keystring += ' && '
                keystring += 'i != %d' % field[2]
        lines.append( 'bool %s_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'std::string fields, values;\n' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 && %s )' % keystring )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'if ( !fields.empty() )' )
        lines.append( '{' )
        lines.append( 'fields += \", \";' )
        lines.append( '}' )
        lines.append( 'if ( !values.empty() )' )
        lines.append( '{' )
        lines.append( 'values += \", \";' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    fields += \"`%s`\";' % field[0] );
            if field[1] == 'std::string' :
                lines.append( '    values += \"\'?\'\";' )
                lines.append( '    escapevalues.push_back( m_db_%s );' % (field[0]) )
            else :
                lines.append( '    values += utils::Integer::toString( m_db_%s );' % field[0] )
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( 'uint32_t len = 32 + %d + fields.size() + values.size();\n' % len(self._table.tablename()) );
        lines.append( 'sqlcmd.resize( len );' )
        if self._table.sharding() == 0 :
            lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
            lines.append( '    \"INSERT INTO `%s` (%s) VALUES (%s)\", fields.c_str(), values.c_str() );' % ( self._table.tablename(), '%s', '%s' ) )
        else :
            self._table.generate_shardcodes( lines )
            lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
            lines.append( '    \"INSERT INTO `%s_%s` (%s) VALUES (%s)\", shard, fields.c_str(), values.c_str() );' % ( self._table.tablename(), '%u', '%s', '%s' ) )
        lines.append( 'sqlcmd.resize(len);\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_update( self, lines ) :
        keys, values, valueslen = self._table.get_keys_values()
        lines.append( 'bool %s_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'std::string dirty;\n' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'if ( !dirty.empty() )' )
        lines.append( '{' )
        lines.append( 'dirty += \", \";' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            if field[1] != 'std::string' :
                lines.append( '    dirty += \"`%s`=\";' % field[0] )
                lines.append( '    dirty += utils::Integer::toString( m_db_%s );' % field[0] )
            else :
                lines.append( '    dirty += \"`%s`=\";' % field[0] )
                lines.append( '    dirty += \"\'?\'\";' )
                lines.append( '    escapevalues.push_back( m_db_%s );' % field[0] )
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        if len(valueslen) == 0 :
            lines.append( 'int32_t len = 32 + %d + dirty.size() + %d;\n' % ( len(self._table.tablename()), len(keys) ) )
        else :
            lines.append( 'int32_t len = 32 + %d + dirty.size() + %d + %s;\n' % ( len(self._table.tablename()), len(keys), valueslen ) )
        lines.append( 'sqlcmd.resize( len );')
        if len(keys) == 0 :
            if self._table.sharding() == 0 :
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                lines.append( '    \"UPDATE `%s` SET %s\", dirty.c_str() );' % ( self._table.tablename(), '%s' ) )
            else :
                self._table.generate_shardcodes( lines )
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                lines.append( '    \"UPDATE `%s_%s` SET %s\", shard, dirty.c_str() );' % ( self._table.tablename(), '%u', '%s' ) )
        else :
            if self._table.sharding() == 0 :
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                if len( values ) == 0 :
                    lines.append( '    \"UPDATE `%s` SET %s WHERE %s\", dirty.c_str() );' % ( self._table.tablename(), '%s', keys ) )
                else :
                    lines.append( '    \"UPDATE `%s` SET %s WHERE %s\", dirty.c_str(), %s );' % ( self._table.tablename(), '%s', keys, values ) )
            else :
                self._table.generate_shardcodes( lines )
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                if len( values ) == 0 :
                    lines.append( '    \"UPDATE `%s_%s` SET %s WHERE %s\", shard, dirty.c_str() );' % ( self._table.tablename(), '%u', '%s', keys ) )
                else :
                    lines.append( '    \"UPDATE `%s_%s` SET %s WHERE %s\", shard, dirty.c_str(), %s );' % ( self._table.tablename(), '%u', '%s', keys, values ) )
        lines.append( 'sqlcmd.resize( len );')
        for key in self._table.prikeys() :
            field = self._table.get_field( key )
            if field[1] == 'std::string' :
                lines.append( 'escapevalues.push_back( m_db_%s );' % field[0] );
        lines.append( '\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_remove( self, lines ) :
        keys, values, valueslen = self._table.get_keys_values()
        lines.append( 'bool %s_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )' % self._table.tablename() )
        lines.append( '{' )
        if len(valueslen) == 0 :
            lines.append( 'uint32_t len = 32 + %d + %d;\n' % ( len(self._table.tablename()), len(keys) ) )
        else :
            lines.append( 'uint32_t len = 32 + %d + %d + %s;\n' % ( len(self._table.tablename()), len(keys), valueslen ) )
        lines.append( 'sqlcmd.resize( len );' )
        if len(keys) == 0 :
            if self._table.sharding() == 0 :
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                lines.append( '    \"DELETE FROM `%s`\" );' % ( self._table.tablename() ) )
            else :
                self._table.generate_shardcodes( lines )
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                lines.append( '    \"DELETE FROM `%s_%s`\", shard );' % ( self._table.tablename(), '%u' ) )
        else :
            if self._table.sharding() == 0 :
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                if len(values) == 0 :
                    lines.append( '    \"DELETE FROM `%s` WHERE %s\" );' % ( self._table.tablename(), keys ) )
                else :
                    lines.append( '    \"DELETE FROM `%s` WHERE %s\", %s );' % ( self._table.tablename(), keys, values ) )
            else :
                self._table.generate_shardcodes( lines )
                lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
                if len(values) == 0 :
                    lines.append( '    \"DELETE FROM `%s_%s` WHERE %s\", shard );' % ( self._table.tablename(), '%u', keys ) )
                else :
                    lines.append( '    \"DELETE FROM `%s_%s` WHERE %s\", shard, %s );' % ( self._table.tablename(), '%u', keys, values ) )
        lines.append( 'sqlcmd.resize( len );' )
        for key in self._table.prikeys() :
            field = self._table.get_field( key )
            if field[1] == 'std::string' :
                lines.append( 'escapevalues.push_back( m_db_%s );' % field[0] );
        lines.append( '\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_replace( self, lines ) :
        keystring = ''
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                if keystring != '' :
                    keystring += ' && '
                keystring += 'i != %d' % field[2]
        lines.append( 'bool %s_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'std::string fields, values, dirty;\n' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 && %s )' % keystring )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'if ( !fields.empty() )' )
        lines.append( '{' )
        lines.append( 'fields += \", \";' )
        lines.append( '}' )
        lines.append( 'if ( !values.empty() )' )
        lines.append( '{' )
        lines.append( 'values += \", \";' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    fields += \"`%s`\";' % field[0] );
            if field[1] == 'std::string' :
                lines.append( '    values += \"\'?\'\";' )
                lines.append( '    escapevalues.push_back( m_db_%s );' % (field[0]) )
            else :
                lines.append( '    values += utils::Integer::toString( m_db_%s );' % field[0] )
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}' )
        if self._table.aifield() != '' :
            lines.append( 'dirty = \"%s=LAST_INSERT_ID(%s)\";' % (self._table.aifield(), self._table.aifield()) )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 1 && %s )' % keystring )
        lines.append( '{' )
        lines.append( 'if ( !dirty.empty() )' )
        lines.append( '{' )
        lines.append( 'dirty += \", \";' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            if not self._table.iskey( field[0] ) :
                lines.append( 'case %s :' % field[2] );
                if field[1] == 'std::string' :
                    lines.append( '    dirty += \"`%s`=\";' % field[0] )
                    lines.append( '    dirty += \"\'?\'\";' )
                    lines.append( '    escapevalues.push_back( m_db_%s );' % (field[0]) )
                else :
                    lines.append( '    dirty += \"`%s`=\";' % field[0] )
                    lines.append( '    dirty += utils::Integer::toString( m_db_%s );' % field[0] )
                lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( 'uint32_t len = 64 + %d + fields.size() + values.size() + dirty.size();\n' % len(self._table.tablename()) );
        lines.append( 'sqlcmd.resize( len );' )
        if self._table.sharding() == 0 :
            lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
            lines.append( '    \"INSERT INTO `%s` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s\", fields.c_str(), values.c_str(), dirty.c_str() );' % ( self._table.tablename(), '%s', '%s', '%s' ) )
        else :
            self._table.generate_shardcodes( lines )
            lines.append( 'len = std::snprintf( (char *)sqlcmd.data(), len-1,' )
            lines.append( '    \"INSERT INTO `%s_%s` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s\", shard, fields.c_str(), values.c_str(), dirty.c_str() );' % ( self._table.tablename(), '%u', '%s', '%s', '%s' ) )
        lines.append( 'sqlcmd.resize(len);\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_encode( self, lines ) :
        lines.append( 'Slice %s_Data::encode( uint8_t op )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'uint16_t nfields = 0;' )
        lines.append( 'StreamBuf pack( 1024, sizeof(uint16_t) );\n' )
        lines.append( '// fields' )
        lines.append( 'if ( unlikely(op == eCodec_Query) )' )
        lines.append( '{' )
        for k in self._table.indexs() :
            field = self._table.get_field( k )
            lines.append( 'if ( m_dirty[%d] == 1 )' % field[2] )
            lines.append( '{')
            lines.append( '++nfields;' );
            lines.append( 'pack.encode( (uint16_t)%d );' % field[2] );
            lines.append( 'pack.encode( m_db_%s );' % field[0] );
            lines.append( '}' )
        lines.append( '}' )
        lines.append( 'else' )
        lines.append( '{' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( op != eCodec_All && m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    ++nfields;' );
            lines.append( '    pack.encode( i );' );
            lines.append( '    pack.encode( m_db_%s );' % field[0] );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( '// key fields' )
        lines.append( "if ( op != eCodec_Query )" )
        lines.append( "{" )
        for k in self._table.allkeys() :
            field = self._table.get_field( k )
            lines.append( "pack.encode( m_db_%s );" % field[0] )
        lines.append( '}\n' )
        lines.append( '// fields number' )
        lines.append( 'pack.reset();' )
        lines.append( 'pack.encode( nfields );\n' )
        lines.append( 'return pack.slice();' )
        lines.append( '}\n' )

    def generate_decode( self, lines ) :
        lines.append( 'bool %s_Data::decode( const Slice & result, uint8_t op )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'uint16_t nfields = 0;' )
        lines.append( 'StreamBuf unpack( result.data(), result.size() );\n' )
        lines.append( '// fields number' )
        lines.append( 'unpack.decode( nfields );\n' )
        lines.append( '// fields' )
        lines.append( 'for ( uint16_t i = 0; i < nfields; ++i )' )
        lines.append( '{' )
        lines.append( 'uint16_t index = 0;' )
        lines.append( 'unpack.decode( index );\n' )
        lines.append( 'switch ( index )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    m_dirty[%d] = 1;' % field[2] );
            lines.append( '    unpack.decode( m_db_%s );' % field[0] );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( '// keys' )
        lines.append( "if ( op != eCodec_Query )" )
        lines.append( "{" )
        for k in self._table.allkeys() :
            field = self._table.get_field( k )
            lines.append( "unpack.decode( m_db_%s );" % field[0] )
        lines.append( '}\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_merge( self, lines ) :
        lines.append( 'void %s_Data::merge( const ISQLData * data )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'assert( data->tablename() == TABLENAME );' )
        lines.append( 'const %s_Data * d = static_cast<const %s_Data *>(data);\n' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( '// fields' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( d->m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    m_dirty[%d] = 1;' % field[2] );
            lines.append( '    set_db_%s( d->get_db_%s() );' % ( field[0], field[0] ) );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}' )
        lines.append( '}\n' )

    def generate_match( self, lines ) :
        lines.append( 'bool %s_Data::match( const ISQLData * data ) const' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'assert( data->tablename() == TABLENAME );' )
        lines.append( 'const %s_Data * d = static_cast<const %s_Data *>(data);\n' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( '// fields' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    if ( get_db_%s() != d->get_db_%s() ) return false;' % ( field[0], field[0] ) );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}' )
        lines.append( 'return true;' );
        lines.append( '}\n' )

    def generate_luainterface( self, lines ) :
        lines.append( 'void %s_Data::registering( kaguya::State * state )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( '(*state)[\"%s_Data\"].setClass( kaguya::UserdataMetatable<%s_Data, ISQLData>()' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( '    .setConstructors<%s_Data(), %s_Data(%s)>()' % (self._table.tablename(), self._table.tablename(), self.keyparams_lua() ) )
        for field in self._table.fields() :
            lines.append( '    .addFunction( \"get_db_%s\", &%s_Data::get_db_%s )' % (field[0], self._table.tablename(), field[0]) )
            lines.append( '    .addFunction( \"set_db_%s\", &%s_Data::set_db_%s )' % (field[0], self._table.tablename(), field[0]) )
        lines.append( '    .addStaticFunction( \"convert\", [] ( ISQLData * data ) -> %s_Data * { return static_cast<%s_Data *>( data ); } )' % (self._table.tablename(), self._table.tablename() ) )
        lines.append( ');' )
        lines.append( '}\n' )
        lines.append( 'bool %s_Data::invoke( kaguya::State * state, const std::string & script )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( '(*state)[\"%s\"] = this;' % self._table.tablename() )
        lines.append( 'state->dostring( script.c_str() );' )
        lines.append( '(*state)[\"%s\"] = nullptr;' % self._table.tablename() )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_parse( self, lines ) :
        lines.append( 'bool %s_Data::parse( const Slices & result )' % self._table.tablename() )
        lines.append( '{' )
        lines.append( 'if ( result.size() != NFIELDS )' )
        lines.append( '{' )
        lines.append( 'return false;' )
        lines.append( '}\n' )
        lines.append( 'for ( uint16_t i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            if field[1] == 'int64_t' :
                lines.append( '    m_db_%s = (%s)std::atoll( result[i].ToString().c_str() );' % (field[0], field[1]) );
            elif field[1] == 'uint64_t' :
                lines.append( '    m_db_%s = (%s)std::atoll( result[i].ToString().c_str() );' % (field[0], field[1]) );
            elif field[1] == 'std::string' :
                lines.append( '    m_db_%s = result[i].ToString();' % ( field[0] ) );
            else :
                lines.append( '    m_db_%s = (%s)std::atoi( result[i].ToString().c_str() );' % (field[0], field[1]) );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_copyfrom( self, lines ) :
        lines.append( 'void %s_Data::copyfrom( const %s_Data & data )' % ( self._table.tablename(), self._table.tablename() ) )
        lines.append( '{' )
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                continue
            lines.append( '// Field: %s, Index: %d' % (field[0], field[2]) )
            lines.append( "m_dirty[%d] = 1;" % field[2] )
            lines.append( "m_db_%s = data.get_db_%s();" % ( field[0], field[0] ) )
        lines.append( '}\n' );

    def generate_js_clean( self, lines ) :
        lines.append( 'this.clean = function()' )
        lines.append( '{' )
        lines.append( 'm_method = 0' )
        lines.append( 'for ( var i = 0; i < NFIELDS; i++ )' )
        lines.append( '{' )
        lines.append( 'm_dirty[i] = 0' )
        lines.append( '}')
        lines.append( '}\n' )

    def generate_js_dirty( self, lines ) :
        lines.append( "this.isDirty = function()" )
        lines.append( '{' )
        lines.append( 'for ( var i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] != 0 ) return true;' )
        lines.append( '}\n' )
        lines.append( 'return false;' )
        lines.append( '}\n' )

    def generate_js_key( self, lines ) :
        lines.append( 'this.keystring = function()' )
        lines.append( '{' )
        lines.append( 'var key = \"\"' )
        for i in range( len(self._table.prikeys()) ) :
            field = self._table.get_field( self._table.prikeys()[i] )
            if i != 0 :
                lines.append( 'key += \"#\";' )
            lines.append( 'key += m_db_%s;' % field[0] )
        lines.append( 'return key;' )
        lines.append( '}\n' )

    def generate_js_encode( self, lines ) :
        lines.append( 'this.encode = function()' )
        lines.append( '{' )
        lines.append( 'var nfields = 0;' )
        lines.append( 'var pack = new StreamBuf();\n' )
        lines.append( '// fields' )
        lines.append( 'for ( var i = 0; i < NFIELDS; ++i )' )
        lines.append( '{' )
        lines.append( 'if ( m_dirty[i] == 0 )' )
        lines.append( '{' )
        lines.append( 'continue;' )
        lines.append( '}\n' )
        lines.append( 'switch ( i )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            lines.append( '    ++nfields;' );
            lines.append( '    pack.ushort( i );' );
            if field[1] == 'int8_t' :
                lines.append( '    pack.int8( m_db_%s );' % field[0] );
            elif field[1] == 'uint8_t':
                lines.append( '    pack.uint8( m_db_%s );' % field[0] );
            elif field[1] == 'int16_t' :
                lines.append( '    pack.short( m_db_%s );' % field[0] );
            elif field[1] == 'uint16_t' :
                lines.append( '    pack.ushort( m_db_%s );' % field[0] );
            elif field[1] == 'int32_t' :
                lines.append( '    pack.int32( m_db_%s );' % field[0] );
            elif field[1] == 'uint32_t' :
                lines.append( '    pack.uint32( m_db_%s );' % field[0] );
            elif field[1] == 'int64_t':
                lines.append( '    pack.int64( m_db_%s );' % field[0] );
            elif field[1] == 'uint64_t':
                lines.append( '    pack.uint64( m_db_%s );' % field[0] );
            elif field[1] == 'std::string' :
                lines.append( '    pack.string( m_db_%s );' % field[0] );
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( '// key fields' )
        for k in self._table.allkeys():
            field = self._table.get_field( k )
            if field[1] == 'int8_t' :
                lines.append( 'pack.int8( m_db_%s );' % field[0] );
            elif field[1] == 'uint8_t':
                lines.append( 'pack.uint8( m_db_%s );' % field[0] );
            elif field[1] == 'int16_t' :
                lines.append( 'pack.short( m_db_%s );' % field[0] );
            elif field[1] == 'uint16_t' :
                lines.append( 'pack.ushort( m_db_%s );' % field[0] );
            elif field[1] == 'int32_t' :
                lines.append( 'pack.int32( m_db_%s );' % field[0] );
            elif field[1] == 'uint32_t' :
                lines.append( 'pack.uint32( m_db_%s );' % field[0] );
            elif field[1] == 'int64_t' :
                lines.append( 'pack.int64( m_db_%s );' % field[0] );
            elif field[1] == 'uint64_t':
                lines.append( 'pack.uint64( m_db_%s );' % field[0] );
            elif field[1] == 'std::string' :
                lines.append( 'pack.string( m_db_%s );' % field[0] );
        lines.append( '// fields number' )
        lines.append( 'pack.ushort( nfields, 0 );\n' )
        lines.append( 'pack.pack();' )
        lines.append( 'return pack.toSlice();' )
        lines.append( '}\n' )

    def generate_js_decode( self, lines ) :
        lines.append( 'this.decode = function( value )' )
        lines.append( '{' )
        lines.append( 'var unpack = new StreamBuf(value);' )
        lines.append( 'var content = unpack.unpack();\n')
        lines.append( '// fields number' )
        lines.append( 'unpack.ushort();' )
        lines.append( 'var nfields = content[ content.length - 1 ]\n' )
        lines.append( '// fields' )
        lines.append( 'for ( var i = 0; i < nfields; ++i )' )
        lines.append( '{' )
        lines.append( 'unpack.ushort();' )
        lines.append( 'var index = content[ content.length - 1 ]\n' )
        lines.append( 'switch ( index )' )
        lines.append( '{' )
        for field in self._table.fields() :
            lines.append( 'case %s :' % field[2] );
            if field[1] == 'int8_t' :
                lines.append( '    {' )
                lines.append( '        unpack.int8();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'uint8_t':
                lines.append( '    {' )
                lines.append( '        unpack.uint8();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'int16_t' :
                lines.append( '    {' )
                lines.append( '        unpack.short();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'uint16_t' :
                lines.append( '    {' )
                lines.append( '        unpack.ushort();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'int32_t' :
                lines.append( '    {' )
                lines.append( '        unpack.int32();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'uint32_t' :
                lines.append( '    {' )
                lines.append( '        unpack.uint32();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'int64_t':
                lines.append( '    {' )
                lines.append( '        unpack.int64();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ].toString();'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'uint64_t':
                lines.append( '    {' )
                lines.append( '        unpack.uint64();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ].toString();'  % field[0] )
                lines.append( '    }' )
            elif field[1] == 'std::string' :
                lines.append( '    {' )
                lines.append( '        unpack.string();' );
                lines.append( '        m_db_%s = content[ content.length - 1 ];'  % field[0] )
                lines.append( '    }' )
            lines.append( '    break;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( '// key fields' )
        for k in self._table.allkeys() :
            field = self._table.get_field( k )
            if field[1] == 'int8_t' :
                lines.append( 'unpack.int8();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'uint8_t':
                lines.append( 'unpack.uint8();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'int16_t' :
                lines.append( 'unpack.short();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'uint16_t' :
                lines.append( 'unpack.ushort();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'int32_t' :
                lines.append( 'unpack.int32();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'uint32_t' :
                lines.append( 'unpack.uint32();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
            elif field[1] == 'int64_t' :
                lines.append( 'unpack.int64();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ].toString();'  % field[0] )
            elif field[1] == 'uint64_t':
                lines.append( 'unpack.uint64();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ].toString();'  % field[0] )
            elif field[1] == 'std::string' :
                lines.append( 'upack.string();' );
                lines.append( 'm_db_%s = content[ content.length - 1 ];'  % field[0] )
        lines.append( 'return true;' )
        lines.append( '}\n' )

    def generate_go( self, dstpath ) :
        lines = []
        lines.append( '// Code generated by tool. DO NOT EDIT.' )
        lines.append( '\n' )
        lines.append( 'package %s' % self._table.tablename().lower() )
        lines.append( '\n' )
        lines.append( 'import (' )
        lines.append( '    \"fmt\"' )
        lines.append( '    \"scheme/utils\"' )
        lines.append( ')' )
        lines.append( '\n' )
        lines.append( 'const (' )
        lines.append( '    TableName string = `%s`' % self._table.tablename() )
        lines.append( '    NFIELDS   uint16 = %d ' % len( self._table.fields() ) )
        lines.append( ')' )
        lines.append( '\n' )
        lines.append( 'type %s struct {' % self._table.tablename() )
        for field in self._table.fields() :
            if self._table.iskey( field[0] ) :
                lines.append( '    %-18s %-15s `json:\"%s\"`    // primary_key' % ( field[0].capitalize(), self._table.gotype( field[1] ), field[0] ) )
            else :
                lines.append( '    %-18s %-15s `json:\"%s\"`' % ( field[0].capitalize(), self._table.gotype( field[1] ), field[0] ) )
        lines.append( '    dirty              [NFIELDS]int8' )
        lines.append( '}' )
        lines.append( '\n' )
        lines.append( 'func (%s) TableName() string {' % self._table.tablename() )
        lines.append( '    return TableName' )
        lines.append( '}' )
        lines.append( '\n' )
        self.generate_go_columns( lines )
        self.generate_go_clone( lines )
        self.generate_go_clean( lines )
        self.generate_go_dirty( lines )
        self.generate_go_key( lines )
        self.generate_go_encode( lines )
        self.generate_go_decode( lines )
        for field in self._table.fields() :
            self._table.generate_go_getter_setter( lines, field )

        dstpath = '%s/%s' % ( dstpath, self._table.tablename().lower() );
        if not os.path.exists( dstpath ) :
            os.makedirs( dstpath )
        self._table.save2( '%s/%s.go' % ( dstpath, self._table.tablename().lower() ), lines )

    def generate_go_columns( self, lines ) :
        lines.append( 'func (m *%s) Columns() []interface{} {' % self._table.tablename() )
        lines.append( '    return []interface{} {' )
        for field in self._table.fields() :
            lines.append( '        &m.%s,' % field[0].capitalize() )
        lines.append( '        &m.dirty,' )
        lines.append( '    }' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_clone( self, lines ) :
        lines.append( 'func (m *%s) Clone() interface{} {' % self._table.tablename() )
        lines.append( '    z := &%s{}' % self._table.tablename() )
        for field in self._table.fields() :
            lines.append( '    z.%s = m.%s' % ( field[0].capitalize(), field[0].capitalize() ) )
        lines.append( '    z.dirty = m.dirty' )
        lines.append( '    return z' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_clean( self, lines ) :
        lines.append( 'func (m *%s) Clean() {' % self._table.tablename() )
        lines.append( '    var i uint16' )
        lines.append( '    for i = 0; i < NFIELDS; i++ {' )
        lines.append( '        m.dirty[i] = 0' )
        lines.append( '    }' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_dirty( self, lines ) :
        lines.append( 'func (m *%s) IsDirty() bool {' % self._table.tablename() )
        lines.append( '    var i uint16' )
        lines.append( '    for i = 0; i < NFIELDS; i++ {' )
        lines.append( '        if m.dirty[i] != 0 {' )
        lines.append( '            return true' )
        lines.append( '        }' )
        lines.append( '    }' )
        lines.append( '    return false' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_key( self, lines ) :
        lines.append( 'func (m *%s) KeyString() string {' % self._table.tablename() )
        lines.append( '    key := \"\"' )
        for i in range( len( self._table.prikeys() ) ) :
            field = self._table.get_field( self._table.prikeys()[i] )
            if i != 0 :
                lines.append( '    key += \"#\"' )
            lines.append( '    key += fmt.Sprint(m.%s)' % field[0].capitalize() )
        lines.append( '    return key' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_encode( self, lines ) :
        lines.append( 'func (m *%s) Encode() *[]byte {' % self._table.tablename() )
        lines.append( '    pack := utils.NewEncodeStream(&[]byte{})' )
        lines.append( '\n' )
        lines.append( '    // fields ' )
        lines.append( '    var nfields uint16 = 0' )
        lines.append( '    pack.Encode(&nfields)' )
        lines.append( '\n' )
        lines.append( '    var i uint16' )
        lines.append( '    for i = 0; i < NFIELDS; i++ {' )
        lines.append( '        if m.dirty[i] == 0 {' )
        lines.append( '            continue' )
        lines.append( '        }' )
        lines.append( '\n' )
        lines.append( '        switch i {' )
        for field in self._table.fields() :
            lines.append( '        case %d:' % field[2] )
            lines.append( '            nfields++' )
            lines.append( '            pack.Encode(&i)' )
            lines.append( '            pack.Encode(&m.%s)' % field[0].capitalize() )
        lines.append( '        default:' )
        lines.append( '        }' )
        lines.append( '    }' )
        lines.append( '\n' )
        lines.append( '    // key fields' )
        for k in self._table.allkeys() :
            field = self._table.get_field( k )
            lines.append( '    pack.Encode(&m.%s)' % field[0].capitalize() )
        lines.append( '\n' )
        lines.append( '    offset := pack.GetOffset()' )
        lines.append( '\n' )
        lines.append( '    // fields number' )
        lines.append( '    pack.Reset()' )
        lines.append( '    pack.Encode(&nfields)' )
        lines.append( '\n' )
        lines.append( '    tmp := make([]byte, offset)' )
        lines.append( '    copy(tmp, *pack.GetBufWithLength(offset))' )
        lines.append( '\n' )
        lines.append( '    return &tmp' )
        lines.append( '}' )
        lines.append( '\n' )

    def generate_go_decode( self, lines ) :
        lines.append( 'func (m *%s) Decode(data *[]byte) {' % self._table.tablename() )
        lines.append( '    unpack := utils.NewDecodeStream(data)' )
        lines.append( '\n' )
        lines.append( '    // fields number' )
        lines.append( '    var nfields uint16' )
        lines.append( '    unpack.Decode(&nfields)' )
        lines.append( '\n' )
        lines.append( '    // fields' )
        lines.append( '    var i uint16' )
        lines.append( '    for i = 0; i < nfields; i++ {' )
        lines.append( '        var index uint16' )
        lines.append( '        unpack.Decode(&index)' )
        lines.append( '        switch index {' )
        for field in self._table.fields() :
            lines.append( '        case %d:' % field[2] );
            lines.append( '            m.dirty[%d] = 1' % field[2] )
            lines.append( '            unpack.Decode(&m.%s)' % field[0].capitalize() )
        lines.append( '        default:' )
        lines.append( '        }' )
        lines.append( '    }' )
        lines.append( '\n' )
        lines.append( '    // key fields' )
        for k in self._table.allkeys() :
            field = self._table.get_field( k )
            lines.append( '    unpack.Decode(&m.%s)' % field[0].capitalize() )
        lines.append( '}' )
        lines.append( '\n' )
