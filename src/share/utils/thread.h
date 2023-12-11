#pragma once

#include <deque>
#include <mutex>

#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#include "types.h"

namespace utils {

// 获取线程号
#if !defined __FreeBSD__
pid_t gettid();
#endif

//
// 基本线程
//
class IThread
{
public:
    IThread();
    virtual ~IThread();
    virtual bool onStart() = 0;
    virtual void onExecute() = 0;
    virtual void onStop() = 0;

public:
    // 设置
    void setDetach() { m_IsDetach = true; }
    void setStackSize( uint32_t size ) { m_StackSize = size; }

    // 控制接口
    bool start( bool suspend = false );
    void suspend();
    void resume();
    void stop( bool waiting = true );

    // 查看
    static bool check( pthread_t id );
    pid_t tid() const { return m_Tid; }
    pthread_t id() const { return m_ThreadID; }
    bool isRunning() const { return m_Status == eRunning; }
    bool isActive() const { return m_Status == eRunning || m_Status == eSuspend; }

    // onExcute()中停止线程
    void stoping() { m_Status = eStoping; }
    void suspending() { m_Status = eSuspend; }

private:
    enum Status
    {
        eReady = 0,
        eRunning = 1,
        eSuspend = 2,
        eStoping = 3,
        eStoped = 4,
    };

    void notify();
    void update( Status s );
    static void * threadfunc( void * arg );

private:
    bool m_IsDetach;
    uint32_t m_StackSize;
    Status m_Status;
    pid_t m_Tid;
    pthread_t m_ThreadID;

protected:
    pthread_cond_t m_CtrlCond;
    pthread_mutex_t m_CtrlLock;
};

//
// 工作线程
// 时序图:
// void loop()
// {
//      while( N )
//          this->onTask();
//      this->onIdle();
//  }
//
class IWorkThread : public IThread
{
public:
    IWorkThread( int32_t precision, int32_t peakcount = 0 );
    virtual ~IWorkThread();
    // 开启的回调
    virtual bool onStart() = 0;
    // 空闲回调
    virtual void onIdle() = 0;
    // 任务回调
    virtual void onTask( int32_t type, void * task ) = 0;
    // 停止的回调
    virtual void onStop() = 0;

public:
    // 提交任务
    bool post( int32_t type, void * task );

    // 清理队列
    void cleanup();

    // 精度
    int32_t precision() const { return m_Precision; }

    // 设置每帧处理的任务个数
    int32_t getPeakCount() const { return m_PeakCount; }
    void setPeakCount( int32_t count ) { m_PeakCount = count; }

    // 获取缓存的时间
    inline int64_t now( bool absolutely = false ) const
    {
        int64_t now = 0;

        now += m_Timeslice.tv_sec * 1000;
        now += m_Timeslice.tv_usec / 1000;

        if ( !absolutely ) {
            now += (int64_t)global::delta_timestamp * 1000;
        }

        return now;
    }

protected:
    // 获取调试信息
    size_t queuesize() const { return m_CachedQueueSize; }
    size_t taskcount() const { return m_CachedTaskCount; }

private:
    // 处理业务
    void onExecute();
    // 当前时间
    int64_t milliseconds();

private:
    // 队列
    struct Task
    {
        Task( int32_t type, void * task )
            : type { type }, task { task } {}
        int32_t type;
        void * task;
    };

    int32_t m_Precision;
    int32_t m_PeakCount;
    struct timeval m_Timeslice;
    std::mutex m_QueueLock;
    std::deque<Task> m_TaskQueue; // 任务队列

    size_t m_CachedQueueSize; // 缓存的队列长度
    size_t m_CachedTaskCount; // 缓存的任务个数
};

} // namespace utils

