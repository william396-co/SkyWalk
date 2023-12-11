
#include "base/battle.h"
#include "base/resource.h"
#include "base/endpoint.h"
#include "base/modules.h"
#include "base/role.h"
#include "base/guild.h"
#include "base/database.h"
#include "base/event.h"
#include "base/dungeon.h"
#include "base/hero.h"
#include "base/activity.h"
#include "utils/streambuf.h"

#if defined(__APPLE__)
template<> bool StreamBuf::decode( time_t & data )
{
    int64_t value = 0;

    decode( value );
    data = (time_t)value;
    return true;
}

template<> bool StreamBuf::encode( const time_t & data )
{
    encode( (int64_t)data );
    return true;
}

template<> bool StreamBuf::decode( size_t & data )
{
    uint64_t value = 0;

    decode( value );
    data = (size_t)value;
    return true;
}

template<> bool StreamBuf::encode( const size_t & data )
{
    encode( (uint64_t)data );
    return true;
}
#endif

template<> bool StreamBuf::decode( HostType & data )
{
    uint8_t type = 0;

    decode( type );
    data = (HostType)type;
    return true;
}

template<> bool StreamBuf::encode( const HostType & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( DBMethod & data )
{
    uint8_t type = 0;

    decode( type );
    data = (DBMethod)type;
    return true;
}

template<> bool StreamBuf::encode( const DBMethod & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( ZoneMode & data )
{
    uint8_t mode = 0;

    decode( mode );
    data = (ZoneMode)mode;
    return true;
}

template<> bool StreamBuf::encode( const ZoneMode & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( MailType & data )
{
    uint8_t type = 0;

    decode( type );
    data = (MailType)type;
    return true;
}

template<> bool StreamBuf::encode( const MailType & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( MailStatus & data )
{
    uint8_t type = 0;

    decode( type );
    data = (MailStatus)type;
    return true;
}

template<> bool StreamBuf::encode( const MailStatus & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( BulletinType & data )
{
    uint8_t type = 0;

    decode( type );
    data = (BulletinType)type;
    return true;
}

template<> bool StreamBuf::encode( const BulletinType & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( SyncMethod & data )
{
    uint8_t type = 0;

    decode( type );
    data = (SyncMethod)type;
    return true;
}

template<> bool StreamBuf::encode( const SyncMethod & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( SceneOp & data )
{
    uint8_t type = 0;

    decode( type );
    data = (SceneOp)type;
    return true;
}

template<> bool StreamBuf::encode( const SceneOp & data )
{
    encode( (uint8_t)data );
    return true;
}

template<> bool StreamBuf::decode( LoginMode & data )
{
    int32_t type = 0;

    decode( type );
    data = (LoginMode)type;
    return true;
}

template<> bool StreamBuf::encode( const LoginMode & data )
{
    encode( (int32_t)data );
    return true;
}

template<> bool StreamBuf::decode( CombatSide & data )
{
    uint16_t type = 0;

    decode( type );
    data = (CombatSide)type;
    return true;
}

template<> bool StreamBuf::encode( const CombatSide & data )
{
    encode( (uint16_t)data );
    return true;
}

template<> bool StreamBuf::decode( RoleState & data )
{
    decode( data.id );
    decode( data.level );
    decode( data.name );
    decode( data.gate_hostid );
    decode( data.scene_hostid );
    return true;
}

template<> bool StreamBuf::encode( const RoleState & data )
{
    encode( data.id );
    encode( data.level );
    encode( data.name );
    encode( data.gate_hostid );
    encode( data.scene_hostid );
    return true;
}

template<> bool StreamBuf::decode( Resource & data )
{
    decode( data.type );
    decode( data.object );
    decode( data.count );
    return true;
}

template<> bool StreamBuf::encode( const Resource & data )
{
    encode( data.type );
    encode( data.object );
    encode( data.count );
    return true;
}

template<> bool StreamBuf::decode( Skill & data )
{
    decode( data.id );
    decode( data.type );
    decode( data.level );
    decode( data.building );
    return true;
}

template<> bool StreamBuf::encode( const Skill & data )
{
    encode( data.id );
    encode( data.type );
    encode( data.level );
    encode( data.building );

    return true;
}

template<> bool StreamBuf::decode( QueryUnit & data )
{
    decode( data.table );
    decode( data.indexstr );
    decode( data.dbdata );
    return true;
}

template<> bool StreamBuf::encode( const QueryUnit & data )
{
    encode( data.table );
    encode( data.indexstr );
    encode( data.dbdata );

    return true;
}

template<> bool StreamBuf::decode( TableIndexEntry & data )
{
    decode( data.table );
    decode( data.indexstr );
    return true;
}

template<> bool StreamBuf::encode( const TableIndexEntry & data )
{
    encode( data.table );
    encode( data.indexstr );

    return true;
}

template<> bool StreamBuf::decode( Endpoint & data )
{
    decode( data.host );
    decode( data.port );
    return true;
}

template<> bool StreamBuf::encode( const Endpoint & data )
{
    encode( data.host );
    encode( data.port );

    return true;
}

template<> bool StreamBuf::decode( HostEndpoint & data )
{
    decode( data.id );
    decode( data.type );
    decode( data.endpoint );
    decode( data.extendpoint );
    return true;
}

template<> bool StreamBuf::encode( const HostEndpoint & data )
{
    encode( data.id );
    encode( data.type );
    encode( data.endpoint );
    encode( data.extendpoint );

    return true;
}

template<> bool StreamBuf::decode( Attribute<> & attr )
{
    decode( attr.values );
    return true;
}

template<> bool StreamBuf::encode( const Attribute<> & attr )
{
    encode( attr.values );
    return true;
}

template<> bool StreamBuf::decode( std::pair<std::string, std::string> & data )
{
    decode( data.first );
    decode( data.second );
    return true;
}

template<> bool StreamBuf::encode( const std::pair<std::string, std::string> & data )
{
    encode( data.first );
    encode( data.second );

    return true;
}

template<> bool StreamBuf::decode( RankRole & data )
{
    decode( data.roleid );
    decode( data.rank );
    decode( data.avatar );
    decode( data.rolename );
    decode( data.score );
    decode( data.level );
    decode( data.viplv );
    decode( data.lastrank );
    return true;
}

template<> bool StreamBuf::encode( const RankRole & data )
{
    encode( data.roleid );
    encode( data.rank );
    encode( data.avatar );
    encode( data.rolename );
    encode( data.score );
    encode( data.level );
    encode( data.viplv );
    encode( data.lastrank );
    return true;
}

template<> bool StreamBuf::decode( ActivityStage & data )
{
    decode( data.timestamp );
    decode( data.count );
    return true;
}

template<> bool StreamBuf::encode( const ActivityStage & data )
{
    encode( data.timestamp );
    encode( data.count );
    return true;
}

template<> bool StreamBuf::encode( const Pair & data )
{
    encode( data.first );
    encode( data.second );
    return true;
}

template<> bool StreamBuf::decode( Pair & data )
{
    decode( data.first );
    decode( data.second );
    return true;
}

template<> bool StreamBuf::encode( const GuildwarStatBrief & data )
{
    encode( data.score );
    encode( data.members );
    encode( data.teams );
    encode( data.revives );
    encode( data.kills );

    return true;
}

template<> bool StreamBuf::decode( GuildwarStatBrief & data )
{
    decode( data.score );
    decode( data.members );
    decode( data.teams );
    decode( data.revives );
    decode( data.kills );

    return true;
}

template<> bool StreamBuf::encode( const Vec2i & data )
{
    encode( data.x );
    encode( data.y );
    return true;
}

template<> bool StreamBuf::decode( Vec2i & data )
{
    decode( data.x );
    decode( data.y );

    return true;
}

template<> bool StreamBuf::encode( const Period & data )
{
    encode( data.first );
    encode( data.second );
    return true;
}

template<> bool StreamBuf::decode( Period & data )
{
    decode( data.first );
    decode( data.second );

    return true;
}

template<> bool StreamBuf::encode( const std::pair<UnitID, HostIDs> & data )
{
    encode( data.first );
    encode( data.second );
    return true;
}

template<> bool StreamBuf::decode( std::pair<UnitID, HostIDs> & data )
{
    decode( data.first );
    decode( data.second );

    return true;
}

template<> bool StreamBuf::encode( const std::pair<UnitID, Attribute<>::Type> & data )
{
    encode( data.first );
    encode( data.second );
    return true;
}

template<> bool StreamBuf::decode( std::pair<UnitID, Attribute<>::Type> & data )
{
    decode( data.first );
    decode( data.second );

    return true;
}

template<> bool StreamBuf::encode( const EventArg & data )
{
    encode( data.type );
    encode( data.param1 );
    encode( data.param2 );
    encode( data.param3 );
    return true;
}

template<> bool StreamBuf::decode( EventArg & data )
{
    decode( data.type );
    decode( data.param1 );
    decode( data.param2 );
    decode( data.param3 );

    return true;
}

template<> bool StreamBuf::encode( const ChannelInfo & data )
{
    encode( data.sceneid );
    encode( data.hostid );
    encode( data.count );
    return true;
}

template<> bool StreamBuf::decode( ChannelInfo & data )
{
    decode( data.sceneid );
    decode( data.hostid );
    decode( data.count );
    return true;
}

template<> bool StreamBuf::encode( const BattleOrder & data )
{
    encode( data.roleid );
    encode( data.heroid );
    encode( data.action );
    encode( data.values );
    return true;
}

template<> bool StreamBuf::decode( BattleOrder & data )
{
    decode( data.roleid );
    decode( data.heroid );
    decode( data.action );
    decode( data.values );
    return true;
}

template<> bool StreamBuf::encode( const BattleFrame & data )
{
    encode( data.frame );
    encode( data.orderlist );
    return true;
}

template<> bool StreamBuf::decode( BattleFrame & data )
{
    decode( data.frame );
    decode( data.orderlist );
    return true;
}

template<> bool StreamBuf::encode( const BasicHero & data )
{
    encode( data.id );
    encode( data.baseid );
    encode( data.helper );
    encode( data.star );
    encode( data.grade );
    encode( data.quality );
    encode( data.rate );
    encode( data.level );
    encode( data.battlepoint );
    encode( data.spells );
    encode( data.attribute );
    encode( data.hp );
    encode( data.indamage );
    encode( data.outdamage );
    return true;
}

template<> bool StreamBuf::decode( BasicHero & data )
{
    decode( data.id );
    decode( data.baseid );
    decode( data.helper );
    decode( data.star );
    decode( data.grade );
    decode( data.quality );
    decode( data.rate );
    decode( data.level );
    decode( data.battlepoint );
    decode( data.spells );
    decode( data.attribute );
    decode( data.hp );
    decode( data.indamage );
    decode( data.outdamage );
    return true;
}

template<> bool StreamBuf::encode( const CombatJournal & data )
{
    encode( data.id );
    encode( data.roleid );
    encode( data.opponent );
    encode( data.result );
    encode( data.robotid );
    encode( data.avatar );
    encode( data.rolename );
    encode( data.level );
    encode( data.viplevel );
    encode( data.change );
    encode( data.timestamp );
    encode( data.statistics );
    encode( data.extrainfo );
    return true;
}

template<> bool StreamBuf::decode( CombatJournal & data )
{
    decode( data.id );
    decode( data.roleid );
    decode( data.opponent );
    decode( data.result );
    decode( data.robotid );
    decode( data.avatar );
    decode( data.rolename );
    decode( data.level );
    decode( data.viplevel );
    decode( data.change );
    decode( data.timestamp );
    decode( data.statistics );
    decode( data.extrainfo );
    return true;
}
