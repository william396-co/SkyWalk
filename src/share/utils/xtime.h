#pragma once

#include <chrono>
#include <string>

namespace utils {

// seconds
inline time_t time()
{
    using namespace std::chrono;
    return duration_cast<seconds>( system_clock::now().time_since_epoch() ).count();
}
// milliseconds
inline time_t now()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count();
}
// microseconds
inline time_t current()
{
    using namespace std::chrono;
    return duration_cast<microseconds>( system_clock::now().time_since_epoch() ).count();
}

class TimeUtils
{
public:
    enum
    {
        eDays_OneWeek = 7,           // 一周的天数
        eSeconds_Hour = 3600,        // 每小时的秒数
        eSeconds_OneDay = 3600 * 24, // 一天的秒数
        eSeconds_OneWeek = 7 * 3600 * 24, // 一周的秒数
    };

    TimeUtils();
    TimeUtils( time_t t );
    TimeUtils( struct tm * tm );

public:
    // 获取UTC时间
    time_t getTimestamp();
    // 获取时间结构
    struct tm * getTimeStruct();

    // 2012-12-24 20121224
    int32_t getDate();

    // 10:10:58 101058
    int32_t getTime();
    void getTime( std::string & timestring );

    // 格式化时间
    std::string getFormatTime( const std::string & f );

    // 获取年份
    int32_t getYear();
    // 获取月份
    int32_t getMonth();

    // 获取星期几
    // 周一到周日 返回1~7
    int32_t getWeekday();

    // 获取日期
    int32_t getMonthday();

    // 获取第几周(距离元旦，周一开始)
    // 0出错; 从1开始
    int32_t getWeeknumber();

    // 获取ISO8601的WeekNumber
    // 返回201653
    int32_t getWeekOfYear();
    int32_t getWeekOfYear( std::pair<int32_t, int32_t> & weeks );

    // 获取默认0点的时间戳
    time_t getZeroClockTimestamp();

    // 获取下一天0点的时间戳
    time_t getNextZeroClockTimestamp();

    // 获取当天指定时间点的时间戳
    // 10:00:00
    time_t getSpecifiedTimestamp( const char * s );

    // 获取下一天的指定时间点的时间戳
    time_t getNextSpecifiedTimestamp( const char * s );

    // 获取周为单位的时间
    // offset   - -1上周;0本周;1下周;2下下周
    // weekday  - 周几
    // s        - 10:00:00
    time_t getSpecifiedTimestamp( int32_t offset, int32_t weekday, const std::string & s );

public:
   
    // 获取时区
    // <0 西时区
    // >0 东时区
    static int32_t timezone();

    // "2010-10-10 00:00:00"
    static std::string getTime( time_t timestamp );

    // "2010-10-10 00:00:00.000"
    static std::string getMTime( time_t mtimestamp );

    // 毫秒
    static int32_t sleep( uint64_t mseconds );

    // 是否是闰年
    static bool isLeapYear( int32_t year );

    // "2010-10-10 00:10:01"
    static time_t getTimestamp( const char * str );

    // "2010-10-10 05:00:00"
    static time_t getZeroTimestamp( const char * str );

    // "2010-10-10 05:00:00"
    static time_t getEndTimestamp( const char * str );

    // "2010-10-10"
    static time_t getTimestampByDate( const char * date );

    // 计算t1到t2经过的自然日
    static int32_t daysdelta( time_t t1, time_t t2 );

    // timestamp是否过了s这个时间节点
    // s 每日指定的刷新时间 6:00:00
    // return: true 允许隔天 false 不允许隔天
    static bool isOverTime( time_t timestamp, const std::string & s, time_t now = 0 );

    // 是否隔月
    static bool isOverMonth( time_t timestamp, int32_t offset, time_t now = 0 );
    static bool isOverMonth( time_t timestamp, const std::string & s, time_t now = 0 );

    // timestamp是否在已某个weekday为起点的7天内
    // weekday 一周内的天数1~7
    static bool isInDuration( time_t timestamp, int32_t weekday, int32_t duration, const std::string & s = "00:00:00", time_t now = 0 );

private:
    time_t m_Timestamp;
    struct tm m_TimeStruct;
};

} // namespace utils
