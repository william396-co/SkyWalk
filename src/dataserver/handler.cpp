
#include "base/base.h"
#include "base/limits.h"

#include "utils/json.h"
#include "utils/types.h"
#include "utils/streambuf.h"

#include "io/slave.h"
#include "mysql/result.h"
#include "lua/kaguya.hpp"
#include "scheme/sqlbind.h"
#include "extra/ticksvc.h"
#include "extra/masterproc.h"
#include "message/protocol.h"
#include "message/dbprotocol.h"

#include "config.h"
#include "logger.h"
#include "dbthreads.h"
#include "tablecache.h"
#include "transaction.h"

#include "server.h"

#include "handler.h"

namespace data {

void CommandHandler::process( SSMessage * msg )
{
    switch ( msg->head.cmd ) {
        case eSSCommand_DBHandShake: handshake( msg ); break;
        case eSSCommand_DBQuery: query( msg ); break;
        case eSSCommand_DBQueryBatch: queryBatch( msg ); break;
        case eSSCommand_DBFlush: flush( msg ); break;
        case eSSCommand_ServerStatus: status( msg ); break;
        case eSSCommand_Ntpdate: ntpdate( msg ); break;
        case eSSCommand_DBFailover: failover( msg ); break;
        case eSSCommand_DBInsert: g_BinLogger->append( (DBInsertCommand *)msg ); insert( msg ); break;
        case eSSCommand_DBUpdate: g_BinLogger->append( (DBUpdateCommand *)msg ); update( msg ); break;
        case eSSCommand_DBRemove: g_BinLogger->append( (DBRemoveCommand *)msg ); remove( msg ); break;
        case eSSCommand_DBReplace: g_BinLogger->append( (DBReplaceCommand *)msg ); replace( msg ); break;
        case eSSCommand_DBInvoke: g_BinLogger->append( (DBInvokeCommand *)msg ); invoke( msg ); break;
    }
}

void CommandHandler::doAutoIncrease( TransID transid, const std::string & lastid )
{
    DataAutoIncrementTrans * trans = (DataAutoIncrementTrans *)g_TransManager->get( transid );
    if ( unlikely( trans == nullptr ) ) {
        LOG_ERROR( "CommandHandler::doAutoIncrease(TransID:{}) : can't found the Transaction .\n", transid );
        return;
    }

    ISQLData * data = trans->getDataBlock();

    // 解析获取lastid
    data->autoincrease( 1, lastid );

    // 处理插入
    DataBlock * block = CommandHandler::realInsertCache( data );
    if ( block != nullptr ) {
        // 返回给客户端
        DBAutoIncreaseCommand command;
        command.table = data->tablename();
        command.transid = trans->getCliTransID();
        command.indexstr = block->getIdxString();
        command.lastid = Slice( lastid );
        g_HarborService->send( trans->getSid(), &command );
    }

    // 销毁事务
    delete trans;
}

void CommandHandler::doDataException( const Tablename & tablename, const std::string & indexstring )
{
    TableCache * cache = GameApp::instance().getCache( tablename );
    if ( cache == nullptr ) {
        LOG_ERROR( "CommandHandler::doDataException(Table:'{}', IndexString:'{}') : Tablename is INVALID .\n", tablename, indexstring );
        return;
    }

    cache->throwException( indexstring );
    LOG_TRACE( "CommandHandler::doDataException() : add IndexString:'{}' to ExceptionTable(Tablename:'{}') .\n", indexstring, tablename );
}

void CommandHandler::doQueryResults( TransID transid, const std::vector<ISQLData *> & blocks )
{
    int64_t now = utils::now();

    DataResultTrans * trans = (DataResultTrans *)g_TransManager->get( transid );
    if ( unlikely( trans == nullptr ) ) {
        LOG_ERROR( "CommandHandler::doQueryResults(TransID:{}) : can't found the Transaction .\n", transid );
        return;
    }

    // 没有指定索引的情况下, 应该就是查询的是整张表
    // 在这种情况下，根据查询到的数据自动建立缓存
    std::vector<std::string> execkeys;
    bool is_whole_table = trans->getIdxString().empty();
    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( trans->getTable() );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::doQueryResults(Table:'{}', IndexString:'{}') : Tablename is INVALID .\n", trans->getTable(), trans->getIdxString() );
        delete trans;
        return;
    }

    // 创建索引条目
    IndexEntry * queryentry = cache->createEntry( trans->getIdxString() );
    assert( queryentry != nullptr && "create IndexEntry failed" );

    // 销毁内存块
    for ( auto data : blocks ) {
        IndexEntry * entry = queryentry;

        // 主键字符串
        std::string keystr, idxstr;
        data->keystring( keystr );
        data->indexstring( eCodec_All, idxstr );

        // 到缓存中查找是否有数据
        DataBlock * datablock = cache->getBlock( keystr );
        if ( unlikely( datablock != nullptr ) ) {
            // 优先使用内存中的数据块
            // data 应该是当前数据库中的脏数据
            delete data;
        } else {
            datablock = cache->createBlock( data, keystr );
            assert( datablock != nullptr && "create DataBlock failed" );
        }

        if ( !cache->isEmpty( keystr ) ) {
            execkeys.push_back( keystr );
        }

        // 当前数据块没有索引缓存的情况下, 不用建立索引缓存
        if ( !idxstr.empty() ) {
            // 如果查询的是整张表的话, 建立当前数据块的索引缓存
            if ( is_whole_table ) {
                entry = cache->createEntry( idxstr );
                if ( entry != nullptr ) {
                    entry->setStatus( IndexEntry::eStatus_Complete );
                }
            }
            // 添加数据主键
            if ( entry != nullptr ) entry->add( keystr );
        }
    }

    // 查询索引条目已经完整
    if ( queryentry != nullptr ) {
        queryentry->setStatus( IndexEntry::eStatus_Complete );
    }

    // 处理缓存中在消息队列中的消息
    for ( const auto & key : execkeys ) {
        CommandHandler::doMessageQueue( cache, key );
    }

    LOG_DEBUG( "CommandHandler::doQueryResults(Table:'{}', IndexString:'{}', TransID:{}) : build the {} Record(s) to DataCache succeed, Elapsed:{}/{}(msecs) .\n",
        trans->getTable(), trans->getIdxString(), trans->getCliTransID(), blocks.size(), utils::now() - now, utils::now() - trans->getStartTimestamp() );
    // 销毁事务
    delete trans;
}

void CommandHandler::doQueryDatablock( TransID transid, const Tablename & table, const std::vector<std::vector<std::string>> & results )
{
    int64_t now = utils::now();

    DatablockTrans * trans = (DatablockTrans *)g_TransManager->get( transid );
    if ( unlikely( trans == nullptr ) ) {
        LOG_ERROR( "CommandHandler::doQueryDatablock(TransID:{}, Table:'{}') : can't found the Transaction .\n", transid, table );
        return;
    }

    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( trans->getTable() );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::doQueryDatablock(TransID:{}, Table:'{}', KeyString:'{}') : Tablename is INVALID .\n",
            transid, table, trans->getKeyString() );
        delete trans;
        return;
    }

    // 确保数据个数是正确的 ( 1 or 0 )
    assert( results.empty() || results.size() == 1 );

    for ( const auto & result : results ) {
        Slices row;
        std::string keystr, idxstr;
        ISQLData * data = g_DataPrototype->data( table );
        if ( unlikely( data == nullptr ) ) {
            LOG_ERROR( "CommandHandler::doQueryDatablock(TransID:{}, Table:'{}') : can't found this Data Prototype \n", transid, table );
            continue;
        }

        // 转换Slices
        for ( const auto & slice : result ) {
            row.push_back( Slice( slice ) );
        }
        // 解析数据结果
        data->parse( row );
        data->keystring( keystr );
        data->indexstring( eCodec_All, idxstr );
        assert( keystr == trans->getKeyString() && idxstr == trans->getIdxString() );

        // 数据
        DataBlock * datablock = cache->getBlock( keystr );
        if ( unlikely( datablock != nullptr ) ) {
            // 优先使用内存中的数据块
            // data 应该是当前数据库中的脏数据
            delete data;
        } else {
            datablock = cache->createBlock( data, keystr );
            assert( datablock != nullptr && "create DataBlock failed" );
        }
        // 索引
        if ( !idxstr.empty() ) {
            IndexEntry * entry = cache->createEntry( idxstr );
            if ( entry != nullptr ) {
                entry->add( keystr );
            }
        }
    }

    // 处理缓存中在消息队列中的消息
    CommandHandler::doMessageQueue( cache, trans->getKeyString() );

    LOG_DEBUG( "CommandHandler::doQueryDatablock(Table:'{}', KeyString:'{}', TransID:{}) : build the {} Record(s) to DataCache succeed, Elapsed:{}/{}(msecs) .\n",
        trans->getTable(), trans->getKeyString(), trans->getTransID(), results.size(), utils::now() - now, utils::now() - trans->getStartTimestamp() );
    delete trans;
}

void CommandHandler::handshake( SSMessage * msg )
{
    auto req = (DBHandShakeCommand *)msg;

    DBHandShakeCommand resp;
    // TODO: slave机器的信息
    resp.version = g_DataPrototype->schemeversion();
    g_HarborService->send( req->sid, &resp );

    LOG_DEBUG( "CommandHandler::handshake(Sid:{}) : check the SchemeVersion(PROXY:'{}', AGENT:'{}') ...\n",
        req->sid, g_DataPrototype->schemeversion(), req->version );

    if ( g_DataPrototype->schemeversion() != req->version ) {
        LOG_ERROR( "CommandHandler::handshake(Sid:{}) : SchemeVersion(PROXY:'{}', AGENT:'{}') is NOT-MATCHED .\n",
            req->sid, g_DataPrototype->schemeversion(), req->version );

        // 踢出该客户端
        g_HarborService->shutdown( req->sid );
    }
}

void CommandHandler::insert( SSMessage * msg )
{
    auto req = (DBInsertCommand *)msg;

    // 根据table获取数据原型
    ISQLData * data = g_DataPrototype->data( req->table );
    if ( unlikely( data == nullptr ) ) {
        LOG_ERROR( "CommandHandler::insert(TransID:{}, Table:'{}') : can't found this Data Prototype .\n",
            req->transid, req->table );
        return;
    }

    // 解析数据
    std::string idxstr;
    data->decode( req->dbdata, eCodec_Dirty );
    data->indexstring( eCodec_All, idxstr );

    if ( likely( req->transid == 0 ) ) {
        // 数据不需要依赖AutoIncrease
        DataBlock * block = CommandHandler::realInsertCache( data );
        if ( block != nullptr ) {
            // 启动定时器
            block->schedule( g_AppConfig.getFlushInterval() * 1000 );
        }
    } else {
        // 主键字符串以及索引字符串
        std::string keystr;
        data->keystring( keystr );

        // 拼接SQL语句, 提交数据库
        std::string sqlcmd;
        std::vector<std::string> escapevalues;
        data->insert( sqlcmd, escapevalues );
        // 数据需要从数据库中获得自增的字段
        DataAutoIncrementTrans * t = new DataAutoIncrementTrans();
        assert( t != nullptr && "create DataAutoIncreaseTrans failed" );
        t->setSid( req->sid );
        t->setDataBlock( data );
        t->setCliTransID( req->transid );
        g_TransManager->append( t, req->timeout );
        g_DBThreads->post( req->table, idxstr, DBMethod::Insert, t->getTransID(), std::move(sqlcmd), std::move(escapevalues) );
    }

    // 通知DataAgent数据异常
    TableCache * cache = GameApp::instance().getCache( req->table );
    if ( cache != nullptr ) {
        if ( cache->isException( idxstr ) ) {
            DBExceptionCommand cmd;
            cmd.table = req->table;
            cmd.indexstr = idxstr;
            cmd.dbdata = req->dbdata.ToString();
            g_HarborService->send( req->sid, &cmd );
        }
    }
}

void CommandHandler::query( SSMessage * msg )
{
    auto req = (DBQueryCommand *)msg;

    CommandHandler::realQuery( req->sid,
        req->head.cmd, req->transid, req->table, req->indexstr, req->dbdata, req->timeout );
}

void CommandHandler::update( SSMessage * msg, int32_t mode )
{
    auto req = (DBUpdateCommand *)msg;

    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( req->table );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::update(Table:'{}', KeyString:'{}') : Tablename is INVALID .\n", req->table, req->keystr );
        return;
    }

    // 获取数据块
    DataBlock * block = cache->getBlock( req->keystr );
    if ( block != nullptr ) {
        // 解析变更列表
        block->updateMethod( DBMethod::Update );
        block->getDataBlock()->decode( req->dbdata, eCodec_Dirty );
        // 启动定时器
        block->schedule( g_AppConfig.getFlushInterval() * 1000 );

        // 通知DataAgent数据异常
        if ( cache->isException( block->getIdxString() ) ) {
            DBExceptionCommand cmd;
            cmd.table = req->table;
            cmd.dbdata = req->dbdata.ToString();
            cmd.indexstr = block->getIdxString();
            g_HarborService->send( req->sid, &cmd );
        }
    } else {
        if ( mode == 0 ) {
            // 首次update, 内存中无数据
            // 检查离线消息队列是否为空
            //      否, 加入离线消息队列, 等待数据块返回
            //      是, 查询数据库, 并加入离线消息队列
            if ( cache->isEmpty( req->keystr ) ) {
                if ( !CommandHandler::queryDatablock( req->table, req->keystr, req->dbdata ) ) {
                    LOG_ERROR( "CommandHandler::update(Table:'{}', KeyString:'{}') : can't launch a Queryer, update the Datablock failed .\n",
                        req->table, req->keystr );
                    return;
                }
            }
            cache->pushMessage( req->keystr, req->clone( true ) );
        } else {
            // 确认数据库中没有该数据
            LOG_ERROR( "CommandHandler::update(Table:'{}', KeyString:'{}') : this DataBlock is not exist in the Database .\n",
                req->table, req->keystr );
        }
    }
}

void CommandHandler::remove( SSMessage * msg, int32_t mode )
{
    auto req = (DBRemoveCommand *)msg;

    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( req->table );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::remove(Table:'{}', Key:'{}') : Tablename is INVALID .\n", req->table, req->keystr );
        return;
    }

    DataBlock * block = cache->getBlock( req->keystr );
    if ( block != nullptr ) {
        std::string idxstr = block->getIdxString();

        // 从索引缓存中删除数据
        if ( !block->getIdxString().empty() ) {
            IndexEntry * entry = cache->getEntry( block->getIdxString() );
            if ( entry != nullptr ) {
                // 删除主键
                entry->del( req->keystr );
                // 删除索引
                if ( entry->datakeys().empty() ) {
                    delete entry;
                    cache->removeEntry( block->getIdxString() );
                }
            }
        }

        // 刷新到数据库中
        block->updateMethod( DBMethod::Remove );

        // 强行落地
        block->flush2DB();
        // 删除数据块
        cache->destroyBlock( req->keystr, block );

        // 通知DataAgent数据异常
        if ( cache->isException( idxstr ) ) {
            DBExceptionCommand cmd;
            cmd.table = req->table;
            cmd.indexstr = idxstr;
            cmd.dbdata = req->dbdata.ToString();
            g_HarborService->send( req->sid, &cmd );
        }
    } else {
        if ( mode == 0 ) {
            // 首次remove, 内存中无数据
            // 检查离线消息队列是否为空
            //      否, 加入离线消息队列, 等待数据块返回
            //      是, 查询数据库, 并加入离线消息队列
            if ( cache->isEmpty( req->keystr ) ) {
                if ( !CommandHandler::queryDatablock( req->table, req->keystr, req->dbdata ) ) {
                    LOG_ERROR( "CommandHandler::remove(Table:'{}', KeyString:'{}') : can't launch a Queryer, remove the Datablock failed .\n",
                        req->table, req->keystr );
                    return;
                }
            }
            cache->pushMessage( req->keystr, req->clone( true ) );
        } else {
            // 确认数据库中没有该数据
            LOG_ERROR( "CommandHandler::remove(Table:'{}', KeyString:'{}') : this DataBlock is not exist in the Database .\n",
                req->table, req->keystr );
        }
    }
}

void CommandHandler::replace( SSMessage * msg, int32_t mode )
{
    auto req = (DBReplaceCommand *)msg;

    if ( req->transid != 0 && mode != 0 ) {
        // 自增索引的情况下
        // 只有首次replace才会处理
        return;
    }

    TableCache * cache = GameApp::instance().getCache( req->table );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::replace(Transid:{}, Table:'{}') : Tablename is INVALID .\n",
            req->transid, req->table );
        return;
    }

    // 根据table获取数据原型
    ISQLData * data = g_DataPrototype->data( req->table );
    if ( unlikely( data == nullptr ) ) {
        LOG_ERROR( "CommandHandler::replace(TransID:{}, Table:'{}') : can't found this Data Prototype .\n",
            req->transid, req->table );
        return;
    }

    // 解析数据
    std::string idxstr, keystr;
    data->decode( req->dbdata, eCodec_Dirty );
    // 主键字符串以及索引字符串
    data->keystring( keystr );
    data->indexstring( eCodec_All, idxstr );

    // 通知DataAgent数据异常
    if ( cache->isException( idxstr ) ) {
        DBExceptionCommand cmd;
        cmd.table = req->table;
        cmd.indexstr = idxstr;
        cmd.dbdata = req->dbdata.ToString();
        g_HarborService->send( req->sid, &cmd );
    }

    if ( unlikely( req->transid != 0 ) ) {
        // 数据需要从数据库中获得自增的字段

        // 添加事务
        DataAutoIncrementTrans * t = new DataAutoIncrementTrans();
        assert( t != nullptr && "create DataAutoIncreaseTrans failed" );
        t->setSid( req->sid );
        t->setDataBlock( data );
        t->setCliTransID( req->transid );
        g_TransManager->append( t, req->timeout );
        // 拼接SQL语句, 提交数据库
        std::string sqlcmd;
        std::vector<std::string> escapevalues;
        data->replace( sqlcmd, escapevalues );
        g_DBThreads->post( req->table, idxstr, DBMethod::Replace, t->getTransID(), std::move(sqlcmd), std::move(escapevalues) );
    } else {
        // 数据不需要依赖AutoIncrease

        DataBlock * block = cache->getBlock( keystr );
        if ( block != nullptr ) {
            delete data;

            // 数据存在内存中, 更新之
            block->updateMethod( DBMethod::Replace );
            block->getDataBlock()->decode( req->dbdata, eCodec_Dirty );
            block->schedule( g_AppConfig.getFlushInterval() * 1000 );
        } else {
            // 索引缓存是完整的
            // 数据块不存在, 说明数据是可以插入成功的
            // 确保索引缓存的完整性, 插入数据, 并且更新索引缓存
            IndexEntry * entry = cache->getEntry( idxstr );
            if ( entry != nullptr
                && entry->getStatus() == IndexEntry::eStatus_Complete ) {
                assert( !entry->has( keystr ) );

                // 创建数据块
                block = cache->createBlock( data, keystr );
                assert( block != nullptr && "create DataBlock failed" );
                // 添加到索引缓存中
                if ( !idxstr.empty() ) entry->add( keystr );
                // 更新为替换数据
                block->updateMethod( DBMethod::Replace );
                block->schedule( g_AppConfig.getFlushInterval() * 1000 );
            } else {
                if ( mode == 0 ) {
                    // 首次replace, 内存中无数据
                    // 检查离线消息队列是否为空
                    //      否, 加入离线消息队列, 等待数据块返回
                    //      是, 查询数据库, 并加入离线消息队列
                    if ( cache->isEmpty( keystr ) ) {
                        CommandHandler::queryDatablock( data, keystr, idxstr );
                    }
                    delete data;
                    cache->pushMessage( keystr, req->clone( true ) );
                } else {
                    // 再次replace, 内存中依然没有数据
                    // 这种情况直接Insert数据块到内存中
                    DataBlock * block = CommandHandler::realInsertCache( data );
                    if ( block != nullptr ) {
                        block->schedule( g_AppConfig.getFlushInterval() * 1000 );
                    }
                }
            }
        }
    }
}

void CommandHandler::queryBatch( SSMessage * msg )
{
    auto req = (DBQueryBatchCommand *)msg;

    for ( const auto & query : req->querylist ) {
        CommandHandler::realQuery( req->sid,
            req->head.cmd, req->transid, query.table, query.indexstr, query.dbdata, req->timeout );
    }
}

void CommandHandler::invoke( SSMessage * msg, int32_t mode )
{
    auto req = (DBInvokeCommand *)msg;

    TableCache * cache = GameApp::instance().getCache( req->table );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::invoke(Table:'{}', KeyString:'{}') : Tablename is INVALID .\n", req->table, req->keystr );
        return;
    }

    // 获取数据块
    DataBlock * block = cache->getBlock( req->keystr );
    if ( block != nullptr ) {
        LOG_TRACE( "CommandHandler::invoke(Table:'{}', KeyString:'{}') : this DataBlock() invoke a SCRIPT('{}') .\n",
            req->table, req->keystr, req->script );

        // 调用脚本
        block->updateMethod( DBMethod::Invoke );
        block->getDataBlock()->invoke(
            g_DataPrototype->state(), req->script );

        // 启动定时器
        block->schedule( g_AppConfig.getFlushInterval() * 1000 );

        // 伪造一份BINLOG
        DBUpdateCommand command;
        command.table = req->table;
        command.keystr = req->keystr;
        command.dbdata = block->getDataBlock()->encode( eCodec_All );
        g_BinLogger->append<DBUpdateCommand>( &command );

        // 通知DataAgent数据异常
        if ( cache->isException( block->getIdxString() ) ) {
            DBExceptionCommand cmd;
            cmd.table = req->table;
            cmd.script = req->script;
            cmd.indexstr = block->getIdxString();
            cmd.dbdata = req->dbdata.ToString();
            g_HarborService->send( req->sid, &cmd );
        }
    } else {
        if ( mode == 0 ) {
            // 首次invoke, 内存中无数据
            // 检查离线消息队列是否为空
            //      否, 加入离线消息队列, 等待数据块返回
            //      是, 查询数据库, 并加入离线消息队列
            if ( cache->isEmpty( req->keystr ) ) {
                if ( !CommandHandler::queryDatablock( req->table, req->keystr, req->dbdata ) ) {
                    LOG_ERROR( "CommandHandler::invoke(Table:'{}', KeyString:'{}') : can't launch a Queryer, invoke the Script:'{}' failed .\n",
                        req->table, req->keystr, req->script );
                    return;
                }
            }
            cache->pushMessage( req->keystr, req->clone( true ) );
        } else if ( req->isreplace ) {
            // 如果指定了replace参数, 直接插入数据
            ISQLData * data = g_DataPrototype->data( req->table );
            if ( unlikely( data == nullptr ) ) {
                LOG_ERROR( "CommandHandler::invoke(Table:'{}') : can't found this Data Prototype .\n", req->table );
            } else {
                // 解析数据
                data->decode( req->dbdata, eCodec_Dirty );
                DataBlock * block = CommandHandler::realInsertCache( data );
                if ( block != nullptr ) {
                    block->schedule( g_AppConfig.getFlushInterval() * 1000 );
                }
            }
        } else {
            // 确认数据库中没有该数据
            LOG_ERROR( "CommandHandler::invoke(Table:'{}', KeyString:'{}') : this DataBlock is not exist in the Database, invoke the Script:'{}' failed .\n",
                req->table, req->keystr, req->script );
        }
    }
}

void CommandHandler::flush( SSMessage * msg )
{
    auto req = (DBFlushCommand *)msg;

    for ( const auto & entry : req->entrylist ) {
        // 强制存档
        CommandHandler::flushIndexEntry( entry.table, req->dropflag, entry.indexstr );
    }
}

void CommandHandler::status( SSMessage * msg )
{
    auto req = (ServerStatusCommand *)msg;

    Tablename max_table;
    size_t max_count = 0;
    utils::JsonWriter exceptions;
    exceptions.start();
    for ( const auto & table : g_DataPrototype->tables() ) {
        TableCache * cache = GameApp::instance().getCache( table );
        if ( cache != nullptr ) {
            if ( cache->datasize() > max_count ) {
                max_table = table;
                max_count = cache->datasize();
            }
            if ( cache->getExceptionCount() != 0 ) {
                exceptions.put( table, cache->getExceptionCount() );
            }
        }
    }
    exceptions.end();

    utils::JsonWriter writer;
    writer.start();
    writer.put( "max_table", max_table );
    writer.put( "max_table_size", max_count );
    writer.put( "uptime", MASTER.uptime() );
    writer.put( "cputime", (int64_t)MASTER.cputime() );
    writer.put( "usedmemory", MASTER.getUseMemory() );
    writer.put( "schemeversion", g_DataPrototype->schemeversion() );
    writer.put( "exceptions", exceptions.tostring() );
    writer.end();

    ServerStatusCommand response;
    response.type = HostType::Data;
    response.transid = req->transid;
    response.id = g_AppConfig.getServerID();
    response.status = std::string( writer.content(), writer.length() );
    Slave::instance().send( &response );
}

void CommandHandler::ntpdate( SSMessage * msg )
{
    auto req = (NtpdateCommand *)msg;
    global::delta_timestamp = req->timestamp - utils::time();

    utils::TimeUtils adjust;
    std::string stradjust = adjust.getFormatTime( "%F %T" );

    LOG_INFO( "CommandHandler::ntpdate(DeltaSeconds:{}) : set CurTimestamp: {}, Now : {}, {} .\n",
        global::delta_timestamp, req->timestamp, adjust.getTimestamp(), stradjust );
}

void CommandHandler::failover( SSMessage * msg )
{
    auto req = (DBFailoverCommand *)msg;

    for ( const auto & except : req->exceptions ) {
        LOG_WARN( "CommandHandler::failover() : resolved this Exception(Table:'{}', IndexString:'{}') .\n",
            except.table, except.indexstr );

        TableCache * cache = GameApp::instance().getCache( except.table );
        if ( cache != nullptr ) {
            cache->fixException( except.indexstr );
        }

        g_DBThreads->post( except.table, except.indexstr, DBMethod::Failover, 0 );
    }
}

inline DataBlock * CommandHandler::realInsertCache( ISQLData * data )
{
    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( data->tablename() );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::realInsertCache(Table:'{}') : Tablename is INVALID .\n", data->tablename() );
        return nullptr;
    }

    // 获取主键字符串以及索引字符串
    std::string keystr, idxstr;
    data->keystring( keystr );
    data->indexstring( eCodec_All, idxstr );

    // 查询内存中是否有数据
    DataBlock * block = cache->getBlock( keystr );
    if ( block != nullptr ) {
        // 出错了, 重复的数据
        LOG_ERROR( "CommandHandler::realInsertCache(Table:'{}') : this DataBlock(KeyString:'{}', Method:{}) is In the DataCache .\n",
            data->tablename(), keystr, block->getMethod() );
        delete data;
        return nullptr;
    }

    // 创建数据块
    block = cache->createBlock( data, keystr );
    assert( block != nullptr && "create DataBlock failed" );

    // 更新为插入数据
    block->updateMethod( DBMethod::Insert );

    // 需要缓存索引
    if ( !idxstr.empty() ) {
        // 获取索引条目
        // 添加不完整索引
        IndexEntry * entry = cache->createEntry( idxstr );
        if ( entry != nullptr ) {
            // 添加到索引缓存中
            entry->add( keystr );
        }
    }

    return block;
}

void CommandHandler::flushIndexEntry( IndexEntry * entry, TableCache * cache, bool isdrop )
{
    size_t flushcount = 0;

    for ( const auto & key : entry->datakeys() ) {
        DataBlock * datablock = cache->getBlock( key );
        if ( datablock == nullptr ) {
            continue;
        }

        if ( datablock->getMethod() != DBMethod::None ) {
            ++flushcount;
            // 强行落地
            datablock->flush2DB();
        }

        if ( isdrop ) {
            cache->removeBlock( key );
            delete datablock;
        }
    }

    LOG_TRACE( "CommandHandler::flushIndexEntry(Table:'{}', IndexString:'{}', DROP:{}, TotalCount:{}, FlushCount:{}) .\n",
        entry->table(), entry->index(), isdrop, entry->datakeys().size(), flushcount );

    if ( isdrop ) {
        cache->removeEntry( entry->index() );
        delete entry;
    }
}

void CommandHandler::flushIndexEntry( const Tablename & table, int8_t dropflag, const std::string & indexstring )
{
    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( table );
    if ( cache == nullptr ) {
        LOG_ERROR( "CommandHandler::flushIndexEntry(Table:'{}', IndexString:'{}') : Tablename is INVALID .\n", table, indexstring );
        return;
    }

    IndexEntry * entry = cache->getEntry( indexstring );
    if ( entry == nullptr ) {
        // DataAgent刷新不存在的数据, 这种情况属于正常的
        LOG_WARN( "CommandHandler::flushIndexEntry(Table:'{}', IndexString:'{}') : can't found this IndexEntry in the IndexCache .\n", table, indexstring );
        return;
    }

    CommandHandler::flushIndexEntry( entry, cache, dropflag == 0 ? false : true );
}

inline void CommandHandler::realQuery( sid_t sid, MessageID cmd, TransID transid, const Tablename & table, const std::string & idxstr, const Slice & dbdata, uint32_t timeout )
{
    LOG_DEBUG( "CommandHandler::realQuery(Table:'{}', IndexString:'{}', Transid:{}) ...\n", table, idxstr, transid );

    // 获取缓存
    TableCache * cache = GameApp::instance().getCache( table );
    if ( unlikely( cache == nullptr ) ) {
        LOG_ERROR( "CommandHandler::realQuery(Table:'{}', IndexString:'{}') : Tablename is INVALID .\n", table, idxstr );
        return;
    }

    if ( cache->isException( idxstr ) ) {
        LOG_ERROR( "CommandHandler::realQuery(Table:'{}', IndexString:'{}') : drop this Request, in the ExceptionTable .\n", table, idxstr );
        return;
    }

    // 不存在, 根据table获取查询原型, 拼接SQL语句去数据库查询
    ISQLData * query = g_DataPrototype->data( table );
    if ( query == nullptr ) {
        LOG_ERROR( "CommandHandler::realQuery(TransID:{}, IndexString:'{}', Table:'{}') : can't found this Index Prototype \n", transid, idxstr, table );
        return;
    }

    // 解析
    query->decode( dbdata, eCodec_Query );

    // 找到的完整的索引缓存
    IndexEntry * entry = cache->getEntry( idxstr );
    if ( entry != nullptr
        && entry->getStatus() == IndexEntry::eStatus_Complete ) {
        // 数据结果
        DBResultsCommand command;
        command.table = table;
        command.indexstr = idxstr;
        command.transid = transid;
        if ( entry->index().empty() ) {
            // NOTICE: 整张表的情况
            cache->datasets( command.results, query );
        } else {
            // 单个索引的情况
            for ( const auto & key : entry->datakeys() ) {
                DataBlock * datablock = cache->getBlock( key );
                if ( datablock != nullptr
                    && query->match( datablock->getDataBlock() ) ) {
                    command.results.push_back( datablock->getDataBlock()->encode( eCodec_All ) );
                }
            }
        }
        delete query;
        g_HarborService->send( sid, &command );
    } else {
        // 索引条目不完整, 强制所有数据落地
        if ( entry != nullptr && entry->getStatus() == IndexEntry::eStatus_InComplete ) {
            if ( entry->index().empty() ) {
                // NOTICE: 刷新整张表
                cache->flush2DB();
            } else {
                // 刷新单个索引
                CommandHandler::flushIndexEntry( entry, cache, false );
            }
        }

        // 拼接查询SQL语句
        std::string sqlcmd;
        std::vector<std::string> values;
        query->query( sqlcmd, values );
        // 发送到数据库中查询
        DataResultTrans * t = new DataResultTrans();
        assert( t != nullptr && "create DataResultTrans failed" );
        t->setCmd( cmd );
        t->setSid( sid );
        t->setTablename( table );
        t->setCliTransID( transid );
        t->setIdxString( idxstr );
        g_TransManager->append( t, timeout );
        // 提交到数据库中查询
        g_DBThreads->post( table, idxstr, DBMethod::Query, t->getTransID(),
            std::move(sqlcmd), std::move(values), new QueryResultsHandler( sid, transid, idxstr, query ) );
    }
}

bool CommandHandler::queryDatablock( const Tablename & table, const std::string & keystr, const Slice & dbdata )
{
    ISQLData * data = g_DataPrototype->data( table );
    if ( unlikely( data == nullptr ) ) {
        LOG_ERROR( "CommandHandler:queryDatablock(Table:'{}', KeyString:'{}') : can't found this Data Prototype .\n", table, keystr );
        return false;
    }

    // 解包
    data->decode( dbdata, eCodec_Dirty );

    // 索引字符串
    std::string idxstr;
    data->indexstring( eCodec_Dirty, idxstr );

    // 查询
    bool rc = CommandHandler::queryDatablock( data, keystr, idxstr );
    delete data; return rc;
}

bool CommandHandler::queryDatablock( ISQLData * data, const std::string & keystr, const std::string & idxstr )
{
    // 拼SQL语句
    std::string sqlcmd;
    std::vector<std::string> values;
    data->query( sqlcmd, values, false );

    // 发送到数据库中查询
    DatablockTrans * t = new DatablockTrans();
    assert( t != nullptr && "create DatablockTrans failed" );
    t->setTablename( data->tablename() );
    t->setIdxString( idxstr );
    t->setKeyString( keystr );
    g_TransManager->append( t, eMaxQueryTimeoutSeconds );
    // 提交到数据库中查询
    g_DBThreads->post( data->tablename(), idxstr, DBMethod::Query, t->getTransID(), std::move(sqlcmd), std::move(values) );

    return true;
}

void CommandHandler::doMessageQueue( TableCache * cache, const std::string & keystr )
{
    TableCache::MessageQueue mq;
    cache->popMessages( keystr, mq );

    for ( const auto & msg : mq ) {
        switch ( msg->head.cmd ) {
            case eSSCommand_DBUpdate: update( msg, 1 ); break;
            case eSSCommand_DBRemove: remove( msg, 1 ); break;
            case eSSCommand_DBReplace: replace( msg, 1 ); break;
            case eSSCommand_DBInvoke: invoke( msg, 1 ); break;
        }
        delete msg;
    }
}

} // namespace data
