#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>

// TODO- use string-view replace it

class Slice
{
public:
    // Create an empty slice.
    Slice()
        : m_Data( "" ),
          m_Size( 0 )
    {}

    // Create a slice that refers to d[0,n-1].
    Slice( const char * d, size_t n )
        : m_Data( d ),
          m_Size( n )
    {}

    // Create a slice that refers to the contents of "s"
    Slice( const std::string & s )
        : m_Data( s.data() ),
          m_Size( s.size() )
    {}

    // Create a slice that refers to s[0,strlen(s)-1]
    Slice( const char * s )
        : m_Data( s ),
          m_Size( std::strlen( s ) )
    {}

    // Return a pointer to the beginning of the referenced data
    const char * data() const { return m_Data; }

    // Return the length (in bytes) of the referenced data
    size_t size() const { return m_Size; }

    // Return true iff the length of the referenced data is zero
    bool empty() const { return m_Size == 0; }

    // Return the ith byte in the referenced data.
    // REQUIRES: n < size()
    char operator[]( size_t n ) const
    {
        assert( n < size() );
        return m_Data[n];
    }

    // Change this slice to refer to an empty array
    void clear()
    {
        m_Data = "";
        m_Size = 0;
    }

    void free()
    {
        if ( m_Data != nullptr ) {
            std::free( (void *)m_Data );
            m_Data = nullptr;
        }
    }

    // Drop the first "n" bytes from this slice.
    void remove( size_t n )
    {
        assert( n <= size() );
        m_Data += n;
        m_Size -= n;
    }

    // Return a string that contains the copy of the referenced data.
    std::string ToString() const
    {
        return std::string( m_Data, m_Size );
    }

    // Three-way comparison.  Returns value:
    //   <  0 iff "*this" <  "b",
    //   == 0 iff "*this" == "b",
    //   >  0 iff "*this" >  "b"
    ssize_t compare( const Slice & b ) const;

    // Return true iff "x" is a prefix of "*this"
    bool startwith( const Slice & x ) const
    {
        return ( ( m_Size >= x.m_Size )
            && ( std::memcmp( m_Data, x.m_Data, x.m_Size ) == 0 ) );
    }

private:
    const char * m_Data;
    size_t m_Size;
};

inline bool operator==( const Slice & x, const Slice & y )
{
    return ( ( x.size() == y.size() )
        && ( std::memcmp( x.data(), y.data(), x.size() ) == 0 ) );
}

inline bool operator!=( const Slice & x, const Slice & y )
{
    return !( x == y );
}

inline ssize_t Slice::compare( const Slice & b ) const
{
    const size_t min_len = ( m_Size < b.m_Size ) ? m_Size : b.m_Size;
    ssize_t r = std::memcmp( m_Data, b.m_Data, min_len );
    if ( r == 0 ) {
        if ( m_Size < b.m_Size ) {
            r = -1;
        } else if ( m_Size > b.m_Size ) {
            r = +1;
        }
    }

    return r;
}

using Slices =std::vector<Slice>;

