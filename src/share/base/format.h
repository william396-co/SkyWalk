#pragma once

#include "types.h"

#include <format>


template<> struct std::formatter<LoginMode> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const LoginMode & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return std::format_to( fc.out(), "{}", (int32_t)value );
        }
};

template<> struct std::formatter<SyncMethod> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const SyncMethod & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return std::format_to( fc.out(), "{}", (int32_t)value );
        }
};
