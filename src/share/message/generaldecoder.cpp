
#include "protocol.h"
#include "dbprotocol.h"
#include "gmprotocol.h"
#include "gameprotocol.h"
#include "gateprotocol.h"

uint16_t GeneralDecoder::get( MessageID cmd ) const
{
    auto result = m_MessageMap.find( cmd );
    if ( result != m_MessageMap.end() ) {
        return result->second;
    }
    return 0;
}

SSMessage * GeneralDecoder::decode( sid_t sid, const SSHead & head, const Slice & body )
{
    SSMessage * msg = nullptr;

    switch ( head.type ) {
        case eMessageType_Transfer: msg = new TransferMessage(); break;
        case eMessageType_Traverse: msg = new TraverseMessage(); break;
        case eMessageType_Response: msg = new ResponseMessage(); break;
        case eMessageType_Broadcast: msg = new BroadcastMessage(); break;
        case eMessageType_Command: {
            auto result = m_MessagePrototype.find( head.cmd );
            if ( result != m_MessagePrototype.end() ) {
                msg = result->second();
            }
        } break;
    }

    if ( msg != nullptr ) {
        msg->sid = sid;
        msg->head = head;

        if ( msg->decode( body ) ) {
            return msg;
        } else {
            delete msg;
            msg = nullptr;
        }
    }

    return msg;
}

void GeneralDecoder::add( MessageID cmd, const std::function<SSMessage *()> & func, uint16_t mask )
{
    m_MessagePrototype.emplace( cmd, func );

    if ( mask != 0 ) {
        m_MessageMap.emplace( cmd, mask );
    }
}

GeneralDecoder::GeneralDecoder()
{
    ///////////////////////////////////////////////////////////////////////////

    add( eSSCommand_Ping, [=] { return new PingCommand(); } );
    add( eSSCommand_Register, [=] { return new RegisterCommand(); }  );
    add( eSSCommand_RegSucceed, [=] { return new RegSucceedCommand(); }  );
    add( eSSCommand_Unregister, [=] { return new UnregisterCommand(); }  );
    add( eSSCommand_Ntpdate, [=] { return new NtpdateCommand(); }  );
    add( eSSCommand_Framework, [=] { return new FrameworkCommand(); }  );
    add( eSSCommand_Reload, [=] { return new ReloadCommand(); }  );
    add( eSSCommand_ShellCommand, [=] { return new ShellCommand(); }, eMessageDomain_Role );
    add( eSSCommand_RoleStatus, [=] { return new RoleStatusCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ServerStatus, [=] { return new ServerStatusCommand(); }  );
    add( eSSCommand_DebugAccount, [=] { return new DebugAccountCommand(); }  );
    add( eSSCommand_Skipday, [=] { return new SkipdayCommand(); }  );
    add( eSSCommand_Save2Document, [=] { return new Save2DocumentCommand(); }  );

    ///////////////////////////////////////////////////////////////////////////

    add( eSSCommand_Kickout, [=] { return new KickoutCommand(); }  );
    add( eSSCommand_KickoutAck, [=] { return new KickoutAckCommand(); }  );
    add( eSSCommand_Logout, [=] { return new LogoutCommand(); }  );
    add( eSSCommand_JoinChannel, [=] { return new JoinChannelCommand(); }  );
    add( eSSCommand_QuitChannel, [=] { return new QuitChannelCommand(); }  );
    add( eSSCommand_RegTunnel, [=] { return new RegTunnelCommand(); }  );
    add( eSSCommand_UnregTunnel, [=] { return new UnregTunnelCommand(); }  );
    add( eSSCommand_Credential, [=] { return new CredentialCommand(); }  );
    add( eSSCommand_SyncOnlineStatus, [=] { return new SyncOnlineStatusCommand(); }  );
    add( eSSCommand_RoleList, [=] { return new SyncRoleListCommand(); }  );
    add( eSSCommand_ReportLatency, [=] { return new ReportLatencyCommand(); }  );
    add( eSSCommand_SyncOnlineCount, [=] { return new SyncOnlineCountCommand(); }  );

    ///////////////////////////////////////////////////////////////////////////

    add( eSSCommand_DBHandShake, [=] { return new DBHandShakeCommand(); }  );
    add( eSSCommand_DBInsert, [=] { return new DBInsertCommand(); }  );
    add( eSSCommand_DBQuery, [=] { return new DBQueryCommand(); }  );
    add( eSSCommand_DBUpdate, [=] { return new DBUpdateCommand(); }  );
    add( eSSCommand_DBRemove, [=] { return new DBRemoveCommand(); }  );
    add( eSSCommand_DBReplace, [=] { return new DBReplaceCommand(); }  );
    add( eSSCommand_DBQueryBatch, [=] { return new DBQueryBatchCommand(); }  );
    add( eSSCommand_DBInvoke, [=] { return new DBInvokeCommand(); }  );
    add( eSSCommand_DBFlush, [=] { return new DBFlushCommand(); }  );
    add( eSSCommand_DBResults, [=] { return new DBResultsCommand(); }  );
    add( eSSCommand_DBAutoIncrease, [=] { return new DBAutoIncreaseCommand(); }  );
    add( eSSCommand_DBException, [=] { return new DBExceptionCommand(); }  );
    add( eSSCommand_DBFailover, [=] { return new DBFailoverCommand(); }  );
    add( eSSCommand_CreateBinFile, [=] { return new CreateBinFileCommand(); }  );
    add( eSSCommand_SyncBinLog, [=] { return new SyncBinFileCommand(); }  );
    add( eSSCommand_DeleteBinFile, [=] { return new DeleteBinFileCommand(); }  );

    ///////////////////////////////////////////////////////////////////////////

    add( eSSCommand_SwitchScene, [=] { return new SwitchSceneCommand(); }  );
    add( eSSCommand_RoleNewborn, [=] { return new RoleNewbornCommand(); }, eMessageDomain_Role );
    add( eSSCommand_SendMail, [=] { return new SendMailCommand(); }, eMessageDomain_Mail );
    add( eSSCommand_RegisterZone, [=] { return new RegisterZoneCommand(); }  );
    add( eSSCommand_CharBrief, [=] { return new CharBriefCommand(); }  );
    add( eSSCommand_UpdateRank, [=] { return new UpdateRankCommand(); } );
    add( eSSCommand_SyncRoleProps, [=] { return new SyncRolePropsCommand(); }, eMessageDomain_Role );
    add( eSSCommand_SyncMainProgress, [=] { return new SyncMainProgressCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ChangeRoleProps, [=] { return new ChangeRolePropsCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ChangeRolename, [=] { return new ChangeRolenameCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_SubscribeQuest, [=] { return new SubscribeQuestCommand(); }  );
    add( eSSCommand_DispatchEvent, [=] { return new DispatchEventCommand(); }, eMessageDomain_Role );
    add( eSSCommand_DataException, [=] { return new DataExceptionCommand(); }  );
    add( eSSCommand_CreateArea, [=] { return new CreateAreaCommand(); }, eMessageDomain_Battle );
    add( eSSCommand_ResultReport, [=] { return new ResultReportCommand(); }, eMessageDomain_Battle );
    add( eSSCommand_OpenSystemList, [=] { return new OpenSystemListCommand(); }, eMessageDomain_Role );
    add( eSSCommand_Pay, [=] { return new PayCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_PayCallback, [=] { return new PayCallbackCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_PayRefund, [=] { return new PayRefundCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_PayRefundCallback, [=] { return new PayRefundCallbackCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_SyncBilling, [=] { return new SyncBillingCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_ChatInWorld, [=] { return new ChatInWorldCommand(); }  );
    add( eSSCommand_SyncAttribute, [=] { return new SyncAttributeCommand(); }, eMessageDomain_Role  );
    add( eSSCommand_QueryAttribute, [=] { return new QueryAttributeCommand(); }, eMessageDomain_Role  );
    add( eSSCommand_ArenaChallenge, [=] { return new ArenaChallengeCommand(); }, eMessageDomain_Arena );
    add( eSSCommand_ArenaInfo, [=] { return new ArenaInfoCommand(); }, eMessageDomain_Arena );
    add( eSSCommand_ArenaRankReward, [=] { return new ArenaRankRewardCommand(); }, eMessageDomain_Arena );
    add( eSSCommand_ArenaSettle, [=] { return new ArenaSettleCommand(); }, eMessageDomain_Arena  );
    add( eSSCommand_ArenaHighLightsReward, [=] { return new ArenaHighLightsRewardCommand(); }, eMessageDomain_Arena );

    ///////////////////////////////////////////////////////////////////////////

    add( eSSCommand_GMResult, [=] { return new GMResultCommand(); }  );
    add( eSSCommand_QueryRole, [=] { return new QueryRoleCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ModifyRole, [=] { return new ModifyRoleCommand(); }, eMessageDomain_Role );
    add( eSSCommand_FlushRole, [=] { return new FlushRoleCommand(); } );
    add( eSSCommand_Shutup, [=] { return new ShutupCommand(); } );
    add( eSSCommand_Banned, [=] { return new BannedCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ActivateRole, [=] { return new ActivateRoleCommand(); }, eMessageDomain_Role );
    add( eSSCommand_SendGMMail, [=] { return new SendGMMailCommand(); }, eMessageDomain_Mail );
    add( eSSCommand_RemoveGMMail, [=] { return new RemoveGMMailCommand(); }, eMessageDomain_Mail );
    add( eSSCommand_ComposeGMMail, [=] { return new ComposeGMMailCommand(); }, eMessageDomain_Mail  );
    add( eSSCommand_SendBulletin, [=] { return new SendBulletinCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_RemoveBulletin, [=] { return new RemoveBulletinCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_UseGiftCode, [=] { return new UseGiftCodeCommand(); }  );
    add( eSSCommand_Login2Account, [=] { return new Login2AccountCommand(); }  );
    add( eSSCommand_AcquireRoleList, [=] { return new AcquireRoleListCommand(); }, eMessageDomain_Role );
    add( eSSCommand_ModifyRolename, [=] { return new ModifyRolenameCommand(); }, eMessageDomain_Utils );
    add( eSSCommand_GetRankboard, [=] { return new GetRankboardCommand(); } );
    add( eSSCommand_UpdateZoneStatus, [=] { return new UpdateZoneStatusCommand(); } );

    ///////////////////////////////////////////////////////////////////////////
}

GeneralDecoder::~GeneralDecoder()
{
    m_MessageMap.clear();
    m_MessagePrototype.clear();
}
