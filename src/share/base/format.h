#pragma once

#include "types.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "fmt/core.h"
#include "fmt/ranges.h"

template<> struct fmt::formatter<LoginMode> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const LoginMode & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return fmt::format_to( fc.out(), "{}", (int32_t)value );
        }
};

template<> struct fmt::formatter<SyncMethod> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const SyncMethod & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return fmt::format_to( fc.out(), "{}", (int32_t)value );
        }
};
