
#include <stdint.h>

#include "base64.h"

/*
** Translation Table as described in RFC1113
*/
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

#define B64_DEF_LINE_SIZE 72
#define B64_MIN_LINE_SIZE 4

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
static void encodeblock( uint8_t * in, uint8_t * out, uint32_t len )
{
    out[0] = (uint8_t)cb64[( int32_t )( in[0] >> 2 )];
    out[1] = (uint8_t)cb64[( int32_t )( ( ( in[0] & 0x03 ) << 4 ) | ( ( in[1] & 0xf0 ) >> 4 ) )];
    out[2] = ( uint8_t )( len > 1 ? cb64[( int32_t )( ( ( in[1] & 0x0f ) << 2 ) | ( ( in[2] & 0xc0 ) >> 6 ) )] : '=' );
    out[3] = ( uint8_t )( len > 2 ? cb64[( int32_t )( in[2] & 0x3f )] : '=' );
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
static uint32_t b64_encode( const char * instr, char * outstr, uint32_t inlen )
{
    uint8_t in[3];
    uint8_t out[4];
    uint32_t out_cur = 0;
    uint32_t in_cur = 0;
    uint32_t i = 0;
    uint32_t len = 0;

    *in = (uint8_t)0;
    *out = (uint8_t)0;
    while ( in_cur < inlen ) {
        len = 0;
        for ( i = 0; i < 3; ++i, ++in_cur ) {
            in[i] = (uint8_t)instr[in_cur];
            if ( in_cur < inlen ) {
                ++len;
            } else {
                in[i] = 0;
            }
        }
        if ( len > 0 ) {
            encodeblock( in, out, len );
            i = 0;
            for ( i = 0; i < 4; ++i, ++out_cur ) {
                outstr[out_cur] = out[i];
            }
        }
    }

    return out_cur;
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
static void decodeblock( uint8_t * in, uint8_t * out )
{
    out[0] = ( uint8_t )( in[0] << 2 | in[1] >> 4 );
    out[1] = ( uint8_t )( in[1] << 4 | in[2] >> 2 );
    out[2] = ( uint8_t )( ( ( in[2] << 6 ) & 0xc0 ) | in[3] );
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
static uint32_t b64_decode( const char * instr, char * outstr, uint32_t inlen )
{
    uint8_t in[4], out[3], v;
    uint32_t inc = 0, outc = 0, i, len;

    /* only size of buffer required */
    if ( !outstr ) {
        while ( inc < inlen ) {
            for ( len = 0, i = 0; i < 4 && inc < inlen; ++i ) {
                v = 0;
                while ( ( inc <= inlen ) && ( v == 0 ) ) {
                    v = ( inc < inlen ) ? instr[inc] : 0;
                    ++inc;

                    v = ( uint8_t )( ( v < 43 || v > 122 ) ? 0 : cd64[v - 43] );
                    if ( v ) {
                        v = ( uint8_t )( ( v == '$' ) ? 0 : v - 61 );
                    }
                }
                if ( inc <= inlen ) {
                    ++len;
                }
            }
            if ( len > 1 ) {
                outc += ( len - 1 );
            }
        }

        return outc;
    }

    while ( inc < inlen ) {
        for ( len = 0, i = 0; i < 4 && inc < inlen; ++i ) {
            v = 0;
            while ( ( inc <= inlen ) && ( v == 0 ) ) {
                v = ( inc < inlen ) ? instr[inc] : 0;
                ++inc;

                v = ( uint8_t )( ( v < 43 || v > 122 ) ? 0 : cd64[v - 43] );
                if ( v ) {
                    v = ( uint8_t )( ( v == '$' ) ? 0 : v - 61 );
                }
            }
            if ( inc <= inlen ) {
                ++len;
                if ( v ) {
                    in[i] = ( uint8_t )( v - 1 );
                }
            } else {
                in[i] = 0;
            }
        }
        if ( len ) {
            decodeblock( in, out );
            for ( i = 0; i < len - 1; ++i ) {
                outstr[outc++] = out[i];
            }
        }
    }

    return outc;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Base64::encode( const std::string & src, std::string & dst )
{
    uint32_t dstlen = 0;
    uint32_t srclen = src.size();

    dst.resize( srclen * 4 / 3 + 4 );
    char * dstptr = (char *)dst.data();

    dstlen = b64_encode( src.data(), dstptr, srclen );
    dst.resize( dstlen );
}

void Base64::decode( const std::string & src, std::string & dst )
{
    uint32_t dstlen = 0;
    uint32_t srclen = src.size();

    dst.resize( srclen );
    char * dstptr = (char *)dst.data();

    dstlen = b64_decode( src.data(), dstptr, srclen );
    dst.resize( dstlen );
}
