
#ifndef __SRC_DATAAGENT_IMPL_TIMETICK_H__
#define __SRC_DATAAGENT_IMPL_TIMETICK_H__

#include <time.h>

#include "base/base.h"
#include "base/types.h"
#include "utils/timer.h"

namespace data {

class TimeTick
{
public:
    TimeTick( uint32_t precision );
    ~TimeTick();

public:
    bool start();
    void run( int64_t now = 0 );
    void stop();

    bool schedule( utils::ITimerEvent * ev, uint32_t msecs );
    bool cancel( utils::ITimerEvent * ev );

    utils::TimeWheel * getTimer() { return &m_TimerManager; }

private:
    enum
    {
        eTimeTick_BucketCount = 4096, // 桶个数
    };

    uint32_t m_Precision;
    int64_t m_Timestamp;
    utils::TimeWheel m_TimerManager;
};

} // namespace data

#endif
