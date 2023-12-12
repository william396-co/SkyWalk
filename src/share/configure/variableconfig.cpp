
#include <iostream>
#include <algorithm>
#include <cassert>

#include "base/base.h"

#include "utils/utility.h"
#include "utils/jsarray.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "utils/xmldocument.h"
#include "variableconfig.h"

namespace meta {

VariableConfigfile::VariableConfigfile()
    : m_IsImport( false ),
      m_Status( 0 ),
      m_ZoneGroupID( 0 ),
      m_OnlineCount( 0 )
{}

VariableConfigfile::~VariableConfigfile()
{}

bool VariableConfigfile::load( const char * path )
{
    utils::XmlRawDoc rawdoc;
    utils::XmlFile xmlfile( path );

    // 解析
    rawdoc.parse<0>( xmlfile.data() );

    // 取出根节点
    utils::XmlNode * root = rawdoc.first_node();
    if ( root == nullptr ) {
        LOG_FATAL( "VariableConfigfile::load() : parse '{}' failed .\n", path );
        return false;
    }

    // importzone
    m_IsImport = utils::xmlget<bool>( root, "importzone" );

    // regions
    auto node = utils::xmlget( root, "regions" );
    if ( node != nullptr ) {
        for ( auto child = node->first_node();
            child != nullptr; child = child->next_sibling() ) {
            m_Regions.push_back( child->value() );
        }
    }

    // status
    m_Status = utils::xmlget<uint32_t>( root, "status" );
    // zonegroup
    m_ZoneGroupID = utils::xmlget<uint32_t>( root, "zonegroup" );
    // onlinecount
    m_OnlineCount = utils::xmlget<uint32_t>( root, "onlinecount" );
    // appversion
    m_AppVersion = utils::xmlget<std::string>( root, "appversion" );
    // actversion
    m_ActivityVersion = utils::xmlget<std::string>( root, "actversion" );

    // versionlist
    node = utils::xmlget( root, "versionlist" );
    if ( node != nullptr ) {
        for ( auto child = node->first_node();
            child != nullptr; child = child->next_sibling() ) {
            m_VersionList.push_back( child->value() );
        }
    }

    // payreset
    node = utils::xmlget( root, "pay" );
    if ( node != nullptr ) {
        for ( auto child = node->first_node( "reset" );
            child != nullptr; child = child->next_sibling() ) {
            uint32_t resetid = utils::xmlget<uint32_t>( child, "id" );
            int64_t starttime = utils::xmlget<int64_t>( child, "starttime" );
            int64_t endtime = utils::xmlget<int64_t>( child, "endtime" );
            m_PayResetList.emplace_back( resetid, starttime, endtime );
        }
    }

    LOG_TRACE( "VariableConfigfile::load('{}') succeed .\n", path );

    return true;
}

void VariableConfigfile::unload()
{
    m_IsImport = false;
    m_Status = 0;
    m_ZoneGroupID = 0;
    m_OnlineCount = 0;
    m_AppVersion.clear();
    m_ActivityVersion.clear();
    m_Regions.clear();
    m_VersionList.clear();
    m_PayResetList.clear();
}

std::string VariableConfigfile::getFullVersion() const
{
    std::string v;

    for ( auto && version : m_VersionList ) {
        if ( version.find_first_of( m_AppVersion ) == 0 ) {
            v = version; break;
        }
    }

    return fmt::format( "%s.%s", v, getActivityVersion() );
}

void VariableConfigfile::clearActivityVersion( const char * path )
{
    utils::XmlRawDoc rawdoc;
    utils::XmlFile xmlfile( path );

    // 解析
    rawdoc.parse<rapidxml::parse_no_data_nodes>( xmlfile.data() );

    // 取出根节点
    utils::XmlNode * root = rawdoc.first_node();
    if ( root != nullptr ) {
        utils::XmlNode * node = root->first_node( "actversion" );
        if ( node != nullptr ) {
            // 清空版本号
            node->value( "" );
            //
            std::ofstream modifydoc( path );
            modifydoc << rawdoc;
            modifydoc.close();
        }
    }
}

const PayReset * VariableConfigfile::getPayReset( int64_t now ) const
{
    for ( const auto & p : m_PayResetList ) {
        if ( p.starttime <= now
            && p.endtime >= now ) {
            return &p;
        }
    }
    return nullptr;
}

} // namespace meta
