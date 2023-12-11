
#include "utils/timer.h"
#include "utils/xtime.h"
#include "domain/profiler.h"

#include "ticksvc.h"

EveryMinuteTimer::EveryMinuteTimer( const TimerFunction & func )
    : m_LastTimestamp( 0LL ),
      m_TimerFunction( func )
{
    setTimestamp( utils::TimeUtils::time() );
}

void EveryMinuteTimer::onEnd()
{
    delete this;
}

bool EveryMinuteTimer::onTimer( uint32_t & timeout )
{
    utils::TimeUtils now;
    uint32_t deltaseconds = 5000; // 每隔5s校准一次时间

    // 测试环境纠正时间戳
    if ( now.getTimestamp() < m_LastTimestamp ) {
        setTimestamp( now.getTimestamp() );
    }

    while ( now.getTimestamp() - m_LastTimestamp >= 60 ) {
        m_LastTimestamp += 60;
        utils::TimeUtils t( m_LastTimestamp );
        std::string format;
        t.getTime( format );
        m_TimerFunction( m_LastTimestamp, format );
    }

    timeout = m_LastTimestamp + 60 - now.getTimestamp();
    timeout *= 1000;
    timeout = timeout > deltaseconds ? deltaseconds: timeout; // 校准时间

    return true;
}

void EveryMinuteTimer::setTimestamp( time_t now )
{
    m_LastTimestamp = now;
    m_LastTimestamp -= m_LastTimestamp % 60;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TickService::TickService( uint32_t precision )
    : m_Timestamp( utils::TimeUtils::now() ),
      m_TimerManager( eTickService_BucketCount, precision )
{}

TickService::~TickService()
{}

bool TickService::start()
{
    if ( !m_TimerManager.init() ) {
        stop();
        return false;
    }

    return true;
}

void TickService::stop()
{
    m_TimerManager.clear();
}

void TickService::run( int64_t now )
{
    // 测试环境调时间导致非法
    if ( now < m_Timestamp ) {
        m_Timestamp = now;
    }

    //if ( now - m_Timestamp < m_TimerManager.precision() )
    //  {
    //     return;
    //  }

    m_Timestamp = now;
    m_TimerManager.update();
}

bool TickService::schedule( utils::ITimerEvent * ev, uint32_t msecs )
{
    return m_TimerManager.schedule( ev, msecs );
}

bool TickService::pause( utils::ITimerEvent * ev )
{
    return m_TimerManager.pause( ev );
}

bool TickService::halt( utils::ITimerEvent * ev, bool onoff )
{
    return m_TimerManager.halt( ev, onoff );
}

bool TickService::cancel( utils::ITimerEvent * ev, const utils::TimeWheel::DeferDeleter & deleter )
{
    return m_TimerManager.cancel( ev, deleter );
}
