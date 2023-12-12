
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <sys/time.h>

#include "serverstats.h"

ServerStats::ServerStats()
{}

ServerStats::~ServerStats()
{}

int64_t ServerStats::now()
{
    int64_t now = 0;
    struct timeval tv;

    if ( ::gettimeofday( &tv, nullptr ) == 0 ) {
        now = tv.tv_sec * 1000000 + tv.tv_usec;
    }

    return now;
}

void ServerStats::print()
{
    printf( "\n" );
    printf( "%s", output().c_str() );
}

std::string ServerStats::output()
{
    std::string out;

    for ( const auto & p : m_PeakStats ) {
        char line[1024];

        std::sprintf( line, "0x%04x --> %" PRId64 ", %" PRId64 "\n", p.first, m_AverageStats[p.first], p.second );
        out += line;
    }

    return out;
}

void ServerStats::update( uint32_t cmd, int64_t start )
{
    int64_t elapsed = ServerStats::now() - start;

    // 替换峰值
    if ( m_PeakStats[cmd] < elapsed ) {
        m_PeakStats[cmd] = elapsed;
    }

    // 计算平均值
    elapsed += m_AverageStats[cmd];
    elapsed /= (float)2.0f;
    m_AverageStats[cmd] = elapsed;
}
