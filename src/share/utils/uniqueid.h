#pragma once
#include <stdint.h>

namespace utils {

    //
    // Twitter-Snowflake
    // https://github.com/twitter/snowflake
    // 可以使用69.7年(可以灵活的变更m_Epoch)
    //
    class UniqueID
    {
    public:
        UniqueID(uint32_t machine);
        ~UniqueID();
        uint64_t generate();
        static uint32_t machine(uint64_t id);
        uint32_t machine() const { return m_Machine; }

    private:
        uint64_t nextmill();
        static uint64_t epoch(const char* str);


    private:
        uint64_t m_Epoch;    //
        uint32_t m_Machine;  // 机器号
        uint64_t m_Current;  // 当前的毫秒数
        uint32_t m_Sequence; // 1ms内的序列号
    };

} // namespace utils
