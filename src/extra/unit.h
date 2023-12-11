#pragma once

#include "utils/types.h"

#include "base/role.h"
#include "base/types.h"
#include "base/endpoint.h"

class Unit 
{
public:
    Unit( UnitID id )
        : m_ID( id )
    {}

    virtual ~Unit()
    {}

    // 获取配置ID
    virtual int32_t getBaseID() const { return 0; }
    // 获取等级
    virtual int32_t getLevel() const { return 0; }
    // 获取VIP等级
    virtual int32_t getVipLevel() const { return 0; }
    // 获取职业
    virtual RaceType getRaceType() const { return 0; }
    // 获取名字
    virtual const char * getName() const { return ""; }
    // 获取Avatar头像ID
    virtual uint32_t getAvatarID() const { return 0; }

public:
    // 获取生物类型
    static UnitType getType( UnitID id );
    // 获取角色ID
    static RoleID getRoleID( UnitID id );
    // 获取角色ID中的区服ID
    static ZoneID getZoneID( UnitID id );

    // 生成角色ID
    static UnitID genRoleID( RoleID id );

    // 数字码 <-> RoleID
    static UnitID toRoleID( const std::string & code );
    static void toDigitalCode( UnitID id, int64_t timestamp, std::string & code );

    // 获取头像和边框
    // |<-闲置->|<-称号->|<-头像->|<-头像框->|
    static void parseAvatarID(
        uint32_t id, int32_t & avatar, int32_t & border, int32_t & title );

public:
    UnitID getID() const { return m_ID; }
    UnitType getType() const { return Unit::getType( m_ID ); }

    // 获取角色ID
    RoleID getRoleID() const;

private:
    UnitID m_ID;
};

class UnitManager
{
public:
    UnitManager();
    ~UnitManager();

    // 遍历所有在线玩家
    template<class Fn>
        Fn foreach ( Fn f )
        {
            for ( const auto & p : m_UnitMap ) {
                if ( p.second->getType() == UnitType::Role ) {
                    f( p.second );
                }
            }
            return f;
        }

public:
    // 创建/销毁生物
    void destroyUnit( UnitID id );
    // 获取
    Unit * getUnit( UnitID id );
    // 添加/删除生物
    void appendUnit( Unit * unit );
    void removeUnit( Unit * unit );
    // 生成玩家的UnitID
    UnitID generate( RoleID id );
    // 生成机器人/NPC的UnitID
    UnitID generate( UnitType type, uint32_t id );
    // 获取在线所有角色
    void getOnlineRole( UnitIDList & list ) const;
    void getOnlineRole( UnorderedSet<UnitID> & s ) const;
    // 随机玩家
    Unit * randomRole();
    // 随机机器人ID
    UnitID randRobotID() const;
    // 获取在线角色数量
    uint32_t getOnlineCount() const { return m_OnlineRoleCount; }

private:
    uint64_t m_NpcSeq;
    uint64_t m_RobotSeq;

private:
    uint32_t m_OnlineRoleCount; // 在线角色数量
    UnorderedMap<UnitID, Unit *> m_UnitMap;
};
