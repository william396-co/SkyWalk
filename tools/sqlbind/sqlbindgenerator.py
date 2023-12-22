#!/usr/bin/env python
# -*- coding:utf-8 -*-

class SqlbindGenerator() :
    def __init__( self, results ) :
        self._results = results

    def generate( self, path, tablestatus, version ) :
        lines = []
        lines.append( '#include \"lua/kaguya.hpp\"' )
        for result in self._results :
            lines.append( '#include \"%s.hpp\"' % result[0] )
        lines.append( '\n' )
        lines.append( 'namespace data' )
        lines.append( '{\n' )
        lines.append( '// VERSION' )
        lines.append( 'std::string TableProtoType::VERSION = \"%s\";\n' % ( version ) )
        lines.append( 'TableProtoType::TableProtoType( kaguya::State * state )' )
        lines.append( '\t: m_CloseState( state == nullptr ),' )
        lines.append( '\t  m_State( state )' )
        lines.append( '{}\n' )
        lines.append( 'TableProtoType::~TableProtoType()' )
        lines.append( '{}\n' )
        lines.append( 'void TableProtoType::initialize()' )
        lines.append( '{' )
        lines.append( 'if ( m_State == nullptr )' )
        lines.append( '{' )
        lines.append( 'm_State = new kaguya::State();' )
        lines.append( 'assert ( m_State != nullptr && \"new kaguya::State failed\" );\n' )
        lines.append( 'kaguya::LoadLibs libs {' )
        lines.append( '    {"_G", luaopen_base},' )
        lines.append( '    {"LUA_MATHLIBNAME", luaopen_math},' )
        lines.append( '    {"LUA_STRLIBNAME", luaopen_string} };' )
        lines.append( 'm_State->openlibs( libs );' )
        lines.append( '}\n' )
        #lines.append( '(*m_State)["ISQLData"].setClass( kaguya::UserdataMetatable<ISQLData>()' )
        #lines.append( '\t.setConstructors<ISQLData()>()' )
        #lines.append( '\t.addFunction( \"invoke\", &ISQLData::invoke )' )
        #lines.append( ');\n' )
        for i in range( len(self._results) ) :
            if i != 0 :
                lines.append( '\n' )
            result = self._results[i]
            lines.append( '%s_Data::registering( m_State );' % result[0] )
            lines.append( 'm_Tables.push_back( \"%s\" );' % result[0] )
            if not tablestatus[ result[0] ]:
                lines.append( 'm_InCompleteTable.insert( \"%s\" );' % result[0] )
            lines.append( 'm_DataProtoType.emplace( \"%s\", new %s_Data() );' % ( result[0], result[0] ) )
        lines.append( '}\n' )
        lines.append( 'void TableProtoType::finalize()' )
        lines.append( '{' )
        lines.append( 'for ( const auto & val : m_DataProtoType )' )
        lines.append( '{' )
        lines.append( 'delete val.second;' )
        lines.append( '}\n' )
        lines.append(  'm_Tables.clear();' )
        lines.append(  'm_InCompleteTable.clear();' )
        lines.append(  'm_DataProtoType.clear();\n' )
        lines.append( 'if ( m_CloseState && m_State != nullptr )' )
        lines.append( '{' )
        lines.append( 'delete m_State;' )
        lines.append( 'm_State = nullptr;' )
        lines.append( '}' )
        lines.append( '}\n' )
        lines.append( '}' )
        self.save2( '%s/sqlbind.cpp'%path, lines )

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
