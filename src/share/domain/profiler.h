#pragma once

#include <chrono>
#include <stack>
#include <unordered_map>
#include <string>
#include <climits>

#include "utils/singleton.h"
#include "utils/xtime.h"


class Profiler : public Singleton<Profiler> {
	friend class Singleton<Profiler>;
    enum Type { ct_None,ct_Var,ct_Func };
    struct profilerInfo
    {
        profilerInfo( const std::string & name, Type type )
            : name { name }, type { type } { start_list.emplace( utils::current() ); }

        std::string name;
        int min = INT_MAX;
        int max = INT_MIN;
        uint32_t over_1us_count = 0;    //[1-10)us
        uint32_t over_10us_count = 0;   //[10-100)us
        uint32_t over_100us_count = 0;  //[100-1000)us
        uint32_t over_1000us_count = 0; //[1000- .us
        uint32_t count = 0;
        uint64_t total = 0;
        std::stack<uint64_t> start_list; // recurse call
        Type type = ct_None;
    };

public:
    void startFunc( const std::string & func_name );
    void endFunc( const std::string & func_name );
    void setModule( const std::string & module ) { m_module = module; }
    void countVar( const std::string & var_name, int count );
    //打印结果
    void printResult();

private:
    std::string m_module;
    time_t m_start = utils::time();
    std::unordered_map<std::string, profilerInfo> m_Data;
};

#define COUNT_VAR(VAR_NAME,CNT)\
do{\
    Profiler::instance().countVar(VAR_NAME,CNT);\
}while(0)

#define START_FUNC(FUNC_NAME)\
do{\
    Profiler::instance().startFunc(FUNC_NAME);\
}while(0)

#define END_FUNC(FUNC_NAME)\
do{\
    Profiler::instance().endFunc(FUNC_NAME);\
}while(0)

#define PRINT_RES()\
do{\
    Profiler::instance().printResult();\
}while(0)

#define PROFILE_MODULE(MODULE)\
do{\
	Profiler::instance().setModule(MODULE);\
}while(0)

