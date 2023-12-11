#pragma once

#include <functional>
#include <time.h>

#include "base/types.h"
#include "utils/timer.h"

// 定时服务
class TickService
{
public:
    enum {
        eTickService_BucketCount = 8192, // 桶个数
    };

    TickService( uint32_t precision );
    ~TickService();

public:
    bool start();
    void run( int64_t now );
    void stop();

    bool schedule( utils::ITimerEvent * ev, uint32_t msecs = 0 );
    bool pause( utils::ITimerEvent * ev );
    bool halt( utils::ITimerEvent * ev, bool onoff );
    bool cancel( utils::ITimerEvent * ev, const utils::TimeWheel::DeferDeleter & deleter = nullptr );

    utils::TimeWheel * getTimer() { return &m_TimerManager; }

private:
    int64_t m_Timestamp;
    utils::TimeWheel m_TimerManager;
};

// 渐进式结算器
class GradualCalculator : public utils::ITimerEvent
{
public :
    using TimerFunction = std::function<bool ()>;

    GradualCalculator( const TimerFunction & f )
        : m_TimerFunction( f ) {}
    virtual ~GradualCalculator() = default;
    virtual void onEnd() { delete this; }
    virtual bool onTimer( uint32_t & timeout ) { return m_TimerFunction(); }

    void start( TickService * s, int32_t interval = 0 ) {
        if ( isSchedule() ) { return; }
        s->schedule( this, interval );
    }

private :
    TimerFunction m_TimerFunction = nullptr;
};

// 每分钟定时器
class EveryMinuteTimer final : public utils::ITimerEvent
{
public:
    using TimerFunction = std::function< void ( time_t, const std::string &) >;

    EveryMinuteTimer( const TimerFunction & func );
    virtual ~EveryMinuteTimer() final = default;
    virtual void onEnd() final;
    virtual bool onTimer( uint32_t & timeout ) final;

protected:
    void setTimestamp( time_t now );
    time_t m_LastTimestamp = 0;
    TimerFunction m_TimerFunction = nullptr;
};
