#pragma once

#include <unordered_map>
#include <vector>
#include <stdint.h>
#include "base/event.h"
#include "utils/types.h"

// 事件代理
class EventAgent
{
public:
    EventAgent() {}
    virtual ~EventAgent() {}

    // 保存进度
    virtual void saveProgress( bool sync ) = 0;
    // 订阅
    virtual void subscribe( EventType type ) {}
    virtual void unsubscribe( EventType type ) {}
    // 是否是激活状态
    virtual bool isActive() const { return true; }
    // 事件触发时重置计数
    virtual bool isResetCount() const { return false; }
    // 计数是否受限(到达目标后，不再计数)
    virtual bool isCountLimited() const { return true; }
};

// 事件
class Event
{
public:
    Event( EventType type, int64_t target, EventAgent * agent )
        : m_Type( type ),
          m_Count( 0 ),
          m_Target( target ),
          m_Agent( agent )
    {}

    virtual ~Event()
    {}

    virtual void onWatch() {}
    virtual void onCancel() {}
    virtual void onEvent( int64_t param1, int64_t param2, int64_t param3 ) {}

public:
    // 获取事件类型
    EventType getType() const { return m_Type; }
    EventAgent * getAgent() const { return m_Agent; }
    // 获取计数
    void setCount( int64_t c ) { m_Count = c; }
    int64_t getCount() const { return m_Count; }
    int64_t getTarget() const { return m_Target; }
    // 是否是激活状态
    bool isActive() const { return m_Agent->isActive(); }
    // 是否完成
    bool isComplete() const { return m_Count >= m_Target; }
    // 是否受限
    bool isCountLimited() const { return m_Agent->isCountLimited(); }

protected:
    EventType m_Type;     // 事件类型
    int64_t m_Count;      // 计数
    int64_t m_Target;     // 目标次数
    EventAgent * m_Agent; // 事件代理
};

class EventWatcher
{
public:
    EventWatcher();
    ~EventWatcher();

public:
    // 关注事件
    bool watch( Event * ev, bool callback = true );
    // 取消关注
    void cancel( Event * ev, bool callback = true );
    // 分发事件
    void dispatch( EventType type,
        int64_t param1 = 0, int64_t param2 = 0, int64_t param3 = 0 );

private:
    typedef std::vector<Event *> EventList;
    // 获取事件列表
    EventList * getList( EventType type );

private:
    std::unordered_map<EventType, EventList> m_WatchList;
};

