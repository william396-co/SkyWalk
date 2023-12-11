
#include <stdlib.h>
#include <algorithm>

#include "base/base.h"

#include "helper.h"
#include "manager.h"

namespace data {

void SchemeHelper::initialize( const TableProtoType * proto, const Register & regschemes )
{
    // 注册
    regschemes( this );

    // 赋值数据原型
    m_Prototype = proto;

    // 计算系统表
    for ( const auto & table : m_Prototype->tables() ) {
        if ( m_UserTables.end()
            == m_UserTables.find( table ) ) {
            m_SysTables.push_back( table );
        }
    }
}

void SchemeHelper::finalize()
{
    for ( const auto & p : m_UserTables ) {
        delete p.second.second;
    }

    m_SysTables.clear();
    m_UserTables.clear();
}

bool SchemeHelper::isSysTable( const Tablename & table ) const
{
    return m_SysTables.end()
        != std::find( m_SysTables.begin(), m_SysTables.end(), table );
}

bool SchemeHelper::isUserTable( const Tablename & table ) const
{
    return m_UserTables.find( table ) != m_UserTables.end();
}

UnitID SchemeHelper::getRoleID( ISQLData * data ) const
{
    auto result = m_UserTables.find( data->tablename() );
    if ( result != m_UserTables.end() ) {
        return result->second.second->get( data );
    }

    return 0;
}

ISQLData * SchemeHelper::fillQueryer( UnitID id, const Tablename & table ) const
{
    auto result = m_UserTables.find( table );
    if ( result != m_UserTables.end() ) {
        ISQLData * data = m_Prototype->data( table );
        if ( data != nullptr ) {
            result->second.second->set( data, id );
            return data;
        }
    }

    return nullptr;
}

void SchemeHelper::fillQueryerList( UnitID id, HostType type, std::vector<ISQLData *> & qs ) const
{
    for ( const auto & p : m_UserTables ) {
        auto pos = std::find(
            p.second.first.begin(), p.second.first.end(), type );
        if ( pos != p.second.first.end() ) {
            ISQLData * data = m_Prototype->data( p.first );
            if ( data == nullptr ) {
                continue;
            }
            qs.push_back( data );
            p.second.second->set( data, id );
        }
    }
}

bool ResultsProcessor::has( const Tablename & table ) const
{
    auto result = m_Processors.find( table );
    if ( result != m_Processors.end() ) {
        return result->second.first;
    }
    return false;
}

void ResultsProcessor::bind( const Tablename & table, const cpp_fn & pf )
{
    auto result = m_Processors.find( table );
    if ( result != m_Processors.end() ) {
        m_Processors.erase( result );
    }
    m_Processors.emplace( table, std::make_pair(false, pf) );
}

void ResultsProcessor::bind( const Tablename & table, const lua_ref & self, const lua_fn & pf )
{
    auto result = m_Processors.find( table );
    if ( result != m_Processors.end() ) {
        m_Processors.erase( result );
    }
    m_Processors.emplace( table, std::make_pair( false,Processor{ self, pf} ) );
}

void ResultsProcessor::unbind( const Tablename & table )
{
    auto result = m_Processors.find( table );
    if ( result != m_Processors.end() ) {
        m_Processors.erase( result );
    }
}

void ResultsProcessor::process( const Tablename & table, const Slices & results )
{
    auto pos = m_Processors.find( table );
    if ( pos != m_Processors.end() ) {
        pos->second.first = true;
        pos->second.second( table, results );
    }
}

} // namespace data
