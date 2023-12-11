
#ifndef __SRC_EXTRA_SKIPDAYTIMER_H__
#define __SRC_EXTRA_SKIPDAYTIMER_H__

#include "utils/timer.h"
#include "utils/types.h"

#include "extra/unit.h"
#include "extra/ticksvc.h"

template<typename T>
class SkipdayTimer : public utils::ITimerEvent
{
public:
    SkipdayTimer( UnitManager * m, void ( T::*f )( time_t ), int32_t count = 10 )
        : m_UnitManager( m ),
          m_Func( f ),
          m_Timestamp( 0 ),
          m_CountPerFrame( count )
    {}

    virtual ~SkipdayTimer()
    {}

    virtual void onEnd()
    {}

    virtual bool onTimer( uint32_t & timeout )
    {
        for ( int32_t i = 0; i < m_CountPerFrame; ++i ) {
            if ( m_SkipdaySet.empty() ) {
                break;
            }
            T * ch = (T *)m_UnitManager->getUnit( *( m_SkipdaySet.begin() ) );
            if ( ch != NULL ) {
                ( ( ch )->*m_Func )( m_Timestamp );
            }
            // 完成了
            m_SkipdaySet.erase( m_SkipdaySet.begin() );
        }
        return !m_SkipdaySet.empty();
    }

public:
    bool start( TickService * s, time_t t )
    {
        m_Timestamp = t;
        m_SkipdaySet.clear();
        m_UnitManager->getOnlineRole( m_SkipdaySet );

        // 每帧处理
        return s->schedule( this, 0 );
    }

    // 获取时间戳
    time_t getTimestamp() const { return m_Timestamp; }
    // 完成
    void remove( UnitID id ) { m_SkipdaySet.erase( id ); }
    // 是否隔天
    bool isSkipday( UnitID id ) const { return m_SkipdaySet.find( id ) != m_SkipdaySet.end(); }

private:
    TickService * m_TickService;
    UnitManager * m_UnitManager;
    void ( T::*m_Func )( time_t );

private:
    time_t m_Timestamp;
    int32_t m_CountPerFrame;
    UnorderedSet<UnitID> m_SkipdaySet;
};

#endif
