
#ifndef __SRC_DOMAIN_SERVERSTAT_H__
#define __SRC_DOMAIN_SERVERSTAT_H__

#include <map>

#include "base/types.h"
#include "utils/singleton.h"

class ServerStats : public Singleton<ServerStats>
{
public:
    static int64_t now();

public:
    // 打印
    void print();
    std::string output();

    // 更新
    void update( uint32_t cmd, int64_t start );

private:
    friend class Singleton<ServerStats>;

    ServerStats();
    ~ServerStats();

    std::map<uint32_t, int64_t> m_PeakStats;
    std::map<uint32_t, int64_t> m_AverageStats;
};

inline int64_t STATS_START()
{
    int64_t s = 0;
#if defined( __DEBUG__ )
    s = ServerStats::instance().now();
#endif
    return s;
}

inline void STATS_STOP( uint32_t cmd, int64_t s )
{
#if defined( __DEBUG__ )
    ServerStats::instance().update( cmd, s );
#endif
}

inline std::string STATS_INFO()
{
    std::string out;
#if defined( __DEBUG__ )
    ServerStats::instance().print();
    out = ServerStats::instance().output();
#endif
    return out;
}

#endif
