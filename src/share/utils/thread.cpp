
#include <vector>
#include <thread>
#include <chrono>

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "xtime.h"
#include "thread.h"
#include "base/support.h"

namespace utils {

#if !defined __FreeBSD__
pid_t gettid()
{
#if defined __linux__
    return syscall( SYS_gettid );
#elif defined __APPLE__ || defined __darwin__
    uint64_t tid;
    pthread_threadid_np( nullptr, &tid );
    return tid;
#else
    return 0;
#endif
}
#endif

IThread::IThread()
{
    m_Status = eReady;
    m_IsDetach = false;
    m_StackSize = 0;
    m_Tid = 0;
    m_ThreadID = 0;
    pthread_cond_init( &m_CtrlCond, nullptr );
    pthread_mutex_init( &m_CtrlLock, nullptr );
}

IThread::~IThread()
{
    if ( !m_IsDetach
        && isActive() ) {
        pthread_join( m_ThreadID, nullptr );
    }

    pthread_cond_destroy( &m_CtrlCond );
    pthread_mutex_destroy( &m_CtrlLock );
}

bool IThread::start( bool suspend )
{
    int32_t rc = 0;
    pthread_attr_t attr;

    pthread_attr_init( &attr );

    if ( m_StackSize > 0 ) {
        pthread_attr_setstacksize( &attr, m_StackSize );
    }
    if ( m_IsDetach ) {
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    }

    m_Status = suspend ? eSuspend : eRunning;
    rc = pthread_create( &m_ThreadID, &attr, threadfunc, this );
    pthread_attr_destroy( &attr );

    return ( rc == 0 );
}

void IThread::stop( bool waiting )
{
    pthread_mutex_lock( &m_CtrlLock );

    switch ( m_Status ) {
        case eReady:
            m_Status = eStoped;
            break;

        case eRunning:
        case eSuspend:
            m_Status = eStoping;
            break;

        default:
            break;
    }

    pthread_cond_signal( &m_CtrlCond );

    while ( waiting
        && m_Status != eStoped
        && pthread_self() != m_ThreadID ) {
        pthread_cond_wait( &m_CtrlCond, &m_CtrlLock );
    }

    pthread_mutex_unlock( &m_CtrlLock );
}

void IThread::suspend()
{
    update( eSuspend );
}

void IThread::resume()
{
    update( eRunning );
}

void IThread::notify()
{
    update( eStoped );
}

void IThread::update( Status s )
{
    pthread_mutex_lock( &m_CtrlLock );

    m_Status = s;
    pthread_cond_signal( &m_CtrlCond );

    pthread_mutex_unlock( &m_CtrlLock );
}

bool IThread::check( pthread_t id )
{
    int32_t rc = pthread_kill( id, 0 );
    return ( rc != ESRCH && rc != EINVAL );
}

void * IThread::threadfunc( void * arg )
{
    IThread * thread = (IThread *)arg;

    sigset_t mask;
    sigfillset( &mask );
    pthread_sigmask( SIG_SETMASK, &mask, nullptr );

    thread->m_Tid = gettid();

    // 线程开始
    if ( thread->onStart() ) {
        while ( thread->isActive() ) {
            if ( thread->m_Status == eRunning ) {
                // 执行
                thread->onExecute();
            } else {
                // 挂起
                pthread_mutex_lock( &thread->m_CtrlLock );
                pthread_cond_wait( &thread->m_CtrlCond, &thread->m_CtrlLock );
                pthread_mutex_unlock( &thread->m_CtrlLock );
            }
        }
    }

    // 线程停止了
    thread->onStop();

    // 通知调用者, 线程已经停止了
    thread->notify();

    return nullptr;
}

// ----------------------------------------------------------------------------

IWorkThread::IWorkThread( int32_t precision, int32_t peakcount )
    : m_Precision( precision ),
      m_PeakCount( peakcount ),
      m_CachedQueueSize( 0 ),
      m_CachedTaskCount( 0 )
{
    milliseconds();
}

IWorkThread::~IWorkThread()
{}

void IWorkThread::onExecute()
{
    std::deque<Task> tasklist;
    int64_t now = milliseconds();

    if ( m_PeakCount == 0 ) {
        // 交换任务队列
        m_QueueLock.lock();
        std::swap( m_TaskQueue, tasklist );
        m_CachedQueueSize = tasklist.size();
        m_CachedTaskCount = m_CachedQueueSize;
        m_QueueLock.unlock();
    } else if ( m_PeakCount > 0 ) {
        // 从任务队列中取出PeakCount个
        m_QueueLock.lock();
        while ( !m_TaskQueue.empty()
            && tasklist.size() < (size_t)m_PeakCount ) {
            Task task = m_TaskQueue.front();
            m_TaskQueue.pop_front();
            tasklist.push_back( task );
        }
        m_CachedTaskCount = tasklist.size();
        m_CachedQueueSize = m_TaskQueue.size();
        m_QueueLock.unlock();
    }

    // 回调逻辑层
    std::deque<Task>::iterator it = tasklist.begin();
    for ( ; it != tasklist.end(); ++it ) {
        onTask( it->type, it->task );
    }

    // 任务线程空闲
    onIdle();

    // 睡眠
    int32_t used_millisecs = milliseconds() - now;
    if ( used_millisecs >= 0 && used_millisecs < m_Precision ) {
        std::this_thread::sleep_for( std::chrono::milliseconds { m_Precision - used_millisecs } );
    }
}

void IWorkThread::cleanup()
{
    m_QueueLock.lock();

    while ( !m_TaskQueue.empty() ) {
        Task task = m_TaskQueue.front();
        m_TaskQueue.pop_front();
        this->onTask( task.type, task.task );
    }
    m_TaskQueue.clear();

    m_QueueLock.unlock();
}

bool IWorkThread::post( int32_t type, void * task )
{
    bool rc = false;


    if ( isActive() ) {
        rc = true;
        std::lock_guard lock(m_QueueLock);
        m_TaskQueue.emplace_back( type, task );
    }

    return rc;
}

inline int64_t IWorkThread::milliseconds()
{
    if ( ::gettimeofday( &m_Timeslice, nullptr ) == 0 ) {
        return now();
    }

    return 0;
}

} // namespace utils

#if 0

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int8_t main_flag;

void signal_handle( int32_t signo )
{
    main_flag = 0;
}

class IWorkThread1 : public Utils::IWorkThread
{

public :

    IWorkThread1() {};
    ~IWorkThread1() {};

public :

    void onTask( int32_t type, void * arg )
    {
        printf("Task:%d \n", type);
    }

};


int main()
{
    int32_t type = 1;
    IWorkThread1 thread;

    signal( SIGINT, signal_handle );

    thread.start();

    main_flag = 1;
    while( main_flag )
    {
        thread.post( type, nullptr );
        ++type;
    }

    thread.stop();

    return 0;
}

#endif
