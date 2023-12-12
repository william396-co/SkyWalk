
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>

#include <time.h>
#include <sys/time.h>

#include "xtime.h"

namespace global {
int32_t delta_timestamp = 0;
}

namespace utils {


TimeUtils::TimeUtils()
{
    m_Timestamp = 0;
    ::bzero( &m_TimeStruct, sizeof( m_TimeStruct ) );
}

TimeUtils::TimeUtils( time_t t )
{
    m_Timestamp = t;
    ::bzero( &m_TimeStruct, sizeof( m_TimeStruct ) );

    if ( t != 0 ) {
        ::localtime_r( &m_Timestamp, &m_TimeStruct );
    }
}

TimeUtils::TimeUtils( struct tm * tm )
{
    ::bzero( &m_TimeStruct, sizeof( m_TimeStruct ) );
    m_TimeStruct = *tm;
    m_Timestamp = ::mktime( tm );
}

time_t TimeUtils::getTimestamp()
{
    if ( m_Timestamp == 0 ) {
        m_Timestamp = now();
    }

    return m_Timestamp;
}

struct tm * TimeUtils::getTimeStruct()
{
    if ( m_TimeStruct.tm_year == 0 ) {
        time_t now = getTimestamp();
        ::localtime_r( &now, &m_TimeStruct );
    }

    return &m_TimeStruct;
}

int32_t TimeUtils::getDate()
{
    int32_t rc = 0;

    getTimeStruct();

    rc = ( m_TimeStruct.tm_year + 1900 ) * 10000;
    rc += ( m_TimeStruct.tm_mon + 1 ) * 100;
    rc += m_TimeStruct.tm_mday;

    return rc;
}

int32_t TimeUtils::getTime()
{
    int32_t rc = 0;

    getTimeStruct();

    rc = ( m_TimeStruct.tm_hour ) * 10000;
    rc += ( m_TimeStruct.tm_min ) * 100;
    rc += m_TimeStruct.tm_sec;

    return rc;
}

void TimeUtils::getTime( std::string & timestring )
{
    getTimeStruct();

    timestring.resize( 16 );
    int32_t len = std::snprintf(
        (char *)timestring.data(), 15, "%02d:%02d:%02d",
        m_TimeStruct.tm_hour, m_TimeStruct.tm_min, m_TimeStruct.tm_sec );
    timestring.resize( len );
}

std::string TimeUtils::getFormatTime( const std::string & f )
{
    char starttime[1024];
    strftime( starttime, 1023, f.c_str(), getTimeStruct() );
    return starttime;
}

int32_t TimeUtils::getWeekday()
{
    getTimeStruct();
    return m_TimeStruct.tm_wday == 0 ? 7 : m_TimeStruct.tm_wday;
}

int32_t TimeUtils::getMonthday()
{
    getTimeStruct();
    return m_TimeStruct.tm_mday;
}

int32_t TimeUtils::getWeeknumber()
{
    char buf[64] = { 0 };

    getTimeStruct();
    if ( ::strftime( buf, 63, "%W", &m_TimeStruct ) != 0 ) {
        return std::atoi( buf ) + 1;
    }

    return 0;
}

int32_t TimeUtils::getWeekOfYear()
{
    std::pair<int32_t, int32_t> weeks;
    return getWeekOfYear( weeks );
}

int32_t TimeUtils::getWeekOfYear( std::pair<int32_t, int32_t> & weeks )
{
    char buf[64] = { 0 };

    getTimeStruct();
    weeks.first = weeks.second = 0;

    if ( ::strftime( buf, 63, "%G", &m_TimeStruct ) != 0 ) {
        weeks.first = std::atoi( buf );
    }
    if ( ::strftime( buf, 63, "%V", &m_TimeStruct ) != 0 ) {
        weeks.second = std::atoi( buf );
    }

    return weeks.first * 100 + weeks.second;
}

int32_t TimeUtils::getYear()
{
    getTimeStruct();
    return m_TimeStruct.tm_year + 1900;
}

int32_t TimeUtils::getMonth()
{
    getTimeStruct();
    return m_TimeStruct.tm_mon + 1;
}

time_t TimeUtils::getZeroClockTimestamp()
{
    getTimeStruct();

    struct tm zero_timestamp = m_TimeStruct;
    zero_timestamp.tm_hour = zero_timestamp.tm_min = zero_timestamp.tm_sec = 0;

    return ::mktime( &zero_timestamp );
}

time_t TimeUtils::getNextZeroClockTimestamp()
{
    return getZeroClockTimestamp() + eSeconds_OneDay;
}

time_t TimeUtils::getSpecifiedTimestamp( const char * s )
{
    getTimeStruct();

    int32_t matched = 0;
    struct tm spec = m_TimeStruct;
    matched = std::sscanf( s, "%d:%d:%d", &( spec.tm_hour ), &( spec.tm_min ), &( spec.tm_sec ) );

    return matched > 0 ? ::mktime( &spec ) : 0;
}

time_t TimeUtils::getNextSpecifiedTimestamp( const char * s )
{
    int32_t matched = 0;
    struct tm spec;
    time_t nzerotime = this->getNextZeroClockTimestamp();
    ::localtime_r( &nzerotime, &spec );
    matched = std::sscanf( s, "%d:%d:%d", &( spec.tm_hour ), &( spec.tm_min ), &( spec.tm_sec ) );

    return matched > 0 ? ::mktime( &spec ) : 0;
}

time_t TimeUtils::getSpecifiedTimestamp( int32_t offset, int32_t weekday, const std::string & s )
{
    time_t rc = 0;

    getTimeStruct();

    struct tm spec = m_TimeStruct;
    int32_t matched = 0, nweekday = 0;
    matched = std::sscanf( s.c_str(), "%d:%d:%d", &( spec.tm_hour ), &( spec.tm_min ), &( spec.tm_sec ) );

    if ( matched <= 0 ) {
        return 0;
    }

    // 时间
    rc = ::mktime( &spec );

    // 计算周日
    weekday = weekday == 0 ? 7 : weekday;
    nweekday = spec.tm_wday == 0 ? 7 : spec.tm_wday;

    // 和weekday相差的天数
    rc += ( weekday - nweekday ) * eSeconds_OneDay;
    rc += offset * eSeconds_OneDay * eDays_OneWeek;

    return rc;
}

int32_t TimeUtils::timezone()
{
    struct tm utctm, localtm;
    time_t now = utils::now();

    // 计算utc时间
    gmtime_r( &now, &utctm );
    // 计算本地时间
    localtime_r( &now, &localtm );

    return ( mktime( &localtm ) - mktime( &utctm ) ) / 3600;
}

std::string TimeUtils::getTime( time_t timestamp )
{
    struct tm t;
    ::localtime_r( &timestamp, &t );

    char buf[64] = { 0 };
    ::strftime( buf, sizeof( buf ), "%Y-%m-%d %H:%M:%S", &t );

    return buf;
}

std::string TimeUtils::getMTime( time_t mtimestamp )
{
    struct tm t;
    time_t timestamp = mtimestamp / 1000;
    ::localtime_r( &timestamp, &t );

    char s[64] = { 0 };
    ::strftime( s, sizeof( s ), "%Y-%m-%d %H:%M:%S", &t );

    char buf[128] = { 0 };
    snprintf( buf, sizeof( buf ), "%s.%03lu", s, mtimestamp % 1000 );

    return buf;
}

int32_t TimeUtils::sleep( uint64_t mseconds )
{
    struct timeval tv;

    tv.tv_sec = mseconds / 1000;
    tv.tv_usec = ( mseconds % 1000 ) * 1000;

    return ::select( 0, nullptr, nullptr, nullptr, &tv );
}

bool TimeUtils::isLeapYear( int32_t year )
{
    char str[31];
    snprintf( str, 31, "%d-02-29", year );
    return TimeUtils( getZeroTimestamp( str ) ).getMonthday() == 29;
}

time_t TimeUtils::getTimestamp( const char * str )
{
    struct tm t;
    char * matched = nullptr;

    ::bzero( &t, sizeof( tm ) );
    matched = strptime( str, "%Y-%m-%d %H:%M:%S", &t );

    return matched != nullptr ? mktime( &t ) : 0;
}

time_t TimeUtils::getZeroTimestamp( const char * str )
{
    struct tm t;
    char * matched = nullptr;

    ::bzero( &t, sizeof( t ) );
    matched = strptime( str, "%Y-%m-%d", &t );

    return matched != nullptr ? mktime( &t ) : 0;
}

time_t TimeUtils::getEndTimestamp( const char * str )
{
    struct tm t;
    char * matched = nullptr;

    ::bzero( &t, sizeof( tm ) );
    matched = strptime( str, "%Y-%m-%d", &t );
    t.tm_hour = 23;
    t.tm_min = 59;
    t.tm_sec = 59;

    return matched != nullptr ? mktime( &t ) : 0;
}

time_t TimeUtils::getTimestampByDate( const char * date )
{
    struct tm t;
    char * matched = nullptr;

    ::bzero( &t, sizeof( tm ) );
    matched = strptime( date, "%Y-%m-%d", &t );

    return matched != nullptr ? mktime( &t ) : 0;
}

int32_t TimeUtils::daysdelta( time_t t1, time_t t2 )
{
    utils::TimeUtils tt1( t1 );
    utils::TimeUtils tt2( t2 );

    return std::abs( tt2.getZeroClockTimestamp() - tt1.getZeroClockTimestamp() ) / eSeconds_OneDay + 1;
}

bool TimeUtils::isOverTime( time_t timestamp, const std::string & s, time_t now )
{
    // 未重置过直接隔天
    if ( timestamp == 0 ) {
        return true;
    }

    TimeUtils t( now );
    // 当前时间
    time_t now_time = t.getTimestamp();
    // 当天刷新点时间
    time_t refresh_time = t.getSpecifiedTimestamp( s.c_str() );

    // 非法数据
    if ( refresh_time <= 0 ) {
        return false;
    }

    // 上次刷新点时间居然超过当前时间，到未来去了.
    // 到未来也要超过一天才算隔天
    // (其实是时间改到未来去了)
    if ( timestamp >= ( refresh_time + eSeconds_OneDay ) ) {
        return true;
    }

    if ( now_time >= refresh_time ) {
        if ( timestamp < refresh_time ) {
            return true;
        }
    } else {
        if ( timestamp < refresh_time - eSeconds_OneDay ) {
            return true;
        }
    }

    return false;
}

bool TimeUtils::isOverMonth( time_t timestamp, int32_t offset, time_t now )
{
    if ( timestamp == 0 ) {
        return true;
    }

    TimeUtils t( now );
    TimeUtils before( timestamp - offset );
    TimeUtils after( t.getTimestamp() - offset );
    return after.getYear() != before.getYear() || after.getMonth() != before.getMonth();
}

bool TimeUtils::isOverMonth( time_t timestamp, const std::string & s, time_t now )
{
    if ( timestamp == 0 ) {
        return true;
    }

    int32_t hour = 0;
    int32_t minute = 0;
    int32_t seconds = 0;
    int32_t matched = std::sscanf( s.c_str(), "%d:%d:%d", &hour, &minute, &seconds );

    time_t offset = 0;
    if ( matched > 0 ) {
        offset = 3600 * hour + 60 * minute + seconds;
    }

    TimeUtils t( now );
    TimeUtils before( timestamp - offset );
    TimeUtils after( t.getTimestamp() - offset );
    return after.getYear() != before.getYear() || after.getMonth() != before.getMonth();
}

bool TimeUtils::isInDuration( time_t timestamp, int32_t weekday, int32_t duration, const std::string & s, time_t now )
{
    if ( timestamp == 0 ) {
        return false;
    }

    if ( weekday == 0 || weekday > 7 ) {
        return true;
    }

    TimeUtils t( now );
    int32_t offset = t.getWeekday() < weekday ? -1 : 0;
    time_t starttime = t.getSpecifiedTimestamp( offset, weekday, s.c_str() );

    return timestamp >= starttime
        && timestamp < starttime + duration * utils::TimeUtils::eSeconds_OneDay;
}

} // namespace utils

