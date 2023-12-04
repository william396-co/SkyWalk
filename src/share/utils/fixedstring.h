#pragma once

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdint>

// TODO this can replace by std::array<char,size> or just string

template<int32_t SIZE>
class FixedString
{
public:
    FixedString() : m_Offset( 0 ) {}
    ~FixedString() {}

public:
    template<typename T>
        void append( const char * fmt, T value )
        {
            int32_t addon = 0;

            addon = std::snprintf(
                m_Buffer + m_Offset, left() - 1, fmt, value );
            if ( addon > 0 && (size_t)addon < left() ) {
                m_Offset += addon;
            }
        }

    void append( char c )
    {
        assert( m_Offset + 1 < sizeof( m_Buffer ) );
        m_Buffer[m_Offset] = c;
        m_Offset += 1;
    }

    void append( const char * buf )
    {
        size_t len = strlen( buf );

        assert( m_Offset + len < sizeof( m_Buffer ) );
        std::memcpy( m_Buffer + m_Offset, buf, len );
        m_Offset += len;
    }

    void append( const char * buf, size_t len )
    {
        assert( m_Offset + len < sizeof( m_Buffer ) );
        std::memcpy( m_Buffer + m_Offset, buf, len );
        m_Offset += len;
    }

    size_t size() const { return m_Offset; }
    const char * data() const { return m_Buffer; }
    const char * avail() const { return m_Buffer + m_Offset; }
    size_t left() const { return sizeof( m_Buffer ) - m_Offset; }

private:
    char m_Buffer[SIZE];
    size_t m_Offset;
};

