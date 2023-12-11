
#include <algorithm>

#include "proplock.h"

void PropLock::lock( uint32_t prop, int64_t expiretime )
{
    auto result = std::find(
        m_LockedProps.begin(),
        m_LockedProps.end(), prop );
    if ( result != m_LockedProps.end() ) {
        result->timestamp = expiretime;
    } else {
        m_LockedProps.push_back( PropUnit( prop, expiretime ) );
    }
}

void PropLock::unlock( uint32_t prop )
{
    auto result = std::find(
        m_LockedProps.begin(),
        m_LockedProps.end(), prop );
    if ( result != m_LockedProps.end() ) {
        m_LockedProps.erase( result );
    }
}

bool PropLock::isLock( uint32_t prop, int64_t now ) const
{
    auto result = std::find(
        m_LockedProps.begin(),
        m_LockedProps.end(), prop );
    if ( result == m_LockedProps.end() ) {
        return false;
    }

    return now <= result->timestamp;
}
