
#include <algorithm>

#include "base/base.h"
#include "dataagent/helper.h"
#include "utils/thread.h"
#include "utils/utility.h"

#include "dataagent/manager.h"
#include "scheme/Variable.hpp"

#include "variable.h"
#include "masterproc.h"

ServerVariable::ServerVariable( uint8_t machine, ZoneID zone, HostID id )
    : m_Machine( machine ),
      m_ZoneID( zone ),
      m_HostID( id ),
      m_CopySeq( 0 ),
      m_UniqueData( nullptr )
{}

ServerVariable::~ServerVariable()
{}

bool ServerVariable::initialize()
{
    data::Variable_Data idx_variable;
    idx_variable.set_db_id( m_HostID );
    g_DataManager.query( &idx_variable, 100, [this]( data::QueryStage stage, const Tablename & table, const Slices & results ) {
        switch ( stage ) {
            case data::QueryStage::None : break;
            case data::QueryStage::Begin :
                APPLICATION->update( eInitState_Variable, SwitchState::On );
                break;
            case data::QueryStage::Timeout :
                LOG_FATAL( "ServerVariable::initialize() : load all Variable(HostID:{}) timeout .\n", m_HostID );
                MASTER.signal( SIGQUIT );
                break;
            case data::QueryStage::Datasets : {
                assert( table == "Variable" );
                assert( results.size() == 0 || results.size() == 1 );
                if ( results.size() == 1 ) {
                    m_UniqueData = g_DataManager.parse<data::Variable_Data>( *( results.begin() ) );
                    assert( m_UniqueData != nullptr && "create data::Variable_Data failed" );
                } else {
                    m_UniqueData = g_DataManager.create<data::Variable_Data>();
                    // 记录为空时插入一条记录
                    m_UniqueData->set_db_id( m_HostID );
                    m_UniqueData->set_db_roleid( 0 );
                    m_UniqueData->set_db_mailid( 0 );
                    m_UniqueData->set_db_teamid( 0 );
                    m_UniqueData->set_db_imageid( 0 );
                    m_UniqueData->set_db_entityid( 0 );
                    m_UniqueData->set_db_guildid( 0 );
                    m_UniqueData->set_db_heroid( 0 );
                    m_UniqueData->set_db_logid( 0 );
                    m_UniqueData->set_db_messageid( 0 );
                    g_DataManager.insert( m_UniqueData );
                }
                LOG_INFO( "ServerVariable::setUniqueData(HostID:{}) : RoleID:{}, EntityID:{}, HeroID:{}, GuildID:{}, MailID:{}, TeamID:{}, ImageID:{} .\n",
                    m_HostID, m_UniqueData->get_db_roleid(), m_UniqueData->get_db_entityid(), m_UniqueData->get_db_heroid(),
                    m_UniqueData->get_db_guildid(), m_UniqueData->get_db_mailid(), m_UniqueData->get_db_teamid(), m_UniqueData->get_db_imageid() );
            } break;
            case data::QueryStage::Complete :
                APPLICATION->update( eInitState_Variable, SwitchState::Off );
                break;
        }
    } );

    return true;
}

void ServerVariable::finalize()
{
    if ( m_UniqueData != nullptr ) {
        g_DataManager.destroy( m_UniqueData );
        m_UniqueData = nullptr;
    }
}

RoleID ServerVariable::genRoleID()
{
    RoleID roleid = 0;
    RoleID id = m_UniqueData->get_db_roleid();

    // 更新
    ++id;
    assert( id < ROLEID_MAX && "RoleID OVERFLOW" );
    m_UniqueData->set_db_roleid( id );
    g_DataManager.update( m_UniqueData );

    //
    // 角色ID规则
    //
    // | 4bits | 14bits |    30bits    |
    // | 类型  | ZoneID | AutoIncrease |
    //

    roleid |= id;
    roleid |= ( (RoleID)m_ZoneID << ROLEID_BYTES );

    return roleid;
}

RoleID ServerVariable::getRoleID() const
{
    return m_UniqueData->get_db_roleid() + 1;
}

GuildID ServerVariable::genGuildID()
{
    GuildID guildid = 0;
    GuildID id = m_UniqueData->get_db_guildid();

    // 更新
    ++id;
    assert( id < GUILDID_MAX && "GuildID OVERFLOW" );
    m_UniqueData->set_db_guildid( id );
    g_DataManager.update( m_UniqueData );

    //
    // 工会ID规则
    //
    // | 1bits | 14bits |     20bits   |
    // |  RES  | ZoneID | AutoIncrease |
    //

    guildid |= id;
    guildid |= ( (GuildID)m_ZoneID << GUILDID_BYTES );
    guildid |= ( (GuildID)1 << ( GUILDID_BYTES + ZONEID_BYTES ) );

    return guildid;
}

ZoneID ServerVariable::getZoneID( GuildID id )
{
    id >>= GUILDID_BYTES;
    id &= ( ( 1 << ZONEID_BYTES ) - 1 );
    return (ZoneID)id;
}

MailID ServerVariable::genMailID()
{
    uint64_t id = m_UniqueData->get_db_mailid();

    // 自增ID
    ++id;
    assert( id < AUTOINCR_MAX && "MailID OVERFLOW" );

    // 保存
    m_UniqueData->set_db_mailid( id );
    g_DataManager.update( m_UniqueData );

    return this->generate( eIDType_Mail, id );
}

EntityID ServerVariable::genEntityID()
{
    uint64_t id = m_UniqueData->get_db_entityid();

    // 自增ID
    ++id;

    assert( id < AUTOINCR_MAX && "EntityID OVERFLOW" );
    if ( id > AUTOINCR_MAX ) {
        LOG_FATAL( "ServerVariable::genEntityID({}) : EntityID Overflow .\n", id );
        return 0;
    }

    // 保存
    m_UniqueData->set_db_entityid( id );
    g_DataManager.update( m_UniqueData );

    return this->generate( eIDType_Entity, id );
}

HeroID ServerVariable::genHeroID()
{
    uint64_t id = m_UniqueData->get_db_heroid();

    // 自增
    ++id;
    assert( id < AUTOINCR_MAX && "HeroID OVERFLOW" );

    // 保存
    m_UniqueData->set_db_heroid( id );
    g_DataManager.update( m_UniqueData );

    return this->generate( eIDType_Hero, id );
}

LogID ServerVariable::genLogID()
{
    uint64_t id = m_UniqueData->get_db_logid();

    // 自增
    ++id;
    assert( id < AUTOINCR_MAX && "LogID OVERFLOW" );
    // 保存
    m_UniqueData->set_db_logid( id );
    g_DataManager.update( m_UniqueData );

    return this->generate( eIDType_Like, id );
}

uint64_t ServerVariable::genMessageID()
{
    uint64_t id = m_UniqueData->get_db_messageid();

    // 自增
    ++id;
    assert( id < AUTOINCR_MAX && "MessageID OVERFLOW" );
    // 保存
    m_UniqueData->set_db_messageid( id );
    g_DataManager.update( m_UniqueData );

    return this->generate( eIDType_Message, id );
}

SceneID ServerVariable::genSceneID( int8_t type, int32_t resource, int32_t channel )
{
    SceneID id = SCENE_BASEID( type );

    switch ( type ) {
        case eSceneType_City :
            id |= resource;
            break;

        case eSceneType_Dungeon :
        case eSceneType_BrawlGround : {
            assert( resource < ( 1 << DUNGEONID_BYTES )
                && channel < ( 1 << DUNGEONCHANNEL_BYTES ) );
            id |= ( resource << DUNGEONCHANNEL_BYTES );
            id |= channel;
        } break;

        case eSceneType_Copy : {
            if ( ++m_CopySeq >= COPYSEQ_MAX ) {
                m_CopySeq = 0;
            }
            id |= ( m_HostID << COPYSEQ_BYTES );
            id |= m_CopySeq;
        } break;
    }

    return id;
}

void ServerVariable::getSceneChannel( SceneID id, DungeonID & dungeonid, uint16_t & channel ) const
{
    // assert( SCENE_TYPE(id) == eSceneType_Dungeon );
    dungeonid = id & ~SCENE_TYPE_MASK;
    dungeonid >>= DUNGEONCHANNEL_BYTES;
    channel = ( id & ~SCENE_TYPE_MASK ) & ( ( 1 << DUNGEONCHANNEL_BYTES ) - 1 );
}

uint64_t ServerVariable::generate( uint8_t type, uint64_t id )
{
    uint64_t rid = 0ULL;

    //
    // ID规则
    //
    // | 4bits | 14bits |  8bits |    38bits    |
    // | Type  | ZoneID | HostID | AutoIncrease |
    //

    rid |= id;
    rid |= ( (uint64_t)m_HostID << AUTOINCR_BYTES );
    rid |= ( (uint64_t)m_ZoneID << ( HOSTID_BYTES + AUTOINCR_BYTES ) );
    rid |= ( (uint64_t)type << ( ZONEID_BYTES + HOSTID_BYTES + AUTOINCR_BYTES ) );

    return rid;
}
