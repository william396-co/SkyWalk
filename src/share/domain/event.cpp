
#include <cassert>
#include <algorithm>

#include "event.h"

EventWatcher::EventWatcher()
{}

EventWatcher::~EventWatcher()
{
    for ( auto & p : m_WatchList ) {
        for ( auto & ev : p.second ) {
            delete ev;
        }
        p.second.clear();
    }

    m_WatchList.clear();
}

bool EventWatcher::watch( Event * ev, bool callback )
{
    // 监听事件
    if ( callback ) {
        ev->getAgent()->subscribe( ev->getType() );
        ev->onWatch();
    }

    // 已经完成
    if ( ev->isComplete()
        && ev->isCountLimited() ) {
        ev->onCancel();
        ev->getAgent()->unsubscribe( ev->getType() );
        return true;
    }

    EventList * list = getList( ev->getType() );
    if ( list == nullptr ) {
        return false;
    }

    // 事件不能重复添加
    if ( list->end()
        == std::find( list->begin(), list->end(), ev ) ) {
        list->push_back( ev );
        return true;
    }

    return false;
}

void EventWatcher::cancel( Event * ev, bool callback )
{
    EventList * list = getList( ev->getType() );
    if ( list == nullptr ) {
        return;
    }

    auto result = std::find(
        list->begin(), list->end(), ev );
    if ( result != list->end() ) {
        if ( callback ) {
            ev->onCancel();
            ev->getAgent()->unsubscribe( ev->getType() );
        }
        list->erase( result );
    }
}

void EventWatcher::dispatch( EventType type, int64_t param1, int64_t param2, int64_t param3 )
{
    EventList * list = getList( type );
    if ( list == nullptr ) {
        return;
    }

    std::vector<Event *> eventlist;

    for ( auto it = list->begin(); it != list->end(); ) {
        Event * ev = *it;
        bool isactive = ev->isActive();

        // 触发事件
        if ( isactive ) {
            ev->onEvent( param1, param2, param3 );
        }
        // 如果事件已经完成并且计数受限的话，自动取消
        if ( isactive
            && ( !ev->isComplete() || !ev->isCountLimited() ) ) {
            ++it;
        } else {
            it = list->erase( it );
            eventlist.push_back( ev );
        }
    }

    for ( auto ev : eventlist ) {
        ev->onCancel();
        ev->getAgent()->unsubscribe( ev->getType() );
    }
}

EventWatcher::EventList * EventWatcher::getList( EventType type )
{
    auto result = m_WatchList.find( type );
    if ( result == m_WatchList.end() ) {
        m_WatchList.emplace( type, EventList() );
        return getList( type );
    }

    return &( result->second );
}
