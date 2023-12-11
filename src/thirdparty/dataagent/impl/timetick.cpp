
#include "utils/xtime.h"

#include "timetick.h"

namespace data {

TimeTick::TimeTick( uint32_t precision )
    : m_Precision( precision ),
      m_Timestamp( utils::TimeUtils::now() ),
      m_TimerManager( eTimeTick_BucketCount, precision )
{}

TimeTick::~TimeTick()
{}

bool TimeTick::start()
{
    if ( !m_TimerManager.init() ) {
        stop();
        return false;
    }

    return true;
}

void TimeTick::stop()
{
    m_TimerManager.clear();
}

void TimeTick::run( int64_t now )
{
    if ( now == 0 ) {
        now = utils::TimeUtils::now();
    }

    if ( now - m_Timestamp < m_Precision ) {
        return;
    }

    m_Timestamp = now;
    m_TimerManager.update();
}

bool TimeTick::schedule( utils::ITimerEvent * ev, uint32_t msecs )
{
    return m_TimerManager.schedule( ev, msecs );
}

bool TimeTick::cancel( utils::ITimerEvent * ev )
{
    return m_TimerManager.cancel( ev );
}

} // namespace data
