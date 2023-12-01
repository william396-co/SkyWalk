#include "uniqueid.h"
#include <cassert>
#include <iomanip>
#include <ctime>
#include <time.h>
#include <cstring>
#include <string.h>
#include "xtime.h"

namespace utils {

    constexpr auto MACHINE_BITS = 14;
    constexpr auto SEQUENCE_BITS = 8;

    UniqueID::UniqueID(uint32_t machine)
        : m_Epoch(epoch("2022-01-01 00:00:00")),
        m_Machine(machine),
        m_Current(utils::now()),
        m_Sequence(0)
    {
        assert(m_Epoch != 0);
        assert(m_Machine <= (1 << MACHINE_BITS));
    }

    UniqueID::~UniqueID()
    {
        m_Epoch = 0;
        m_Machine = 0;
        m_Current = 0;
        m_Sequence = 0;
    }

    uint64_t UniqueID::generate()
    {
        uint64_t id = 0;
        uint64_t current = utils::now();

        assert(m_Current <= current && "Clock moved backwards");

        if (current != m_Current) {
            m_Sequence = 0;
            m_Current = current;
        }
        else {
            ++m_Sequence;
            m_Sequence &= ((1 << SEQUENCE_BITS) - 1);

            // 序列号用完，等待1ms
            if (m_Sequence == 0) {
                current = nextmill();
                m_Current = current;
            }
        }

        id = (current - m_Epoch) << (MACHINE_BITS + SEQUENCE_BITS);
        id |= (uint64_t)m_Machine << SEQUENCE_BITS;
        id |= m_Sequence;

        return id;
    }

    uint32_t UniqueID::machine(uint64_t id)
    {
        id >>= SEQUENCE_BITS;
        id &= ((1ULL << MACHINE_BITS) - 1);

        return (uint32_t)id;
    }

    uint64_t UniqueID::nextmill()
    {
        uint64_t current = 0;
        while ((current = utils::now()) <= m_Current);
        return current;
    }

    uint64_t UniqueID::epoch(const char* str)
    {
        //
        // 2010-10-10 00:00:00
        // 1286640000000
        //

        struct tm t;
        char* matched = nullptr;

        memset(&t, 0, sizeof(t));
      //  matched = strptime(str, "%Y-%m-%d %H:%M:%S", &t);

        return matched != nullptr ? mktime(&t) * 1000ULL : 1286640000000;
    }

} // namespace utils
