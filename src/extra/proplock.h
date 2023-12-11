#pragma once

#include "base/types.h"
#include "base/role.h"

class PropLock
{
public:
    PropLock() {}
    ~PropLock() {}

public:
    // 锁定
    void lock( uint32_t prop, int64_t expiretime );
    // 解锁
    void unlock( uint32_t prop );
    // 是否锁定
    bool isLock( uint32_t prop, int64_t now ) const;

private:
    struct PropUnit
    {
        uint32_t prop;
        int64_t timestamp;

        PropUnit() : prop( 0 ), timestamp( 0 ) {}
        PropUnit( uint32_t p, int64_t now ) : prop( p ), timestamp( now ) {}
        bool operator==( uint32_t prop ) const { return this->prop == prop; }
    };

private:
    std::vector<PropUnit> m_LockedProps;
};
