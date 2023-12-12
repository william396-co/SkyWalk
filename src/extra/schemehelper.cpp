
#include "utils/hashfunc.h"
#include "dataagent/helper.h"
#include "dataagent/manager.h"

#include "scheme/CharBase.hpp"
#include "scheme/CharBrief.hpp"

#include "scheme/Relation.hpp"



#include "scheme/Arena.hpp"
#include "scheme/CombatJournal.hpp"

#include "schemehelper.h"

namespace data {

uint8_t general_router( uint8_t count, const Tablename & table, const std::string & idxstr )
{
    if ( !g_SchemeHelper->isSysTable( table ) ) {
        return utils::HashFunction::murmur32( idxstr.c_str(), idxstr.size() ) % count;
    }

    return utils::HashFunction::murmur32( table.c_str(), table.size() ) % count;
}

std::pair<bool, UnitID> parse_exception( const Tablename & table, const Slice & dbdata, const std::string & scripts )
{
    if ( g_SchemeHelper->isSysTable( table ) ) {
        return { true, 0 };
    }

    UnitID roleid = 0;
    ISQLData * data = g_DataManager.prototype()->data( table );
    if ( data == nullptr ) {
        return { false, 0 };
    }

    data->decode( dbdata, 0 );
    roleid = g_SchemeHelper->getRoleID( data );
    g_DataManager.destroy( data );

    return { true, roleid };
}

void schemes_register( SchemeHelper * helper )
{
    // 世界服务器
    helper->regUserTable<data::Relation_Data>( HostType::World );
    helper->regUserTable<data::Arena_Data>( HostType::World );
    // TODO: 添加世界服务器的表

    // 场景服务器
    helper->regUserTable<data::CharBase_Data>( HostType::Scene );
    // TODO: 添加场景服务器的表

    // 多服务器的表
    helper->regUserTable<data::CombatJournal_Data>( { HostType::Scene, HostType::World } );
}

} // namespace data
