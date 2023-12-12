#pragma once

#include "base/types.h"
#include "base/endpoint.h"

#include "configure/config.h"
#include "configure/variableconfig.h"

namespace data {
class Variable_Data;
};

// 服务器参数(逻辑服务器)
class ServerVariable 
{
public:
    ServerVariable( uint8_t machine, ZoneID zone, HostID id );
    ~ServerVariable();

public:
    bool initialize();
    void finalize();
    // 设置数据对象
    data::Variable_Data * getUniqueData() const { return m_UniqueData; }

public:
    // 生成角色ID
    RoleID genRoleID();
    RoleID getRoleID() const;
    // 生成工会ID
    GuildID genGuildID();
    static ZoneID getZoneID( GuildID id );
    // 生成邮件ID
    MailID genMailID();
    // 生成道具ID
    EntityID genEntityID();
    // 生成伙伴id
    HeroID genHeroID();
    // 生成日志id
    LogID genLogID();
    // 生成留言ID
    uint64_t genMessageID();
    // 生成场景ID
    SceneID genSceneID( int8_t type, int32_t resource, int32_t channel = 0 );

    //
    void getSceneChannel( SceneID id, DungeonID & dungeonid, uint16_t & channel ) const;

private:
    // type+zoneid+hostid+autoincr
    uint64_t generate( uint8_t type, uint64_t id );

private:
    uint8_t m_Machine; // 机房
    ZoneID m_ZoneID;
    HostID m_HostID;
    uint32_t m_CopySeq;                 // 副本序号
    data::Variable_Data * m_UniqueData; //
};
