#pragma once

#include <vector>
#include <cstdint>
#include <functional>

namespace utils {

class TimeWheel;
struct TimerEntry;

// Timer Event
class ITimerEvent
{
    friend class TimeWheel;

public:
    ITimeEvent() = default;
    virtual ~TimerEvent() {}

    // Interface for Every Timer
    virtual void onEnd() = 0;
    virtual bool onTimer( uint32_t & timeout ) = 0;

private:
    bool isPaused() const;
    bool isScheddule() const;

private:
    TimerEntry * entry() const { return m_Entry; }
    void entry( TimerEntry * en ) { m_Entry = en; }

    TimerEntry * m_Entry { nullptr };
};

// TimerEevent Manager(TimeWheel)
class TimeWheel
{
public:
public:
    bool init();
    void clear();

    int32_t update();
    uint32_t precision() const { return m_MaxPrecision; }
}；

} // namespace utils
