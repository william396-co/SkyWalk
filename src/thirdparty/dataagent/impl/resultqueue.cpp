
#include "agentimpl.h"
#include "transaction.h"
#include "resultqueue.h"

namespace data {

ResultQueue::ResultQueue()
{}

ResultQueue::~ResultQueue()
{
    for ( const auto & task : m_Queue ) {
        if ( task.type == eTaskType_SSMessage ) {
            delete static_cast<SSMessage *>( task.task );
        }
    }

    m_Queue.clear();
}

bool ResultQueue::post( int32_t type, void * task )
{
    m_Lock.lock();
    m_Queue.push_back( Task( type, task ) );
    m_Lock.unlock();

    return true;
}

void ResultQueue::process( DataAgentImpl * agent, bool stopping )
{
    std::deque<Task> taskqueue;

    // swap
    m_Lock.lock();
    std::swap( taskqueue, m_Queue );
    m_Lock.unlock();

    // loop, and process
    for ( const auto & task : taskqueue ) {
        if ( task.type != eTaskType_SSMessage ) {
            continue;
        }

        SSMessage * message = static_cast<SSMessage *>( task.task );

        switch ( message->head.cmd ) {
            case eSSCommand_DBResults: {
                DBResultsCommand * cmd = (DBResultsCommand *)message;

                // 查找事务
                utils::Transaction * t = agent->getTransactionManager()->get( cmd->transid );
                if ( likely( t != nullptr && !stopping ) ) {
                    t->onTrigger( message );
                }
            } break;

            case eSSCommand_DBAutoIncrease: {
                DBAutoIncreaseCommand * cmd = (DBAutoIncreaseCommand *)message;

                // 查找事务
                utils::Transaction * t = agent->getTransactionManager()->get( cmd->transid );
                if ( likely( t != nullptr && stopping ) ) {
                    t->onTrigger( message );
                }
            } break;

            case eSSCommand_Ping:
            case eSSCommand_Register:
                agent->updateKeepalive( message->sid );
                break;

            case eSSCommand_Unregister:
                agent->updateKeepalive( message->sid, 0 );
                break;

            case eSSCommand_DBException: {
                DBExceptionCommand * cmd = (DBExceptionCommand *)message;

                if ( agent->getExceptionFunction() != nullptr ) {
                    ( agent->getExceptionFunction() )( cmd->table, cmd->dbdata, cmd->script );
                }
            } break;
        }

        // 销毁消息
        delete message;
    }
}

} // namespace data
