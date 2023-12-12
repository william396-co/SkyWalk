
#include <cmath>
#include <time.h>

#include "base/base.h"
#include "base/types.h"
#include "utils/base62.h"

#include "unit.h"

UnitType Unit::getType( UnitID id )
{
    return ( UnitType )( id >> ( ROLEID_BYTES + ZONEID_BYTES ) );
}

RoleID Unit::getRoleID( UnitID id )
{
    return id & UNIT_ROLE_MASK;
}

RoleID Unit::getRoleID() const
{
    assert( getType() == UnitType::Role );
    return Unit::getRoleID( m_ID );
}

ZoneID Unit::getZoneID( UnitID id )
{
    id >>= ROLEID_BYTES;
    id &= ( ( 1 << ZONEID_BYTES ) - 1 );

    return (ZoneID)id;
}

UnitID Unit::genRoleID( UnitID id )
{
    UnitID unitid = (UnitID)UnitType::Role;

    unitid <<= ( ROLEID_BYTES + ZONEID_BYTES );
    unitid |= id;

    return unitid;
}

UnitID Unit::toRoleID( const std::string & code )
{
    uint64_t id = 0ULL;
    RoleID roleid = 0ULL;

    id = utils::Base62().decode( code );
    roleid = ( ( id & 0x01ffc00000000000ULL ) >> 16 );
    roleid |= ( id & 0x000000003fffffffULL );

    return Unit::genRoleID( roleid );
}

void Unit::toDigitalCode( UnitID id, int64_t timestamp, std::string & code )
{
    uint64_t invite10 = 1ULL;

    // 标志位 + 区服ID + 时间戳 + 自增角色ID
    ZoneID zoneid = Unit::getZoneID( id );
    RoleID roleid = Unit::getRoleID( id );

    invite10 <<= ( ZONEID_BYTES + 16 + ROLEID_BYTES );
    invite10 |= ( (uint64_t)zoneid << ( 16 + ROLEID_BYTES ) );
    invite10 |= ( ( timestamp & 0x000000000000ffffLL ) << ROLEID_BYTES );
    invite10 |= ( roleid & ( ( 1ULL << ROLEID_BYTES ) - 1 ) );

    code = utils::Base62().encode( invite10 );
}

void Unit::parseAvatarID( uint32_t avatarid,
    int32_t & avatar, int32_t & border, int32_t & title )
{
    // 确保avatar最高8位为空
    avatarid &= ( ( 1 << ( TITLE_BYTES + AVATAR_BYTES + BORDER_BYTES ) ) - 1 );
    // 边框
    border = (uint8_t)avatarid;
    avatarid >>= BORDER_BYTES;

    // 头像
    avatar = (uint8_t)avatarid;
    avatarid >>= AVATAR_BYTES;

    // 称号
    title = (uint8_t)avatarid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UnitManager::UnitManager()
    : m_NpcSeq( 0 ),
      m_RobotSeq( 0ULL ),
      m_OnlineRoleCount( 0 )
{
    // 单服默认10w个角色
    m_UnitMap.rehash( std::ceil( 100000 / m_UnitMap.max_load_factor() ) );
}

UnitManager::~UnitManager()
{
    for ( const auto & p : m_UnitMap ) {
        if ( p.second != nullptr ) {
            delete p.second;
        }
    }

    m_UnitMap.clear();
    m_OnlineRoleCount = 0;
}

void UnitManager::destroyUnit( UnitID id )
{
    auto result = m_UnitMap.find( id );
    if ( result == m_UnitMap.end() ) {
        return;
    }

    Unit * unit = result->second;
    if ( unit == nullptr ) {
        return;
    }

    if ( unit->getType() == UnitType::Role ) {
        --m_OnlineRoleCount;
    }

    delete unit;
    m_UnitMap.erase( result );
}

Unit * UnitManager::getUnit( UnitID id )
{
    auto result = m_UnitMap.find( id );
    if ( result == m_UnitMap.end() ) {
        return nullptr;
    }

    return result->second;
}

void UnitManager::appendUnit( Unit * unit )
{
    UnitID id = unit->getID();

    assert( "unit is exist"
        && m_UnitMap.find( id ) == m_UnitMap.end() );

    if ( unit->getType() == UnitType::Role ) {
        ++m_OnlineRoleCount;
    }

    m_UnitMap.emplace( id, unit );
}

void UnitManager::removeUnit( Unit * unit )
{
    if ( unit->getType() == UnitType::Role ) {
        --m_OnlineRoleCount;
    }

    m_UnitMap.erase( unit->getID() );
}

UnitID UnitManager::generate( RoleID id )
{
    UnitID unitid = Unit::genRoleID( id );

    assert( getUnit( unitid ) == nullptr
        && "this UnitID is Conflict" );

    return unitid;
}

UnitID UnitManager::generate( UnitType type, uint32_t id )
{
    UnitID unitid = eInvalidUnitID;

    switch ( type ) {
        case UnitType::Npc: {
            if ( ++m_NpcSeq == NPCID_MAX ) {
                m_NpcSeq = 0;
            }

            unitid = (UnitID)type;
            unitid <<= ( NPCID_BYTES + HOSTID_BYTES );
            unitid |= m_NpcSeq;
        } break;

        case UnitType::Robot: {
            if ( ++m_RobotSeq == UNIT_ROLE_MASK ) {
                m_RobotSeq = 0;
            }

            unitid = (UnitID)type;
            unitid <<= ( ROLEID_BYTES + ZONEID_BYTES );
            unitid |= m_RobotSeq;
        } break;

        default:
            break;
    }

    assert( getUnit( unitid ) == nullptr );

    return unitid;
}

void UnitManager::getOnlineRole( UnitIDList & list ) const
{
    for ( const auto & p : m_UnitMap ) {
        if ( Unit::getType( p.first ) == UnitType::Role ) {
            list.push_back( p.first );
        }
    }
}

void UnitManager::getOnlineRole( std::unordered_set<UnitID> & s ) const
{
    for ( const auto & p : m_UnitMap ) {
        if ( p.second->getType() == UnitType::Role ) {
            s.insert( p.first );
        }
    }
}

Unit * UnitManager::randomRole()
{
    Unit * unit = nullptr;
    int32_t index = 0;
    int32_t selected = time( nullptr ) % 10;

    for ( const auto & p : m_UnitMap ) {
        if ( Unit::getType( p.first ) == UnitType::Role ) {
            unit = p.second;

            if ( index++ == selected ) {
                break;
            }
        }
    }

    return unit;
}

UnitID UnitManager::randRobotID() const
{
    UnitID id = 0;

    id = (UnitID)UnitType::Robot;
    id <<= ( ROLEID_BYTES + ZONEID_BYTES );
    id |= m_RobotSeq;

    return id;
}
