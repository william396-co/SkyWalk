
#include <algorithm>
#include <cassert>
#include <iterator>
#include "types.h"
#include "timer.h"

namespace utils {

struct TimerEntry
{
public:
    TimerEntry() = default;
    ~TimerEntry() { remove(); }

    void remove() {
        if ( prev != nullptr
            && next != nullptr ) {
            prev->next = next;
            next->prev = prev;
            prev = next = nullptr;
        }
    }

public:
    bool state = true;      // 是否挂起
    uint32_t timeout = 0;   // 超时时间
    uint32_t stepcount = 0; // 步长
    uint32_t endref = 0;    // 过期索引
    uint32_t leftmsecs = 0; // 剩余时间

    TimerEntry * prev = nullptr;
    TimerEntry * next = nullptr;
    ITimerEvent * event = nullptr;
};

bool ITimerEvent::isPaused() const
{
    if ( m_Entry == nullptr ) {
        return false;
    }

    return m_Entry->leftmsecs != 0;
}

bool ITimerEvent::isSchedule() const
{
    // 未设置过Entry
    if ( m_Entry == nullptr ) {
        return false;
    }

    return m_Entry->leftmsecs == 0;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

TimeWheel::TimeWheel( uint32_t count, uint32_t precision )
    : m_DispatchRef( 0 ),
      m_BucketCount( count ),
      m_MaxPrecision( precision ),
      m_EventBuckets( nullptr ),
      m_CurrentEvent( nullptr )
{
    assert( !( count & ( count - 1 ) ) && "count must be 2^n" );
}

TimeWheel::~TimeWheel()
{}

bool TimeWheel::init()
{
    m_EventBuckets = new TimerEntry[m_BucketCount];
    if ( m_EventBuckets == nullptr ) {
        return false;
    }

    m_DispatchRef = 0;
    m_CurrentEvent = nullptr;

    for ( uint32_t i = 0; i < m_BucketCount; ++i ) {
        TimerEntry * entry = m_EventBuckets + i;
        entry->prev = entry;
        entry->next = entry;
    }

    return true;
}

void TimeWheel::clear()
{
    if ( m_EventBuckets ) {
        // 将管理器中的所有timer都销毁
        for ( uint32_t i = 0; i < m_BucketCount; ++i ) {
            TimerEntry * head = m_EventBuckets + i;
            TimerEntry * entry = head->next;
            while ( entry != head ) {
                TimerEntry * nextentry = entry->next;
                entry->event->onEnd();
                delete entry;
                entry = nextentry;
            }
        }

        delete[] m_EventBuckets;
        m_EventBuckets = nullptr;
    }

    m_CurrentEvent = nullptr;
    for ( const auto & op : m_OperationList ) {
        if ( op.isdefer() ) {
            cancel( op.event, op.deleter );
        }
    }

    m_DispatchRef = 0;
    m_BucketCount = 0;
    m_MaxPrecision = 0;
    m_CurrentList.clear();
    m_OperationList.clear();
}

bool TimeWheel::schedule( ITimerEvent * ev, uint32_t msecs )
{
    if ( m_CurrentEvent != nullptr ) {
        append( { ev, msecs } );
        return false;
    }

    uint32_t timeout = 0;
    TimerEntry * entry = nullptr;

    if ( ev->isSchedule() ) {
        // 正在调度中...
        entry = ev->entry();
        entry->remove();
        timeout = msecs == 0 ? entry->timeout : msecs;
    } else if ( ev->isPaused() ) {
        // 正在暂停
        entry = ev->entry();
        timeout = msecs == 0 ? entry->timeout : msecs;
        msecs = entry->leftmsecs;
    } else {
        // 创建entry
        entry = new TimerEntry();
        assert( entry != nullptr && "create new Entry failed" );
        timeout = msecs < m_MaxPrecision ? m_MaxPrecision : msecs;
    }

    // 最大时间精度
    msecs = std::max( msecs, m_MaxPrecision );

    uint32_t index = getIndex( msecs );
    uint32_t stepcount = getStepCount( msecs );

    // 初始化事件
    ev->entry( entry );
    // 初始化entry
    entry->state = true;
    entry->event = ev;
    entry->timeout = timeout;
    entry->stepcount = stepcount;
    entry->leftmsecs = 0;
    entry->endref = m_DispatchRef + msecs / m_MaxPrecision;
    add2Front( index, entry );

    return true;
}

bool TimeWheel::pause( ITimerEvent * ev )
{
    if ( m_CurrentEvent != nullptr ) {
        append( { ev } );
        return false;
    }

    TimerEntry * entry = ev->entry();

    if ( entry != nullptr ) {
        // 计算剩余时间
        uint32_t nbuckets = entry->endref - m_DispatchRef;

        // NOTICE: nbuckets == 0
        // 意味着定时器已经到期, 鉴于pause()的操作性, 现将定时器挂起
        // 并且标志nbuckets = 1, schedule()的下一时间精度立即回调
        nbuckets = nbuckets == 0 ? 1 : nbuckets;

        // 移除Entry, 设置剩余时间
        entry->remove();
        entry->leftmsecs = nbuckets * m_MaxPrecision;
    }

    return true;
}

bool TimeWheel::halt( ITimerEvent * ev, bool onoff )
{
    TimerEntry * entry = ev->entry();

    if ( entry != nullptr ) {
        entry->state = onoff;
        return true;
    }

    return false;
}

bool TimeWheel::cancel( ITimerEvent * ev, const DeferDeleter & deleter )
{
    halt( ev, false );

    if ( m_CurrentEvent != nullptr ) {
        append( { ev, deleter } );
        return false;
    }

    TimerEntry * entry = ev->entry();
    if ( entry != nullptr ) {
        delete entry;
        ev->entry( nullptr );
    }

    if ( deleter ) deleter( ev );

    return true;
}

int32_t TimeWheel::update()
{
    int32_t count = 0;
    uint32_t index = ( m_DispatchRef & ( m_BucketCount - 1 ) );

    TimerEntry * header = m_EventBuckets + index;
    TimerEntry * entry = header->next;
    while ( entry != header ) {
        TimerEntry * nextentry = entry->next;
        if ( --entry->stepcount == 0 ) {
            // 定时器时间到了
            uint32_t mseconds = entry->timeout;
            // 获取当前定时器事件
            m_CurrentEvent = entry->event;
            if ( entry->state
                && !m_CurrentEvent->onTimer( mseconds ) ) {
                // 移出定时器
                m_CurrentEvent->entry( nullptr );
                m_CurrentEvent->onEnd();
                delete entry;
                // 当前帧中有对m_CurrentEvent异步取消的操作
                auto pos = std::find_if( m_CurrentList.rbegin(),
                    m_CurrentList.rend(), []( auto & op ) { return op.isdefer(); } );
                if ( pos != m_CurrentList.rend() ) { m_OperationList.push_back(*pos); }
            } else {
                // 继续添加定时器
                entry->timeout = std::max(mseconds, m_MaxPrecision);
                // 重新计算索引值以及步长
                uint32_t newindex = getIndex( entry->timeout );
                entry->stepcount = getStepCount( entry->timeout );
                if ( newindex != index ) {
                    // 删除, 添加到新的slot中
                    entry->remove();
                    add2Front( newindex, entry );
                }
                // 把当前事件的操作都继续添加到操作队列中
                std::copy( m_CurrentList.begin(),
                    m_CurrentList.end(), std::back_inserter( m_OperationList ) );
            }
            m_CurrentList.clear();
            m_CurrentEvent = nullptr;
        }
        ++count;
        entry = nextentry;
    }

    m_CurrentEvent = nullptr;
    std::for_each(
        m_OperationList.begin(),
        m_OperationList.end(), [this]( auto & op ) { op.process(this); } );
    ++m_DispatchRef;
    m_OperationList.clear();

    return count;
}

inline uint32_t TimeWheel::getIndex( uint32_t seconds )
{
    uint32_t index = m_DispatchRef;

    index += ( seconds / m_MaxPrecision );
    index &= ( m_BucketCount - 1 );

    return index;
}

inline uint32_t TimeWheel::getStepCount( uint32_t seconds )
{
    uint32_t steps = seconds / m_MaxPrecision / m_BucketCount;

    if ( seconds % ( m_MaxPrecision * m_BucketCount ) != 0 ) {
        steps += 1;
    }

    // 确保step大于等于1
    return steps < 1 ? 1 : steps;
}

inline void TimeWheel::append( const Operation & operation )
{
    if ( m_CurrentEvent == operation.event ) {
        m_CurrentList.push_back( operation );
    } else {
        m_OperationList.push_back( operation );
    }
}

inline void TimeWheel::add2Front( uint32_t index, TimerEntry * entry )
{
    TimerEntry * header = m_EventBuckets + index;

    entry->next = header;
    entry->prev = header->prev;
    entry->prev->next = entry;
    entry->next->prev = entry;
}

} // namespace utils
