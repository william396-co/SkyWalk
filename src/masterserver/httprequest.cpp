
#include "base/base.h"

#include "io/harbor.h"
#include "extra/json.h"

#include "message/gmprotocol.h"
#include "message/gameprotocol.h"

#include "server.h"
#include "httprequest.h"

namespace master {

Login2AccountDB::Login2AccountDB( const std::string & url )
    : CurlRequest( url + "/account/login" ),
      m_AffID( 0 ),
      m_ZoneID( 0 ),
      m_Status( 0 ),
      m_RoleID( 0 ),
      m_Avatar( 0 ),
      m_Level( 0 ),
      m_VipLevel( 0 ),
      m_LoginTime( 0 ),
      m_CreateTime( 0 ),
      m_Battlepoint( 0 ),
      m_CreateRoleNumber( 0 )
{}

Login2AccountDB::~Login2AccountDB()
{}

void Login2AccountDB::onResponse()
{
    do {
        rapidjson::Document doc;
        doc.Parse( this->getResponse().c_str() );

        // 解析失败
        if ( doc.HasParseError()
            || !doc.IsObject() || !doc.HasMember( "result" ) || !doc.HasMember( "message" ) ) {
            LOG_ERROR( "Login2AccountDB::onResponse(Account:'{}', AffID:{}, ZoneID:{}, RoleID:{}) : parse this Response(FullURL:'{}', '{}') failed .\n",
                m_Account, m_AffID, m_ZoneID, m_RoleID, getUrl(), getResponse() );
            break;
        }

        int32_t result = doc["result"].GetInt();

        if ( result == 0 ) {
            LOG_INFO( "Login2AccountDB::onResponse() : this Character(Account:'{}', AffID:{}, ZoneID:{}, RoleID:{}) login to the Platform succeed .\n",
                    m_Account, m_AffID, m_ZoneID, m_RoleID );
        } else {
            LOG_ERROR( "Login2AccountDB::onResponse() : this Character(Account:'{}', AffID:{}, ZoneID:{}, RoleID:{}) login to the Platform failed, FullURL:'{}', Errorcode:{}, Message:'{}' .\n",
                m_Account, m_AffID, m_ZoneID, m_RoleID, getUrl(), result, doc["message"].GetString() );
        }
    } while ( 0 );
}

void Login2AccountDB::onError( const char * reason )
{
    LOG_ERROR( "Login2AccountDB::onError() : the Character(Account:'{}', AffID:{}, ZoneID:{}, RoleID:{}) login failed, FullURL:'{}', '{}' .\n",
        m_Account.c_str(), m_AffID, m_ZoneID, m_RoleID, getUrl(), reason );
}

void Login2AccountDB::setAffID( uint32_t id )
{
    m_AffID = id;
    addParam( "affId", std::to_string( id ) );
}

void Login2AccountDB::setZoneID( ZoneID id )
{
    m_ZoneID = id;
    addParam( "zoneId", std::to_string( id ) );
}

void Login2AccountDB::setStatus( int32_t status )
{
    m_Status = status;
    addParam( "status", std::to_string( m_Status ) );
}

void Login2AccountDB::setAccount( const std::string & account )
{
    m_Account = account;
    addParam( "account", account );
}

void Login2AccountDB::setRoleID( UnitID id )
{
    m_RoleID = id;
    addParam( "roleId", std::to_string( id ) );
}

void Login2AccountDB::setAvatar( uint32_t avatar )
{
    m_Avatar = avatar;
    addParam( "avatar", std::to_string( avatar ) );
}

void Login2AccountDB::setRolename( const std::string & rolename )
{
    m_Rolename = rolename;
    addParam( "name", rolename );
}

void Login2AccountDB::setLevel( uint32_t lv )
{
    m_Level = lv;
    addParam( "level", std::to_string( lv ) );
}

void Login2AccountDB::setVipLevel( uint32_t lv )
{
    m_VipLevel = lv;
    addParam( "viplv", std::to_string( lv ) );
}

void Login2AccountDB::setLoginTime( time_t now )
{
    m_LoginTime = now;
    addParam( "loginTime", std::to_string( now ) );
}

void Login2AccountDB::setCreateTime( time_t now )
{
    m_CreateTime = now;
    addParam( "createTime", std::to_string( now ) );
}

void Login2AccountDB::setOnlineRoleNumber( uint32_t count )
{
    m_OnlineRoleNumber = count;
    addParam( "onlinerolenumber", std::to_string(count) );
}

void Login2AccountDB::setCreateRoleNumber( uint32_t count )
{
    m_CreateRoleNumber = count;
    addParam( "createrolenumber", std::to_string( count ) );
}

void Login2AccountDB::setBattlepoint( int64_t battlepoint )
{
    m_Battlepoint = battlepoint;
    addParam( "battlePoint", std::to_string( battlepoint ) );
}

void Login2AccountDB::setDungeonStage( const std::string & stage )
{
    m_DungeonStage = stage;
    addParam( "dungeonstage", stage );
}

void Login2AccountDB::setStoreAccount( const std::string & type, const std::string & account )
{
    m_StoreType = type;
    m_StoreAccount = account;
    addParam( "storetype", type );
    addParam( "storeaccount", account );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ActivateRoleRequest::ActivateRoleRequest( const std::string & url )
    : CurlRequest( url + "/role/status" ),
      m_RoleID( 0 ),
      m_Status( 0 )
{}

ActivateRoleRequest::~ActivateRoleRequest()
{}

void ActivateRoleRequest::onResponse()
{
    do {
        rapidjson::Document doc;
        doc.Parse( this->getResponse().c_str() );

        // 解析失败
        if ( doc.HasParseError()
            || !doc.IsObject() || !doc.HasMember( "result" ) || !doc.HasMember( "message" ) ) {
            LOG_ERROR( "ActivateRoleRequest::onResponse(Account:'{}', RoleID:{}, Status:{}) : parse this Response(FullURL:'{}', '{}') failed .\n",
                m_Account, m_RoleID, m_Status, getUrl(), getResponse() );
            break;
        }

        int32_t result = doc["result"].GetInt();
        if ( result == 0 ) {
            LOG_TRACE( "ActivateRoleRequest::onResponse() : this Character(Account:'{}', RoleID:{}, Status:{}) update to the Platform succeed .\n",
                    m_Account, m_RoleID, m_Status );
        } else {
            LOG_ERROR( "ActivateRoleRequest::onResponse() : this Character(Account:'{}', RoleID:{}, Status:{}) update to the Platform failed, FullURL:'{}', Errorcode:{}, Message:'{}' .\n",
                m_Account, m_RoleID, m_Status, getUrl(), result, doc["message"].GetString() );
        }
    } while ( 0 );
}

void ActivateRoleRequest::onError( const char * reason )
{
    LOG_ERROR( "ActivateRoleRequest::onError() : the Character(Account:'{}', RoleID:{}, Status:{}) update to the Platform failed, FullURL:'{}', '{}' .\n",
        m_Account.c_str(), m_RoleID, m_Status, getUrl(), reason );
}

void ActivateRoleRequest::setStatus( int32_t status )
{
    m_Status = status;
    addParam( "status", std::to_string( status ) );
}

void ActivateRoleRequest::setAccount( const std::string & account )
{
    m_Account = account;
    addParam( "account", account );
}

void ActivateRoleRequest::setRoleID( UnitID id )
{
    m_RoleID = id;
    addParam( "roleId", std::to_string( id ) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

UseGiftCodeRequest::UseGiftCodeRequest( const std::string & url )
    : CurlRequest( url + "/game_service/giftcode" ),
      m_RoleID( 0ULL ),
      m_Sid( 0ULL ),
      m_ChannelID( 0 )
{}

UseGiftCodeRequest::~UseGiftCodeRequest()
{}

void UseGiftCodeRequest::onResponse()
{
    // json格式返回何种格式待定
    // {"code":10000,"status":0,"rewards":[{"type":1,"object":100002,"count":10},{"type":2,"object":100003,"count":20}]}

    LOG_DEBUG( "UseGiftCodeRequest:{} channelid:{} code:{} res:{} \n", m_RoleID, m_ChannelID, m_Code.c_str(), this->getResponse().c_str() );

    bool success = false;
    do {
        rapidjson::Document doc;
        doc.Parse( this->getResponse().c_str() );

        // 解析失败
        if ( doc.HasParseError()
            || !doc.IsObject() || !doc.HasMember( "status" ) ) {
            LOG_ERROR( "UseGiftCodeRequest::onResponse(RoleID:'{}', code:{}) : parse this Response('{}') failed .\n", m_RoleID, m_Code.c_str(), this->getResponse().c_str() );
            break;
        }

        if ( getCode() == 200 ) {
            LOG_DEBUG( "UseGiftCodeRequest::onResponse(RoleID:'{}', code:{}) : parse this Response('{}') success .\n", m_RoleID, m_Code.c_str(), this->getResponse().c_str() );
            success = true;
            break;
        } else {
            if ( doc.HasMember( "status" ) ) {
                LOG_ERROR( "UseGiftCodeRequest::onResponse() : this Character(RoleID:'{}' code:'{}') used failed, Errorcode={} .\n", m_RoleID, m_Code.c_str(), doc["status"].GetString() );
            } else {
                LOG_ERROR( "UseGiftCodeRequest::onResponse() : this Character(RoleID:'{}', code:'{}') used failed, Unknown Reason .\n", m_RoleID, m_Code.c_str() );
            }
        }
    } while ( 0 );

    if ( success ) {
        GiftCode gift;
        utils::JsonReader reader( getResponse().c_str() );
        reader.get( gift );

        // 返回场景
        UseGiftCodeCommand cmd;
        cmd.roleid = m_RoleID;
        cmd.code = gift.code;
        cmd.status = gift.status;
        std::copy( gift.rewards.begin(), gift.rewards.end(), std::back_inserter( cmd.rewards ) );
        g_HarborService->send( m_Sid, &cmd );
    }
}

void UseGiftCodeRequest::onError( const char * reason )
{
    LOG_ERROR( "UseGiftCodeRequest::onError reason {}", reason );
}

void UseGiftCodeRequest::setRoleID( UnitID roleid )
{
    m_RoleID = roleid;
    addParam( "roleid", std::to_string( roleid ) );
}

void UseGiftCodeRequest::setCode( const char * code )
{
    m_Code = code;
    addParam( "code", code );
}

void UseGiftCodeRequest::setChannelID( uint32_t channelid )
{
    m_ChannelID = channelid;
    addParam( "channel", std::to_string( channelid ) );
}

} // namespace master
