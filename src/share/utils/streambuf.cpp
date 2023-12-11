
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "xmath.h"
#include "endian.h"
#include "streambuf.h"

// 模板原型
template<class T>
    bool StreamBuf::decode( T & data )
    {
        assert( m_Method == eMethod_Decode );
        return true;
    }

template<class T>
    bool StreamBuf::encode( const T & data )
    {
        assert( m_Method == eMethod_Encode );
        return true;
    }

StreamBuf::StreamBuf( const char * buf, size_t len, int8_t endian )
    : m_Endian( endian ),
      m_Method( eMethod_Decode ),
      m_Buffer( (char *)buf ),
      m_Length( len ),
      m_Size( 0 ),
      m_Offset( 0 ),
      m_Position( 0 ),
      m_IsFixed( true ),
      m_IsSelfFree( false )
{}

StreamBuf::StreamBuf( size_t len, size_t offset, int8_t endian )
    : m_Endian( endian ),
      m_Method( eMethod_Encode ),
      m_Buffer( nullptr ),
      m_Length( len ),
      m_Size( 0 ),
      m_Offset( offset ),
      m_Position( offset ),
      m_IsFixed( false ),
      m_IsSelfFree( true )
{
    if ( len != 0 ) {
        m_Buffer = (char *)std::calloc( 1, len );
        assert( m_Buffer != nullptr && "allocate for m_Buffer failed" );
    }
}

StreamBuf::StreamBuf( char * buf, size_t len, int8_t endian )
    : m_Endian( endian ),
      m_Method( eMethod_Encode ),
      m_Buffer( buf ),
      m_Length( len ),
      m_Size( 0 ),
      m_Offset( 0 ),
      m_Position( 0 ),
      m_IsFixed( false ),
      m_IsSelfFree( true )
{}

StreamBuf::~StreamBuf()
{
    if ( m_IsSelfFree
        && m_Buffer != nullptr ) {
        std::free( m_Buffer );
        m_Buffer = nullptr;
    }
}

bool StreamBuf::decode( bool & data )
{
    uint8_t d = 0;
    bool rc = false;

    assert( m_Method == eMethod_Decode );
    rc = decode( d );
    data = d == 1;

    return rc;
}

bool StreamBuf::decode( int8_t & data )
{
    size_t len = sizeof( int8_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(int8_t *)( m_Buffer + m_Position );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( uint8_t & data )
{
    size_t len = sizeof( uint8_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(uint8_t *)( m_Buffer + m_Position );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( int16_t & data )
{
    size_t len = sizeof( int16_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(int16_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be16toh( data ) : le16toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( uint16_t & data )
{
    size_t len = sizeof( uint16_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(uint16_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be16toh( data ) : le16toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( int32_t & data )
{
    size_t len = sizeof( int32_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(int32_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be32toh( data ) : le32toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( uint32_t & data )
{
    size_t len = sizeof( uint32_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(uint32_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be32toh( data ) : le32toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( int64_t & data )
{
    size_t len = sizeof( int64_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(int64_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be64toh( data ) : le64toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( uint64_t & data )
{
    size_t len = sizeof( uint64_t );

    assert( m_Method == eMethod_Decode );
    if ( m_Size + len <= m_Length ) {
        data = *(uint64_t *)( m_Buffer + m_Position );
        data = ( m_Endian == eEndian_Big ) ? be64toh( data ) : le64toh( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::decode( Slice & data )
{
    size_t length = 0;

    assert( m_Method == eMethod_Decode );
    if ( decode( length ) ) {
        if ( m_Size + length <= m_Length ) {
            char * buf = m_Buffer + m_Position;
            m_Size += length;
            m_Position += length;
            data = Slice( buf, length );
            return true;
        }
    }

    data = Slice( nullptr, 0 );
    return false;
}

bool StreamBuf::decode( std::string & data )
{
    size_t length = 0;

    assert( m_Method == eMethod_Decode );
    if ( decode( length ) ) {
        assert( length >= 1 );
        if ( length >= 1
                && m_Size + length <= m_Length ) {
            data.resize( length - 1 );
            data.assign( m_Buffer + m_Position, length - 1 );
            m_Size += length;
            m_Position += length;
            return true;
        }
    }

    return false;
}

bool StreamBuf::decode( char * data, uint16_t & len )
{
    uint16_t length = 0;

    assert( m_Method == eMethod_Decode );
    if ( decode( length ) ) {
        assert( length >= 1 );
        if ( length >= 1
            && m_Size + length <= m_Length
            && m_Buffer[m_Position + length - 1] == 0 ) {
            std::memcpy( data, m_Buffer + m_Position, length );
            m_Size += length;
            m_Position += length;
            len = length - 1;
            return true;
        }
    }

    len = 0;
    data[len] = 0;

    return false;
}

bool StreamBuf::decode( char * data, uint32_t & len )
{
    uint32_t length = 0;

    assert( m_Method == eMethod_Decode );
    if ( decode( length ) ) {
        assert( length >= 1 );
        if ( length >= 1
            && m_Size + length <= m_Length
            && m_Buffer[m_Position + length - 1] == 0 ) {
            std::memcpy( data, m_Buffer + m_Position, length );
            m_Size += length;
            m_Position += length;
            len = length - 1;
            return true;
        }
    }

    len = 0;
    data[len] = 0;

    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool StreamBuf::encode( const bool & data )
{
    uint8_t d = data ? 1 : 0;
    assert( m_Method == eMethod_Encode );
    return encode( d );
}

bool StreamBuf::encode( const int8_t & data )
{
    size_t len = sizeof( int8_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(int8_t *)( m_Buffer + m_Position ) = data;
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const uint8_t & data )
{
    size_t len = sizeof( uint8_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(uint8_t *)( m_Buffer + m_Position ) = data;
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const int16_t & data )
{
    size_t len = sizeof( int16_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(int16_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe16( data ) : htole16( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const uint16_t & data )
{
    size_t len = sizeof( uint16_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(uint16_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe16( data ) : htole16( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const int32_t & data )
{
    size_t len = sizeof( int32_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(int32_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe32( data ) : htole32( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const uint32_t & data )
{
    size_t len = sizeof( uint32_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(uint32_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe32( data ) : htole32( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const int64_t & data )
{
    size_t len = sizeof( int64_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(int64_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe64( data ) : htole64( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const uint64_t & data )
{
    size_t len = sizeof( uint64_t );

    assert( m_Method == eMethod_Encode );
    if ( expand( len ) ) {
        *(uint64_t *)( m_Buffer + m_Position )
            = ( m_Endian == eEndian_Big ) ? htobe64( data ) : htole64( data );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

bool StreamBuf::encode( const Slice & data )
{
    size_t length = data.size();

    assert( m_Method == eMethod_Encode );
    if ( encode( length ) ) {
        if ( expand( length ) ) {
            std::memcpy( m_Buffer + m_Position,
                data.data() == nullptr ? "" : data.data(), length );
            m_Size += length;
            m_Position += length;
            return true;
        }
    }

    return false;
}

bool StreamBuf::encode( const std::string & data )
{
    size_t length = data.size() + 1;

    assert( m_Method == eMethod_Encode );
    if ( encode( length ) ) {
        if ( expand( length ) ) {
            std::memcpy( m_Buffer + m_Position, data.data(), length );
            m_Size += length;
            m_Position += length;
            return true;
        }
    }

    return false;
}

bool StreamBuf::encode( const char * data, uint16_t len )
{
    assert( m_Method == eMethod_Encode );

    uint16_t length = len + 1;
    data = ( data == nullptr ? "" : data );

    if ( encode( length ) ) {
        if ( expand( length ) ) {
            std::memcpy( m_Buffer + m_Position, data, length );
            m_Size += length;
            m_Position += length;
            return true;
        }
    }

    return false;
}

bool StreamBuf::encode( const char * data, uint32_t len )
{
    assert( m_Method == eMethod_Encode );

    uint32_t length = len + 1;
    data = ( data == nullptr ? "" : data );

    if ( encode( length ) ) {
        if ( expand( length ) ) {
            std::memcpy( m_Buffer + m_Position, data, length );
            m_Size += length;
            m_Position += length;
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool StreamBuf::append( const std::string & data )
{
    size_t length = data.size();

    assert( m_Method == eMethod_Encode );
    if ( expand( length ) ) {
        std::memcpy( m_Buffer + m_Position, data.data(), length );
        m_Size += length;
        m_Position += length;
        return true;
    }

    return false;
}

bool StreamBuf::append( const char * data, size_t len )
{
    assert( m_Method == eMethod_Encode );
    data = ( data == nullptr ? "" : data );
    if ( expand( len ) ) {
        std::memcpy( m_Buffer + m_Position, data, len );
        m_Size += len;
        m_Position += len;
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool StreamBuf::expand( size_t len )
{
    size_t size = len + m_Size + m_Offset;

    // 防止溢出
    if ( size < m_Size + m_Offset ) {
        return false;
    }

    if ( size > m_Length ) {
        if ( m_IsFixed ) {
            return false;
        }

        // 扩展长度避免溢出
        size_t newlength = utils::Math::roundpow2( size );
        if ( newlength < m_Length ) {
            return false;
        }

        char * newbuffer = (char *)std::realloc( m_Buffer, newlength );
        if ( newbuffer == nullptr ) {
            return false;
        }

        m_Buffer = newbuffer;
        m_Length = newlength;
    }

    return true;
}

void StreamBuf::clear()
{
    m_Buffer = nullptr;
    m_Length = 0;
    m_Size = 0;
    m_Offset = 0;
    m_Position = 0;
    m_IsFixed = false;
    m_IsSelfFree = true;
}

void StreamBuf::reset()
{
    m_Offset = 0;
    m_Position = 0;
    m_IsFixed = true;
}

Slice StreamBuf::slice()
{
    Slice s( data(), size() );

    clear();
    return s;
}

std::string StreamBuf::string() const
{
    return std::string( data(), size() );
}
