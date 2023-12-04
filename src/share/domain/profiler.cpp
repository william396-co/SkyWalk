#include "profiler.h"

#include <cassert>
#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "base/base.h"

void Profiler::startFunc( const std::string & func_name )
{
#if defined( __DEBUG__ )
    auto iter = m_Data.find( func_name );
    if ( iter == m_Data.end() ) {
       iter = m_Data.emplace( func_name, profilerInfo { func_name, ct_Func } ).first;
    } else {
        iter->second.start_list.emplace(  utils::current() );
    }
#endif
}

void Profiler::endFunc( const std::string & func_name )
{
#if defined( __DEBUG__ )
    auto iter = m_Data.find( func_name );
    if ( iter == m_Data.end() ) {
        LOG_ERROR( "{}  func_name:{}\n", __FUNCTION__, func_name );
        assert(false);
        return;
    }

    // match
    assert( !iter->second.start_list.empty() );
    if( iter->second.start_list.empty() ){
        LOG_ERROR("{} func_name:[{}] not matched\n", __FUNCTION__, func_name );
        return;
    }

    auto start_time = iter->second.start_list.top();
    iter->second.start_list.pop();
    // calc run time
    int run_time = utils::current() - start_time;

    ++iter->second.count;
    iter->second.total += run_time;
    if ( run_time < iter->second.min )
        iter->second.min = run_time;
    if ( run_time > iter->second.max )
        iter->second.max = run_time;

    if ( run_time < 10 ) ++iter->second.over_1us_count;
    else if ( run_time >= 10 && run_time < 100 ) ++iter->second.over_10us_count;
    else if ( run_time >= 100 && run_time < 1000 ) ++iter->second.over_100us_count;
    else ++iter->second.over_1000us_count;
#endif
}

void Profiler::countVar( const std::string & var_name, int count )
{
#if defined( __DEBUG__ )
    auto iter = m_Data.find( var_name );
    if ( iter == m_Data.end() ) {
        m_Data.emplace( var_name, profilerInfo { var_name, ct_Var } );
        iter = m_Data.find( var_name );
        iter->second.min = 0;
        iter->second.max = 0;
    }

    ++iter->second.count;
    iter->second.total += count;
    if ( count < iter->second.min ) iter->second.min = count;
    if ( count > iter->second.max ) iter->second.max = count;
#endif
}

template<typename T>
std::string to_string_with_precision( const T a_value, const int n = 4 )
{
    std::ostringstream out;
    out.precision( n );
    out << std::fixed << a_value;
    return out.str();
}

void Profiler::printResult()
{
#if defined( __DEBUG__ )
    constexpr auto func_len = 80;
    constexpr auto val_len = 14;
    constexpr auto rate_len = 10;

    int run_time = utils::time() - m_start;

    struct tm timestruct;
    time_t now_t = utils::time();

    localtime_r( &now_t, &timestruct );
    char filename[PATH_MAX];
    snprintf( filename, sizeof( filename ),
            "debug/%s-%04d-%02d-%02d-%02d:%02d:%02d.prof",
            m_module.empty() ? "profiler" : m_module.c_str(),
            timestruct.tm_year + 1900, timestruct.tm_mon + 1, timestruct.tm_mday, timestruct.tm_hour, timestruct.tm_min, timestruct.tm_sec );

    std::ofstream ss( filename );

    ss << "total run " << run_time << " seconds!\n";
    ss << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    ss << std::setw( func_len ) << std::left << "| Function";
    ss << std::setw( val_len ) << std::left << "| MIN";
    ss << std::setw( val_len ) << std::left << "| AVG";
    ss << std::setw( val_len ) << std::left << "| MAX";
    ss << std::setw( val_len ) << std::left << "| 10us";
    ss << std::setw( rate_len ) << std::left << "| rate";
    ss << std::setw( val_len ) << std::left << "| 10-100us";
    ss << std::setw( rate_len ) << std::left << "| rate";
    ss << std::setw( val_len ) << std::left << "| 100-1000us";
    ss << std::setw( rate_len ) << std::left << "| rate";
    ss << std::setw( val_len ) << std::left << "| over_1000us";
    ss << std::setw( rate_len ) << std::left << "| rate";
    ss << std::setw( val_len ) << std::left << "| TOTAL";
    ss << "|\n";

    ss << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    std::string temp;
    double rate = 0.0;
    for ( const auto & iter : m_Data ) {

        if ( iter.second.count <= 0 ) continue;

        ss << "|  " << std::setw( func_len - 3 ) << std::left << iter.second.name;
        if ( iter.second.type == ct_Func ) {
            temp = "|  " + to_string_with_precision( double( iter.second.min / 1000.0 ) ) + " ms"; //min
            ss << std::setw( val_len ) << std::left << temp;

            temp = "|  " + to_string_with_precision( double( iter.second.total / iter.second.count / 1000.0 ) ) + " ms"; //avg
            ss << std::setw( val_len ) << std::left << temp;

            temp = "|  " + to_string_with_precision( double( iter.second.max / 1000.0 ) ) + " ms"; //max
            ss << std::setw( val_len ) << std::left << temp;

            temp = "|  " + std::to_string( iter.second.over_1us_count ); //[1-10)us
            ss << std::setw( val_len ) << std::left << temp;

            temp = "| " + to_string_with_precision( iter.second.over_1us_count / (double)iter.second.count * 100.0, 2 ) + "%";
            ss << std::setw( rate_len ) << std::left << temp;

            temp = "|  " + std::to_string( iter.second.over_10us_count ); //[10-100)us
            ss << std::setw( val_len ) << std::left << temp;

            temp = "| " + to_string_with_precision( iter.second.over_10us_count / (double)iter.second.count * 100.0, 2 ) + "%";
            ss << std::setw( rate_len ) << std::left << temp;

            temp = "|  " + std::to_string( iter.second.over_100us_count ); //[100-1000)us
            ss << std::setw( val_len ) << std::left << temp;

            temp = "| " + to_string_with_precision( iter.second.over_100us_count / (double)iter.second.count * 100.0, 2 ) + "%";
            ss << std::setw( rate_len ) << std::left << temp;

            temp = "|  " + std::to_string( iter.second.over_1000us_count ); //[1000- .us
            ss << std::setw( val_len ) << std::left << temp;

            temp = "| " + to_string_with_precision( iter.second.over_1000us_count / (double)iter.second.count * 100.0, 2 ) + "%";
            ss << std::setw( rate_len ) << std::left << temp;
        } else {
            temp = "|  " + std::to_string( iter.second.min );
            ss << std::setw( val_len ) << std::left << temp;

            temp = "|  " + std::to_string( double( iter.second.total / iter.second.count ) );
            ss << std::setw( val_len ) << std::left << temp;

            temp = "|  " + std::to_string( iter.second.max );
            ss << std::setw( val_len ) << std::left << temp;

            for ( int i = 0; i < 4; ++i ) {
                temp = "|  ";
                ss << std::setw( val_len ) << std::left << temp;
                temp = "|  ";
                ss << std::setw( rate_len ) << std::left << temp;
            }
        }
        temp = "|  " + std::to_string( iter.second.count );
        ss << std::setw( val_len ) << std::left << temp;
        ss << "|\n";
    }
    ss << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

    ss.flush();
    ss.close();
#endif
}

