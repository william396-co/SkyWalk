
#include "lua/kaguya.hpp"
#include "Arena.hpp"
#include "Attribute.hpp"
#include "CharBase.hpp"
#include "CharBrief.hpp"

namespace data
{

// VERSION
std::string TableProtoType::VERSION = "8a74d6c51d4b9aa815d15bc456bd1230";

TableProtoType::TableProtoType( kaguya::State * state )
	: m_CloseState( state == nullptr ),
	  m_State( state )
{}

TableProtoType::~TableProtoType()
{}

void TableProtoType::initialize()
{
    if ( m_State == nullptr )
    {
        m_State = new kaguya::State();
        assert ( m_State != nullptr && "new kaguya::State failed" );

        kaguya::LoadLibs libs {
            {"_G", luaopen_base},
            {"LUA_MATHLIBNAME", luaopen_math},
            {"LUA_STRLIBNAME", luaopen_string} };
        m_State->openlibs( libs );
    }

    Arena_Data::registering( m_State );
    m_Tables.push_back( "Arena" );
    m_DataProtoType.emplace( "Arena", new Arena_Data() );

    Attribute_Data::registering( m_State );
    m_Tables.push_back( "Attribute" );
    m_InCompleteTable.insert( "Attribute" );
    m_DataProtoType.emplace( "Attribute", new Attribute_Data() );

    m_Tables.push_back( "CharBase" );
    m_DataProtoType.emplace( "CharBase", new CharBase_Data() );

    CharBrief_Data::registering( m_State );
    m_Tables.push_back( "CharBrief" );
    m_InCompleteTable.insert( "CharBrief" );
    m_DataProtoType.emplace( "CharBrief", new CharBrief_Data() );
}

void TableProtoType::finalize()
{
    for ( const auto & val : m_DataProtoType )
    {
        delete val.second;
    }

    m_Tables.clear();
    m_InCompleteTable.clear();
    m_DataProtoType.clear();

    if ( m_CloseState && m_State != nullptr )
    {
        delete m_State;
        m_State = nullptr;
    }
}

}
