
#ifndef __SRC_MASTERSERVER_HTTPREQUEST_H__
#define __SRC_MASTERSERVER_HTTPREQUEST_H__

#include "io/io.h"
#include "base/types.h"
#include "http/xcurl.h"

namespace master {

// 登录
class Login2AccountDB : public CurlRequest
{
public:
    Login2AccountDB( const std::string & url );
    virtual ~Login2AccountDB();

    virtual void onResponse();
    virtual void onError( const char * reason );

public:
    void setAffID( uint32_t id );
    void setZoneID( ZoneID id );
    void setStatus( int32_t status );
    void setAccount( const std::string & account );
    void setRoleID( UnitID id );
    void setAvatar( uint32_t avatar );
    void setRolename( const std::string & rolename );
    void setLevel( uint32_t lv );
    void setVipLevel( uint32_t lv );
    void setLoginTime( time_t now );
    void setCreateTime( time_t now );
    void setOnlineRoleNumber( uint32_t count );
    void setCreateRoleNumber( uint32_t count );
    void setBattlepoint( int64_t battlepoint );
    void setDungeonStage( const std::string & stage );
    void setStoreAccount( const std::string & type, const std::string & account );

private:
    uint32_t m_AffID;
    ZoneID m_ZoneID;
    int32_t m_Status;
    std::string m_Account;
    std::string m_StoreType;
    std::string m_StoreAccount;
    UnitID m_RoleID;
    uint32_t m_Avatar;
    std::string m_Rolename;
    uint32_t m_Level;
    uint32_t m_VipLevel;
    time_t m_LoginTime;
    time_t m_CreateTime;
    int64_t m_Battlepoint;      // 战斗力
    std::string m_DungeonStage; // 普通关卡进度
    uint32_t m_OnlineRoleNumber;
    uint32_t m_CreateRoleNumber;
};

class ActivateRoleRequest : public CurlRequest
{
public:
    ActivateRoleRequest( const std::string & url );
    virtual ~ActivateRoleRequest();

    virtual void onResponse();
    virtual void onError( const char * reason );

public:
    void setRoleID( UnitID roleid );
    void setAccount( const std::string & account );
    void setStatus( int32_t status );

private:
    UnitID m_RoleID;
    std::string m_Account;
    int32_t m_Status;
};

class UseGiftCodeRequest : public CurlRequest
{
public:
    UseGiftCodeRequest( const std::string & url );
    virtual ~UseGiftCodeRequest();

    virtual void onResponse();
    virtual void onError( const char * reason );

public:
    void setRoleID( UnitID roleid );
    void setCode( const char * code );
    void setSid( sid_t sid ) { m_Sid = sid; }
    void setChannelID( uint32_t channelid );

private:
    UnitID m_RoleID;
    std::string m_Code;
    sid_t m_Sid;
    uint32_t m_ChannelID;
};

} // namespace master

#endif
