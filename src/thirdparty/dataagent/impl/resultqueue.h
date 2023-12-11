
#ifndef __SRC_DATAAGENT_IMPL_RESULTQUEUE_H__
#define __SRC_DATAAGENT_IMPL_RESULTQUEUE_H__

#include <deque>

#include <pthread.h>

#include "io/task.h"
#include "base/types.h"
#include "utils/lock.h"
#include "message/dbprotocol.h"

namespace data {

class DataAgentImpl;

class ResultQueue
{
public:
    ResultQueue();
    ~ResultQueue();

public:
    // 提交结果
    bool post( int32_t type, void * task );

    // 处理队列中的数据
    void process( DataAgentImpl * agent, bool stopping = false );

private:
    struct Task
    {
        int32_t type;
        void * task;

        Task()
            : type( eTaskType_None ),
              task( nullptr )
        {}

        Task( int32_t type_, void * task_ )
            : type( type_ ),
              task( task_ )
        {}
    };

    utils::ThreadLock m_Lock;
    std::deque<Task> m_Queue;
};

} // namespace data

#endif
