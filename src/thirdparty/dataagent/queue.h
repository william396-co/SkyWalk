
#ifndef __SRC_DATAAGENT_QUEUE_H__
#define __SRC_DATAAGENT_QUEUE_H__

#include <vector>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "agent.h"
#include "helper.h"
#include "manager.h"

namespace data {

// 处理函数
// -1 取消, 0, 成功, 1 - 超时
using AsyncConsumer = std::function<void( int32_t, void *, int32_t )>;

template<typename Key, typename Hash = std::hash<Key> >
class LoadqueueService
{
public:
    LoadqueueService( DataManager * mgr )
        : m_Manager( mgr )
    {}

    ~LoadqueueService() {
        for ( auto && kv : m_QueueMap ) {
            kv.second.process( -1 );
        }
        m_QueueMap.clear();
    }

public :
    bool has( const Key & id ) const {
        return m_QueueMap.find( id ) != m_QueueMap.end();
    }

    void * context( const Key & id ) const {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            return pos->second.data;
        }
        return nullptr;
    }

    int32_t status( const Key & id ) const {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            return pos->second.status;
        }
        return -1;
    }

    void update( const Key & id, int32_t status ) {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            pos->second.status = status;
        }
    }

    void attach( const Key & id, int32_t status = -1, void * data = nullptr, const AsyncConsumer & recycler = nullptr ) {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            pos->second.data = data;
            pos->second.status = status;
            pos->second.recycler = recycler;
        }
    }

    void remove( const Key & id ) {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            pos->second.process( -1 );
            m_QueueMap.erase( pos );
        }
    }

    void push( const Key & id, const AsyncConsumer & processor ) {
        auto pos = m_QueueMap.find( id );
        if ( pos != m_QueueMap.end() ) {
            pos->second.queue.emplace_back( processor );
        }
    }

    void query( const Key & id, ISQLData * q, int32_t timeout, const GeneralQueryCallback & queryer ) {
        if ( has( id ) ) {
            return;
        }
        m_QueueMap.emplace( id, Queue( queryer ) );
        using namespace std::placeholders;
        m_Manager->query( q, timeout,
            std::bind( &LoadqueueService::callbackWrapper, this, id, _1, _2, _3 ) );
    }

    void query( const Key & id, const std::vector<ISQLData *> & qs, int32_t timeout, const GeneralQueryCallback & queryer ) {
        if ( has( id ) ) {
            return;
        }
        m_QueueMap.emplace( id, Queue( queryer ) );
        using namespace std::placeholders;
        m_Manager->query( qs, timeout,
            std::bind( &LoadqueueService::callbackWrapper, this, id, _1, _2, _3 ) );
    }

private:
    struct Queue {
        int32_t status = -1;                    // 状态
        void * data = nullptr;                  // 上下文
        AsyncConsumer recycler = nullptr;       // 上下文回收
        std::vector<AsyncConsumer> queue;       // 队列
        GeneralQueryCallback queryer = nullptr; // 查询器
        Queue() = default;
        Queue( const GeneralQueryCallback & cb ) : queryer( cb ) {}
        ~Queue() = default;
        void process( int32_t result ) {
            std::for_each( queue.begin(), queue.end(),
                [ this, result ] ( auto & p ) { p( result, data, status ); } );
            if ( data != nullptr
                && recycler != nullptr ) {
                recycler( result, data, status );
            }
        }
    };

    void callbackWrapper( const Key & id, QueryStage stage, const Tablename & table, const Slices & results ) {
        auto pos = m_QueueMap.find( id );
        if ( pos == m_QueueMap.end() ) {
            return;
        }

        Queue & q = pos->second;
        // 回调
        q.queryer( stage, table, results );
        // 继续回调给父节点
        if ( stage == QueryStage::Complete
            || stage == QueryStage::Timeout ) {
            q.process( stage == QueryStage::Complete ? 0 : 1 );
            q.queue.clear(); m_QueueMap.erase( pos );
        }
    }

private :
    DataManager * m_Manager;
    std::unordered_map<Key, Queue, Hash> m_QueueMap;
};

} // namespace data

#endif
