#pragma once

#include <vector>
#include <cstdint>
#include <functional>

namespace utils {

class TimeWheel;
struct TimerEntry;

//
// 定时器事件
//
class ITimerEvent
{
public:
    ITimerEvent() {}
    virtual ~ITimerEvent() {}
    // onTimer()中同步cancel()并且销毁ITimerEvent是不被允许的
    virtual void onEnd() = 0;
    virtual bool onTimer( uint32_t & timeout ) = 0;

public:
    // 是否暂停...
    bool isPaused() const;
    // 是否在调度中...
    bool isSchedule() const;

private:
    friend class TimeWheel;
    TimerEntry * entry() const { return m_Entry; }
    void entry( TimerEntry * en ) { m_Entry = en; }

    TimerEntry * m_Entry = nullptr;
};

//
// 定时器管理器(时间轮)
//
class TimeWheel
{
public:
    // count        - 桶的个数, 必须是2的N次幂
    // precision    - 最大精度, 精确到毫秒, update()的调用粒度
    TimeWheel( uint32_t count, uint32_t precision );
    ~TimeWheel();
    typedef std::function<void( ITimerEvent * )> DeferDeleter;

public:
    // 初始化/清空
    bool init();
    void clear();

    // 更新
    int32_t update();

    // 精度
    uint32_t precision() const { return m_MaxPrecision; }

    // 添加定时器
    //      ev      - 定时器句柄;
    //      msecs   - 定时器的超时时间, 自动对齐到最大精度
    //
    // 根据ev的状态进行行为描述 :
    //      1. ev首次添加到管理器中, 超时时间msecs < 最大精度的情况下, 会自动对齐到最大精度
    //      2. ev已经在管理器的调度中, 添加时会先将上一次的调度取消, 重新添加到管理器中
    //         msecs == 0, 定时器的超时时间不做更改
    //         msecs > 最大精度, 定时器的超时时间更改为msecs
    //      3. ev处于暂停状态, 添加时会继续走完上一调度周期后, 继续以msecs为超时时间调度
    //         msecs == 0, 定时器的超时时间不做更改
    //         msecs > 最大精度, 定时器在走完上一周期后, 超时时间更改为msecs
    bool schedule( ITimerEvent * ev, uint32_t msecs = 0 );

    // 暂停定时器
    // 根据定时器的状态进行行为描述:
    //      1. 定时器未到期, 定时器挂起, 等待恢复schedule()
    //      2. 定时器刚到期, 定时器挂起, 等待恢复schedule(), 恢复时会多走一个最大精度时间
    bool pause( ITimerEvent * ev );

    // 挂起/恢复回调
    bool halt( ITimerEvent * ev, bool onoff );

    // 取消定时器(支持延迟销毁)
    bool cancel( ITimerEvent * ev, const DeferDeleter & deleter = nullptr );

private:
    struct Operation
    {
        uint8_t type = 0;
        ITimerEvent * event = nullptr;
        uint32_t mseconds = 0;
        DeferDeleter deleter = nullptr;
        Operation() = default;
        Operation( ITimerEvent * ev ) : type(2), event(ev) {}
        Operation( ITimerEvent * ev, uint32_t s ) : type(1), event(ev), mseconds(s) {}
        Operation( ITimerEvent * ev, const DeferDeleter & d ) : type(3), event(ev), deleter(d) {}
        bool isdefer() const { return type == 3 && deleter != nullptr; }
        void process( TimeWheel * timer ) {
            switch ( type ) {
                case 1 : timer->schedule( event, mseconds ); break;
                case 2 : timer->pause( event ); break;
                case 3 : timer->cancel( event, deleter ); break;
            }
        }
    };

    uint32_t getIndex( uint32_t seconds );
    uint32_t getStepCount( uint32_t seconds );
    void append( const Operation & operation );
    void add2Front( uint32_t index, TimerEntry * entry );

private:
    uint32_t m_DispatchRef;
    uint32_t m_BucketCount;
    uint32_t m_MaxPrecision;
    TimerEntry * m_EventBuckets;
    ITimerEvent * m_CurrentEvent;
    std::vector<Operation> m_CurrentList;
    std::vector<Operation> m_OperationList;
};

} // namespace utils
