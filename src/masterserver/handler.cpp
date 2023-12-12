
#include <cstring>

#include "base/types.h"
#include "base/base.h"
#include "base/result.h"

#include "rapidjson/document.h"
#include "utils/json.h"
#include "utils/types.h"
#include "utils/xtime.h"
#include "utils/utility.h"
#include "utils/transaction.h"
#include "http/message.h"
#include "http/curlthread.h"
#include "extra/httpsvc.h"

#include "message/gmprotocol.h"
#include "message/gateprotocol.h"
#include "message/gameprotocol.h"

#include "config.h"
#include "version.h"
#include "httprequest.h"
#include "transaction.h"

#include "server.h"
#include "handler.h"

namespace master {

void CommandHandler::process( SSMessage * cmd )
{
    switch ( cmd->head.cmd ) {
        case eSSCommand_Ntpdate: ntpdate( cmd ); break;
        case eSSCommand_UseGiftCode: useGiftCode( cmd ); break;
        case eSSCommand_Login2Account: login2Account( cmd ); break;
        case eSSCommand_ServerStatus: sendStatus( cmd ); break;
        case eSSCommand_GMResult: gmResult( cmd ); break;
        case eSSCommand_QueryRole: queryRole( cmd ); break;
        case eSSCommand_ModifyRole: modifyRole( cmd ); break;
        case eSSCommand_ActivateRole : activateRole( cmd ); break;
    }
}

void CommandHandler::process( sid_t sid, HttpRequest * request )
{
    int32_t result = false;

    if ( request->getURI() == "/status" ) {
        // 获取服务器状态
        result = status( sid, request );
    } else if ( request->getURI() == "/reload" ) {
        // 热更
        result = reload( sid, request );
    } else if ( request->getURI() == "/update" ) {
        // 更新状态
        result = update( sid, request );
    } else if ( request->getURI() == "/shellcmd" ) {
        // 执行GM指令
        result = doShellCmd( sid, request );
    } else if ( request->getURI() == "/role/query" ) {
        // 查询角色
        result = queryRole( sid, request );
    } else if ( request->getURI() == "/role/acquire" ) {
        // 查询角色
        result = acquireRole( sid, request );
    } else if ( request->getURI() == "/role/modify" ) {
        // 修改角色数据
        result = modifyRole( sid, request );
    } else if ( request->getURI() == "/role/flush" ) {
        // 刷新玩家数据落地
        result = flushRole( sid, request );
    } else if ( request->getURI() == "/role/banned" ) {
        // 封号
        result = banned( sid, request );
    } else if ( request->getURI() == "/role/shutup" ) {
        // 禁言
        result = shutup( sid, request );
    } else if ( request->getURI() == "/role/activate" ) {
        // 启用/停用账号
        result = activate( sid, request );
    } else if ( request->getURI() == "/role/rename" ) {
        // 重命名
        result = rename( sid, request );
    } else if ( request->getURI() == "/mail/add" ) {
        // 添加邮件
        result = addMail( sid, request );
    } else if ( request->getURI() == "/mail/remove" ) {
        // 移除邮件
        result = removeMail( sid, request );
    } else if ( request->getURI() == "/mail/compose" ) {
        // 撰写邮件
        result = composeMail( sid, request );
    } else if ( request->getURI() == "/rank/get" ) {
        // 排行榜
        result = getRankboard( sid, request );
    } else if ( request->getURI() == "/rank/update" ) {
        // 更新排行榜
        result = updateRank( sid, request );
    } else if ( request->getURI() == "/bulletin/add" ) {
        // 添加公告
        result = sendBulletin( sid, request );
    } else if ( request->getURI() == "/bulletin/remove" ) {
        // 移除公告
        result = removeBulletin( sid, request );
    } else if ( request->getURI() == "/pay/callback" ) {
        // 支付回调
        result = payCallback( sid, request );
    } else if ( request->getURI() == "/pay/refund" ) {
        // 支付退款回调
        result = payRefundCallback( sid, request );
    } else {
        result = eHttpResult_InvalidRequest;
    }

    // 处理完成,并且不需要保活的情况下
    // 直接关闭连接
    if ( result >= 0 ) {
        if ( result > 0 ) {
            g_HttpService->sendErrorcode(
                sid, request, __BUILD_VERSION__, result );
        }

        if ( !request->isKeepalive() ) {
            g_HttpService->shutdown( sid );
        }

        delete request;
    }
}

void CommandHandler::useGiftCode( SSMessage * msg )
{
    UseGiftCodeCommand * cmd = (UseGiftCodeCommand *)msg;

    // 发送http请求
    UseGiftCodeRequest * request = new UseGiftCodeRequest( g_AppConfig.getApiURL() );
    request->setRoleID( cmd->roleid );
    request->setCode( cmd->code.c_str() );
    request->setSid( cmd->sid );
    request->setChannelID( cmd->channelid );
    g_HttpAgent->send( eHttpMethod_Get, request, 10000 );
}

void CommandHandler::login2Account( SSMessage * msg )
{
    Login2AccountCommand * cmd = (Login2AccountCommand *)msg;

    // 为了做压力测试定制
    if ( g_AppConfig.getApiURL().empty() ) {
        return;
    }

    // 同步给账号服务器
    Login2AccountDB * req = new Login2AccountDB( g_AppConfig.getApiURL() );
    assert( req != nullptr && "create Login2AccountDB failed" );
    // 设置属性
    req->setAffID( g_AppConfig.getPfID() );
    req->setZoneID( cmd->zoneid );
    req->setStatus( 1 );
    req->setAccount( cmd->account );
    req->setRoleID( cmd->roleid );
    req->setAvatar( cmd->avatar );
    req->setRolename( cmd->rolename );
    req->setLevel( cmd->level );
    req->setVipLevel( cmd->viplevel );
    req->setLoginTime( cmd->logintime );
    req->setCreateTime( cmd->createtime );
    req->setBattlepoint( cmd->battlepoint );
    req->setStoreAccount( cmd->storetype, cmd->storeaccount );
    req->setOnlineRoleNumber( cmd->onlinerolenumber );
    req->setCreateRoleNumber( cmd->createrolenumber );

    // 提交
    g_HttpAgent->send( eHttpMethod_Get, req, 10000 );
}

void CommandHandler::sendStatus( SSMessage * cmd )
{
    ServerStatusCommand * msg = (ServerStatusCommand *)cmd;

    utils::Transaction * t = g_TransactionManager->get( msg->transid );
    if ( t == nullptr ) {
        return;
    }

    t->onTrigger( static_cast<void *>( cmd ) );
}

void CommandHandler::gmResult( SSMessage * cmd )
{
    GMResultCommand * msg = (GMResultCommand *)cmd;

    utils::Transaction * t = g_TransactionManager->get( msg->transid );
    if ( t == nullptr ) {
        LOG_ERROR( "CommandHandler::gmResult(Sid:{}) : can't found this Transaction(TransID:{}) .\n", msg->sid, msg->transid );
    } else {
        LOG_TRACE( "CommandHandler::gmResult(Sid:{}, TransID:{}) ...\n", msg->sid, msg->transid );
        t->onTrigger( static_cast<void *>( cmd ) );
        if ( GameApp::instance().getReloadTransid() == msg->transid ) {
            GameApp::instance().setReloadTransid( 0 );
        }
        delete t;
    }
}

void CommandHandler::queryRole( SSMessage * cmd )
{
    QueryRoleCommand * req = (QueryRoleCommand *)cmd;

    utils::Transaction * t = g_TransactionManager->get( req->transid );
    if ( t == nullptr ) {
        LOG_ERROR( "CommandHandler::queryRole(Sid:{}) : can't found this Transaction(TransID:{}) .\n", req->sid, req->transid );
        return;
    }

    t->onTrigger( static_cast<void *>( cmd ) ); delete t;
}

void CommandHandler::modifyRole( SSMessage * cmd )
{
    ModifyRoleCommand * req = (ModifyRoleCommand *)cmd;

    utils::Transaction * t = g_TransactionManager->get( req->transid );
    if ( t == nullptr ) {
        return;
    }

    t->onTrigger( static_cast<void *>( cmd ) );
}

void CommandHandler::activateRole( SSMessage * cmd )
{
    ActivateRoleCommand * msg = (ActivateRoleCommand *)cmd;

    // 同步给账号服务器
    ActivateRoleRequest * req = new ActivateRoleRequest( g_AppConfig.getApiURL() );
    assert( req != nullptr && "create ActivateRoleRequest failed" );
    // 设置属性
    req->setStatus( msg->status );
    req->setAccount( msg->account );
    req->setRoleID( msg->roleid );
    // 提交
    g_HttpAgent->send( eHttpMethod_Get, req, 10000 );
}

void CommandHandler::ntpdate( SSMessage * cmd )
{
    NtpdateCommand * req = (NtpdateCommand *)cmd;

    // 广播给所有服务器(避免更新到全局服务器上去)
    g_HarborService->broadcast( HostType::Gate, req );
    g_HarborService->broadcast( HostType::Copy, req );
    g_HarborService->broadcast( HostType::Scene, req );
    g_HarborService->broadcast( HostType::World, req );
    g_HarborService->broadcast( HostType::Data, req );
    global::delta_timestamp = req->timestamp - utils::now();

    utils::TimeUtils adjust;
    std::string datetime = adjust.getFormatTime( "%F %T" );

    LOG_INFO( "CommandHandler::ntpdate(DeltaSeconds:{}) : set CurTimestamp: {}, Now: {}, {} .\n",
        global::delta_timestamp, req->timestamp, adjust.getTimestamp(), datetime );
}

////////////////////////////////////////////////////////////////////////////////

int32_t CommandHandler::status( sid_t sid, HttpRequest * request )
{
    ServerStatusTransaction * transaction = nullptr;
    HostID id = std::atoi( ( *request )["id"].c_str() );

    if ( id != g_AppConfig.getServerID() ) {
    // 创建事务
        transaction = new ServerStatusTransaction();
    if ( transaction == nullptr ) {
        return eHttpResult_OutofMemory;
    }

        transaction->setHostID( id );
    transaction->setSid( sid );
    transaction->setVersion( request->getVersion().c_str() );
        transaction->setConnection( ( *request )[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( transaction, 10 );

    ServerStatusCommand cmd;
        cmd.id = id;
    cmd.transid = transaction->getTransID();

        if ( id != 0 ) {
            g_HarborService->sendByHostID( id, &cmd );
        } else {
            // Chunked协议
            HttpResponse response( __BUNDLE_NAME__ );
            response.setResult( 200 );
            response.addHeader( HttpMessage::HEADER_TRANSFER_ENCODING, "chunked" );
            response.setVersion( request->getVersion() );
            response.addHeader( HttpMessage::HEADER_CONTENT_TYPE, "text/html; charset=UTF-8" );
            std::string buffer;
            response.serialize( buffer );
            g_HttpService->send( sid, buffer );

            // 广播查询服务器状态的消息
            HostIDs ids;
            g_AppConfig.getFramework()->getHostIDs( HostType::Any, ids );
            g_HarborService->broadcast( ids, &cmd );
        }
    }

    if ( id == 0 || id == g_AppConfig.getServerID() ) {
        // 发送管理服务器的状态
        utils::JsonWriter doc;
        doc.start();
        doc.put( "uptime", MASTER.uptime() );
        doc.put( "cputime", (int64_t)MASTER.cputime() );
        doc.end();
        if ( transaction == nullptr ) {
            utils::JsonWriter writer;
            writer.start();
            writer.put( "serverid", id );
            writer.put( "servertype", (int32_t)(HostType::Master) );
            writer.put( "status", doc.content() );
            writer.end();
            g_HttpService->sendContent( sid, request, __BUILD_VERSION__, writer.tostring() );
        } else {
            transaction->sendStatus( g_AppConfig.getServerID(), HostType::Master, doc.tostring() );
        }
    }

    return 0;
}

int32_t CommandHandler::reload( sid_t sid, HttpRequest * request )
{
    uint32_t mode = std::atoi( (*request)["mode"].c_str() );

    if ( GameApp::instance().getReloadTransid() != 0 ) {
        return eHttpResult_HotfixInProgress;
    }

    ResultTransaction * trans = new ResultTransaction;
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 120 );
    GameApp::instance().setReloadTransid( trans->getTransID() );

    ReloadCommand cmd;
    cmd.mode = mode;
    cmd.transid = trans->getTransID();
    g_HarborService->broadcast( HostType::World, &cmd );

    LOG_TRACE( "CommandHandler::reload(Mode:{}, TransID:{}) : wait for the GameModule's result ...\n", mode, trans->getTransID() );
    return 0;
}

int32_t CommandHandler::update( sid_t sid, HttpRequest * request )
{
    int32_t status = std::atoi( (*request)["status"].c_str() );

    UpdateZoneStatusCommand cmd;
    cmd.status = status;
    cmd.transid = 0;
    g_HarborService->broadcast( &cmd );
    LOG_TRACE( "CommandHandler::update(Status:{}) .\n", status );

    utils::JsonWriter writer;
    writer.start();
    writer.put( "result", 0 );
    writer.put( "status", status );
    writer.end();
    g_HttpService->sendContent( sid, request, __BUILD_VERSION__, writer.tostring() );

    return 0;
}

int32_t CommandHandler::doShellCmd( sid_t sid, HttpRequest * request )
{
    std::string shellcmd = (*request)["cmd"];
    UnitID roleid = std::atoll( (*request)["roleid"].c_str() );

    LOG_TRACE( "CommandHandler::doShellCmd() : send the ShellCommand('{}') to the Character(RoleID:{}) .\n", shellcmd, roleid );

    if ( !g_AppConfig.isBetaZone() ) {
        return eHttpResult_PermissionDenied;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    g_TransactionManager->append( trans, 60 );

    ShellCommand cmd;
    cmd.roleid = roleid;
    cmd.content = shellcmd;
    cmd.transid = trans->getTransID();
    cmd.method = ShellCommand::SERVER;
    g_HarborService->broadcast( HostType::Gate, &cmd );

    return 0;
}

int32_t CommandHandler::queryRole( sid_t sid, HttpRequest * request )
{
    QueryRoleTransaction * trans = new QueryRoleTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    QueryRoleCommand cmd;
    cmd.rolename = (*request)["rolename"];
    cmd.roleid = std::atoll( (*request)["roleid"].c_str() );
    cmd.transid = trans->getTransID();
    cmd.version = (*request)["schemever"];
    cmd.tablename = (*request)["tablename"];
    g_HarborService->broadcast( HostType::World, &cmd );

    return 0;
}

int32_t CommandHandler::acquireRole( sid_t sid, HttpRequest * request )
{
    AcquireRoleListCommand cmd;
    cmd.roleid = std::atoll( (*request)["roleid"].c_str() );
    cmd.rolename = (*request)["rolename"];

    if ( cmd.roleid == 0
            && cmd.rolename.empty() ) {
        return eHttpResult_AcquireRoleListInvalid;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );
    cmd.transid = trans->getTransID();
    g_HarborService->broadcast( HostType::World, &cmd );

    return 0;
}

int32_t CommandHandler::modifyRole( sid_t sid, HttpRequest * request )
{
    ModifyRoleCommand cmd;
    cmd.roleid = std::atoll( (*request)["roleid"].c_str() );
    cmd.tablename = (*request)["tablename"];
    cmd.version = (*request)["schemever"];
    cmd.method = (DBMethod)std::atoi( (*request)["method"].c_str() );
    cmd.value = Slice( request->getContent() );

    if ( cmd.value.empty()
            && cmd.method != DBMethod::Remove ) {
        return eHttpResult_ModifyValueIsNull;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );
    cmd.transid = trans->getTransID();
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_INFO( "CommandHandler::modifyRole(RoleID:{}, Method:{}) : modify this RoleData(Tablename:{}) .\n",
            cmd.roleid, (int32_t)cmd.method, cmd.tablename );

    return 0;
}

int32_t CommandHandler::flushRole( sid_t sid, HttpRequest * request )
{
    const Tablename & tables = ( *request )["tables"];

    FlushRoleCommand cmd;
    cmd.roleid = std::atoll( ( *request )["roleid"].c_str() );
    cmd.drop = std::atoi( ( *request )["dropcache"].c_str() ) == 1 ? true : false;
    if ( !tables.empty() ) {
        rapidjson::Document doc;
        doc.Parse( tables.c_str() );
        if ( !doc.IsArray() ) {
            return eHttpResult_InvalidFlushParams;
        }
        for ( size_t i = 0; i < doc.Size(); ++i ) {
            cmd.tables.push_back( doc[i].GetString() );
        }
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    } else {
        trans->setSid( sid );
        trans->setVersion( request->getVersion() );
        trans->setConnection( ( *request )[HttpMessage::HEADER_CONNECTION] );
        g_TransactionManager->append( trans, 60 );
        cmd.transid = trans->getTransID();
    }
    g_HarborService->broadcast( HostType::World, &cmd );

    LOG_INFO( "CommandHandler::flushRole(RoleID:{}, DropCache:{}) .\n", cmd.roleid, cmd.drop );

    return 0;
}

int32_t CommandHandler::banned( sid_t sid, HttpRequest * request )
{
    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    if ( (*request)["reason"].size() > 128 ) {
        return eHttpResult_ReasonTooLong;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    BannedCommand cmd;
    cmd.reason = (*request)["reason"];
    cmd.transid = trans->getTransID();
    cmd.roleid = std::atoll( (*request)["roleid"].c_str() );
    cmd.method = std::atoi( (*request)["method"].c_str() );
    cmd.seconds = std::atoi( (*request)["seconds"].c_str() );
    g_HarborService->broadcast( HostType::World, &cmd );

    return 0;
}

int32_t CommandHandler::shutup( sid_t sid, HttpRequest * request )
{
    // TODO:
    return 0;
}

int32_t CommandHandler::activate( sid_t sid, HttpRequest * request )
{
    int32_t status = std::atoi( (*request)["status"].c_str() );
    UnitID roleid = std::atoll( (*request)["roleid"].c_str() );

    LOG_INFO( "CommandHandler::activate() : RoleID:{}, Status:{} .\n", roleid, status );

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    ActivateRoleCommand cmd;
    cmd.roleid = roleid;
    cmd.status = status;
    cmd.transid = trans->getTransID();
    g_HarborService->broadcast( HostType::World, &cmd );

    return 0;
}

int32_t CommandHandler::rename( sid_t sid, HttpRequest * request )
{
    std::string rolename = (*request)["rolename"];
    UnitID roleid = std::atoll( (*request)["roleid"].c_str() );

    LOG_INFO( "CommandHandler::rename() : RoleID:{}, Rolename:{} .\n", roleid, rolename );

    if ( rolename.size() >= 512 ) {
        return eHttpResult_RolenameTooLong;
    }

    if ( utils::Utility::checkbuiltinwords( rolename.c_str() ) ) {
        return eHttpResult_RolenameInvalid;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    ModifyRolenameCommand cmd;
    cmd.roleid = roleid;
    cmd.rolename = rolename;
    cmd.transid = trans->getTransID();
    g_HarborService->broadcast( HostType::World, &cmd );

    return 0;
}

int32_t CommandHandler::addMail( sid_t sid, HttpRequest * request )
{
    time_t starttime = std::atoll( (*request)["starttime"].c_str() );
    time_t expiretime = std::atoll( (*request)["expiretime"].c_str() );

    if ( starttime == 0 || expiretime == 0
            || starttime > expiretime || expiretime < utils::time() ) {
        return eHttpResult_MailTimeIsInvalid;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    SendGMMailCommand cmd;
    cmd.transid = trans->getTransID();
    cmd.starttime = starttime;
    cmd.expiretime = expiretime;
    cmd.mailid = std::atoi( (*request)["id"].c_str() );
    cmd.tag = std::atoi( (*request)["tag"].c_str() );
    cmd.keepdays = std::atoi( (*request)["keepdays"].c_str() );
    cmd.sendername = (*request)["sendername"];
    cmd.title = (*request)["title"];
    cmd.content = (*request)["content"];
    cmd.paramlist = (*request)["paramlist"];
    cmd.condition = (*request)["condition"];
    utils::JsonReader( (*request)["attachment"] ).get( cmd.attachments );
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_TRACE( "CommandHandler::addMail(MailID:{}, StartTime:{}, ExpireTime:{}, Condition:'{}', Title:'{}', TransID:{}) .\n",
            cmd.mailid, cmd.starttime, cmd.expiretime, cmd.condition, cmd.title.c_str(), cmd.transid );

    return 0;
}

int32_t CommandHandler::removeMail( sid_t sid, HttpRequest * request )
{
    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    RemoveGMMailCommand cmd;
    cmd.transid = trans->getTransID();
    cmd.mailid = std::atoi( ( *request )["id"].c_str() );
    // 通知世界服务器
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_TRACE( "CommandHandler::removeMail(MailID:{}, TransID:{}) .\n", cmd.mailid, cmd.transid );

    return 0;
}

int32_t CommandHandler::composeMail( sid_t sid, HttpRequest * request )
{
    UnitID roleid = std::atoll( (*request)["roleid"].c_str() );

    if ( roleid == 0 ) {
        return eHttpResult_MailReceiverInvalid;
    }

    if ( !GameApp::instance().isComposeMail() ) {
        return eHttpResult_ComposeTooFast;
    }

    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    ComposeGMMailCommand cmd;
    cmd.transid = trans->getTransID();
    cmd.roleid = roleid;
    cmd.mailid = std::atoi( (*request)["id"].c_str() );
    cmd.tag = std::atoi( (*request)["tag"].c_str() );
    cmd.keepdays = std::atoi( (*request)["keepdays"].c_str() );
    cmd.sendername = (*request)["sendername"];
    cmd.title = (*request)["title"];
    cmd.content = (*request)["content"];
    cmd.paramlist = (*request)["paramlist"];
    utils::JsonReader( (*request)["attachment"] ).get( cmd.attachments );
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_TRACE( "CommandHandler::composeMail(MailID:{}, RoleID:{}, Title:'{}', TransID:{}) .\n",
            cmd.mailid, cmd.roleid, cmd.title, cmd.transid );

    return 0;
}

int32_t CommandHandler::getRankboard( sid_t sid, HttpRequest * request )
{
    // TODO:
    return -1;
}

int32_t CommandHandler::updateRank( sid_t sid, HttpRequest * request )
{
    // TODO:
    return -1;
}

int32_t CommandHandler::sendBulletin( sid_t sid, HttpRequest * request )
{
    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    SendBulletinCommand cmd;
    cmd.transid = trans->getTransID();
    cmd.bulletinid = atoi( (*request)["id"].c_str() );
    cmd.type = BulletinType::GameMaster;
    cmd.style = atoi( (*request)["style"].c_str() );
    cmd.showplace = atoi( (*request)["showplace"].c_str() );
    cmd.priority = atoi( (*request)["priority"].c_str() );
    cmd.cycletimes = atoi( (*request)["cycletimes"].c_str() );
    cmd.interval = atoi( (*request)["interval"].c_str() );
    cmd.content = (*request)["content"];
    {
        utils::JsonReader doc( ( *request )["paramlist"] );
        doc.get( cmd.msgparamlist );
    }
    cmd.sendperiod.first = atoll( (*request)["starttime"].c_str() );
    cmd.sendperiod.second = atoll( (*request)["endtime"].c_str() );
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_TRACE( "CommandHandler::sendBulletin(BulletinID:{}, TransID:{}, {}~{}) .\n", cmd.bulletinid, cmd.transid, cmd.sendperiod.first, cmd.sendperiod.second );

    return 0;
}

int32_t CommandHandler::removeBulletin( sid_t sid, HttpRequest * request )
{
    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );

    RemoveBulletinCommand cmd;
    cmd.transid = trans->getTransID();
    cmd.bulletinid = atoi( (*request)["id"].c_str() );
    g_HarborService->broadcast( HostType::World, &cmd );
    LOG_TRACE( "CommandHandler::removeBulletin(BulletinID:{}, TransID:{}) .\n", cmd.bulletinid, cmd.transid );

    return 0;
}

int32_t CommandHandler::payCallback( sid_t sid, HttpRequest * request )
{
    // 世界服处理
    PayCallbackCommand cmd;
    cmd.order = (*request)["order"];
    cmd.account = "";
    cmd.token = "";
    ZoneID zoneid = std::atoi( (*request)["zoneid"].c_str() );

    // 区服错误
    if ( zoneid != g_AppConfig.getZoneID() ) {
        return eHttpResult_PayCallbackZoneError;
    }

    // 账单号错误
    if ( cmd.order.empty() ) {
        return eHttpResult_PayCallbackOrderError;
    }

    // 事务
    ResultTransaction * trans = new ResultTransaction();
    if ( trans == nullptr ) {
        return eHttpResult_OutofMemory;
    }

    trans->setSid( sid );
    trans->setVersion( request->getVersion() );
    trans->setConnection( (*request)[HttpMessage::HEADER_CONNECTION] );
    g_TransactionManager->append( trans, 60 );
    cmd.transid = trans->getTransID();

    // 发送到世界服务器
    g_HarborService->broadcast( HostType::World, &cmd );

    LOG_TRACE( "CommandHandler::payCallback:(Order:'{}', ZoneID:{}) .\n", cmd.order, zoneid );

    return 0;
}

int32_t CommandHandler::payRefundCallback( sid_t sid, HttpRequest * request )
{
    PayRefundCallbackCommand cmd;
    utils::JsonReader( (*request)["orderlist"] ).get( cmd.orderlist );
    cmd.token = "";
    ZoneID zoneid = std::atoi( (*request)["zoneid"].c_str() );

    // 区服错误
    if ( zoneid != g_AppConfig.getZoneID() ) {
        return eHttpResult_PayRefundZoneError;
    }

    // 账单列表为空
    if ( cmd.orderlist.empty() ) {
        return eHttpResult_PayRefundOrderlistEmpty;
    }

    //发送到世界服务器
    g_HarborService->broadcast( HostType::World, &cmd );

    // http返回
    utils::JsonWriter doc;
    doc.start();
    doc.put( "result", 0 );
    doc.end();
    g_HttpService->sendContent( sid, request, __BUILD_VERSION__, doc.tostring() );

    LOG_TRACE( "CommandHandler::payRefundCallback(ZoneID:{}) : the Refund OrderList:'{}' .\n", zoneid, (*request)["orderlist"] );

    return 0;
}

} // namespace master
