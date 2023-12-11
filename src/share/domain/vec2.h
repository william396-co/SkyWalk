
#ifndef __SRC_DOMAIN_VEC2_H__
#define __SRC_DOMAIN_VEC2_H__

#include <cmath>
#include <stdint.h>

class Angle;

/// \brief 2D vector
///
/// These vector templates are defined for:\n
/// int8_t (Vec2c), uint8_t (Vec2uc), int16_t (Vec2s),\n
/// uint16_t (Vec2us), int (Vec2i), uint32_t (Vec2ui), float (Vec2f), double (Vec2d)
template<typename Type>
class Vec2
{
public:
    typedef Type value_type;
    typedef Vec2<float> float_type;
    typedef Vec2<double> double_type;
    static const int array_size;

    union { Type x; Type s; Type r; };
    union { Type y; Type t; Type g; };

    Vec2()
        : x( 0 ), y( 0 ) {}
    explicit Vec2( const Type & scalar )
        : x( scalar ), y( scalar ) {}
    explicit Vec2( const Type & p1, const Type & p2 )
        : x( p1 ), y( p2 ) {}
    explicit Vec2( const Type * array_xy )
        : x( array_xy[0] ), y( array_xy[1] ) {}

    Vec2( const Vec2<double> & copy );
    Vec2( const Vec2<float> & copy );
    Vec2( const Vec2<int32_t> & copy );

    /// \brief Normalizes a vector
    ///
    /// Operates in the native datatype
    ///
    /// \param vector = Vector to use
    /// \return normalized vector
    static Vec2<Type> normalize( const Vec2<Type> & vector );

    /// \brief Dot products a vector with an other vector.
    ///
    /// Operates in the native datatype
    ///
    /// \param vector_1 = First vector used for the dot product.
    /// \param vector_2 = Second vector used for the dot product.
    /// \return The resulting dot product of the two vectors.
    static Type dot( const Vec2<Type> & vector_1, const Vec2<Type> & vector_2 ) { return vector_1.x * vector_2.x + vector_1.y * vector_2.y; }

    /// \brief Rounds all components on a vector
    ///
    /// Uses Asymmetric Arithmetic Rounding
    ///
    /// \param vector = Vector to use
    /// \return The rounded vector
    static Vec2<Type> round( const Vec2<Type> & vector );

    /// \brief Rotate a vector around another point.
    ///
    /// \param vector = Vector to use
    /// \param hotspot The point around which to rotate.
    /// \param angle = Angle to rotate.
    static Vec2<Type> rotate( const Vec2<Type> & vector, const Vec2<Type> & hotspot, const Angle & angle );

    /// \brief Returns true if equal within the bounds of an epsilon
    ///
    /// \param first = Value A
    /// \param second = Value B
    /// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
    static bool is_equal( const Vec2<Type> & first, const Vec2<Type> & second, Type epsilon )
    {
        Type diff_x = second.x - first.x;
        Type diff_y = second.y - first.y;
        return ( diff_x >= -epsilon && diff_x <= epsilon && diff_y >= -epsilon && diff_y <= epsilon );
    }

    /// \name Attributes
    /// \{

public:
    /// \brief Returns the length (magnitude) of this vector.
    ///
    /// Operates in the native datatype
    ///
    /// \return the length of the vector
    Type length() const;

    /// \brief Normalizes this vector
    ///
    /// Operates in the native datatype
    ///
    /// \return reference to this object
    Vec2<Type> & normalize();

    /// \brief Dot products this vector with an other vector.
    ///
    /// Operates in the native datatype
    ///
    /// \param vector Second vector used for the dot product.
    ///
    /// \return The resulting dot product of the two vectors.
    Type dot( const Vec2<Type> & vector ) const { return x * vector.x + y * vector.y; }

    /// \brief Calculate the angle between this vector and an other vector.
    ///
    /// \param vector = Second vector used to calculate angle.
    ///
    /// \return The angle between the two vectors
    Angle angle( const Vec2<Type> & vector ) const;

    /// \brief Calculate the angle between this vector and an other vector, where the vectors are unit vectors
    ///
    /// \param vector = Second vector used to calculate angle.
    ///
    /// \return The angle between the two vectors
    Angle angle_normed( const Vec2<Type> & vector ) const;

    /// \brief Calculate the angle of the line joining this point and other point
    ///
    /// \param point = Second point in the line
    ///
    /// \return The angle
    Angle angle_line( const Vec2<Type> & point ) const;

    /// \brief Calculate the distance between this vector and an other vector.
    ///
    /// \param vector = Second vector used to calculate distance.
    ///
    /// \return The distance between the two vectors.
    Type distance( const Vec2<Type> & vector ) const;
    double squared_distance( const Vec2<Type> & vector ) const;

    /// \brief Rounds all components of this vector
    ///
    /// Uses Asymmetric Arithmetic Rounding
    ///
    /// \return reference to this object
    Vec2<Type> & round();

    /// \brief Rotate this vector around another point.
    ///
    /// \param hotspot The point around which to rotate.
    /// \param angle = Angle to rotate.
    ///
    /// \return reference to this object
    Vec2<Type> & rotate( const Vec2<Type> & hotspot, const Angle & angle );

    /// \brief Rounds a value for the datatype
    ///
    /// For doubles and floats, this function does not round.
    /// \param value = Value to round
    /// \return The rounded value
    Type round_value( float value ) const;

    /// \brief To Array
    ///
    ///
    void to_array( Type * array ) const
    {
        array[0] = x;
        array[1] = y;
    }

    /// \brief Returns true if equal within the bounds of an epsilon
    ///
    /// \param other = Other value
    /// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
    bool is_equal( const Vec2<Type> & other, Type epsilon ) const { return Vec2<Type>::is_equal( *this, other, epsilon ); }

    /// 是否在区间内
    bool is_in_region( const Vec2<Type> & leftbottom, const Vec2<Type> & righttop ) const
    {
        return leftbottom.x <= x && x <= righttop.x && leftbottom.y <= y && y <= righttop.y;
    }

    // 是否在x轴两点之间
    bool is_in_regionx( const Vec2<Type> & left, const Vec2<Type> & right ) const
    {
        if ( left.y != y && y != right.y ) {
            return false;
        }
        return right.x - left.x > 0 ? left.x <= x && x <= right.x : right.x <= x && x <= left.x;
    }

    // 是否在x轴两点之间
    bool is_in_regiony( const Vec2<Type> & left, const Vec2<Type> & right ) const
    {
        if ( left.x != x && x != right.x ) {
            return false;
        }
        return right.y - left.y > 0 ? left.y <= y && y <= right.y : right.y <= y && y <= left.y;
    }

    /// \}
    /// \name Operators
    /// \{

public:
    /// \brief += operator.
    void operator+=( const Vec2<Type> & vector )
    {
        x += vector.x;
        y += vector.y;
    }

    /// \brief += operator.
    void operator+=( Type value )
    {
        x += value;
        y += value;
    }

    /// \brief -= operator.
    void operator-=( const Vec2<Type> & vector )
    {
        x -= vector.x;
        y -= vector.y;
    }

    /// \brief -= operator.
    void operator-=( Type value )
    {
        x -= value;
        y -= value;
    }

    /// \brief - operator.
    Vec2<Type> operator-() const { return Vec2<Type>( -x, -y ); }

    /// \brief *= operator.
    void operator*=( const Vec2<Type> & vector )
    {
        x *= vector.x;
        y *= vector.y;
    }

    /// \brief *= operator.
    void operator*=( Type value )
    {
        x *= value;
        y *= value;
    }

    /// \brief /= operator.
    void operator/=( const Vec2<Type> & vector )
    {
        x /= vector.x;
        y /= vector.y;
    }

    /// \brief /= operator.
    void operator/=( Type value )
    {
        x /= value;
        y /= value;
    }

    /// \brief = operator.
    Vec2<Type> & operator=( const Vec2<Type> & vector )
    {
        x = vector.x;
        y = vector.y;
        return *this;
    }

    /// \brief == operator.
    bool operator==( const Vec2<Type> & vector ) const { return ( ( x == vector.x ) && ( y == vector.y ) ); }

    /// \brief != operator.
    bool operator!=( const Vec2<Type> & vector ) const { return ( ( x != vector.x ) || ( y != vector.y ) ); }

    /// \brief < operator.
    bool operator<( const Vec2<Type> & vector ) const { return y < vector.y || ( y == vector.y && x < vector.x ); }

    /// \brief > operator.
    bool operator>( const Vec2<Type> & vector ) const { return y > vector.y || ( y == vector.y && x > vector.x ); }
    /// \}
};

template<typename Type> const int Vec2<Type>::array_size = 2;

/// \brief + operator.
template<typename Type>
Vec2<Type> operator+( const Vec2<Type> & v1, const Vec2<Type> & v2 )
{
    return Vec2<Type>( v1.x + v2.x, v1.y + v2.y );
}

/// \brief + operator.
template<typename Type>
Vec2<Type> operator+( Type s, const Vec2<Type> & v )
{
    return Vec2<Type>( s + v.x, s + v.y );
}

/// \brief + operator.
template<typename Type>
Vec2<Type> operator+( const Vec2<Type> & v, Type s )
{
    return Vec2<Type>( v.x + s, v.y + s );
}

/// \brief - operator.
template<typename Type>
Vec2<Type> operator-( const Vec2<Type> & v1, const Vec2<Type> & v2 )
{
    return Vec2<Type>( v1.x - v2.x, v1.y - v2.y );
}

/// \brief - operator.
template<typename Type>
Vec2<Type> operator-( Type s, const Vec2<Type> & v )
{
    return Vec2<Type>( s - v.x, s - v.y );
}

/// \brief - operator.
template<typename Type>
Vec2<Type> operator-( const Vec2<Type> & v, Type s )
{
    return Vec2<Type>( v.x - s, v.y - s );
}

/// \brief * operator.
template<typename Type>
Vec2<Type> operator*( const Vec2<Type> & v1, const Vec2<Type> & v2 )
{
    return Vec2<Type>( v1.x * v2.x, v1.y * v2.y );
}

/// \brief * operator.
template<typename Type>
Vec2<Type> operator*( Type s, const Vec2<Type> & v )
{
    return Vec2<Type>( s * v.x, s * v.y );
}

/// \brief * operator.
template<typename Type>
Vec2<Type> operator*( const Vec2<Type> & v, Type s )
{
    return Vec2<Type>( v.x * s, v.y * s );
}

/// \brief / operator.
template<typename Type>
Vec2<Type> operator/( const Vec2<Type> & v1, const Vec2<Type> & v2 )
{
    return Vec2<Type>( v1.x / v2.x, v1.y / v2.y );
}

/// \brief / operator.
template<typename Type>
Vec2<Type> operator/( Type s, const Vec2<Type> & v )
{
    return Vec2<Type>( s / v.x, s / v.y );
}

/// \brief / operator.
template<typename Type>
Vec2<Type> operator/( const Vec2<Type> & v, Type s )
{
    return Vec2<Type>( v.x / s, v.y / s );
}

//////////////////////////////////////////////////////////////////////////

template<>
inline Vec2<uint8_t>::Vec2( const Vec2<float> & copy )
{
    x = (uint8_t)std::floor( copy.x + 0.5f );
    y = (uint8_t)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<uint8_t>::Vec2( const Vec2<double> & copy )
{
    x = (uint8_t)std::floor( copy.x + 0.5 );
    y = (uint8_t)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<uint8_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (uint8_t)copy.x;
    y = (uint8_t)copy.y;
}

template<>
inline Vec2<int8_t>::Vec2( const Vec2<float> & copy )
{
    x = (int8_t)std::floor( copy.x + 0.5f );
    y = (int8_t)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<int8_t>::Vec2( const Vec2<double> & copy )
{
    x = (int8_t)std::floor( copy.x + 0.5 );
    y = (int8_t)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<int8_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (int8_t)copy.x;
    y = (int8_t)copy.y;
}

template<>
inline Vec2<uint16_t>::Vec2( const Vec2<float> & copy )
{
    x = (uint16_t)std::floor( copy.x + 0.5f );
    y = (uint16_t)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<uint16_t>::Vec2( const Vec2<double> & copy )
{
    x = (uint16_t)std::floor( copy.x + 0.5 );
    y = (uint16_t)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<uint16_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (uint16_t)copy.x;
    y = (uint16_t)copy.y;
}

template<>
inline Vec2<int16_t>::Vec2( const Vec2<float> & copy )
{
    x = (int16_t)std::floor( copy.x + 0.5f );
    y = (int16_t)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<int16_t>::Vec2( const Vec2<double> & copy )
{
    x = (int16_t)std::floor( copy.x + 0.5 );
    y = (int16_t)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<int16_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (int16_t)copy.x;
    y = (int16_t)copy.y;
}

template<>
inline Vec2<int32_t>::Vec2( const Vec2<float> & copy )
{
    x = (int)std::floor( copy.x + 0.5f );
    y = (int)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<int32_t>::Vec2( const Vec2<double> & copy )
{
    x = (int)std::floor( copy.x + 0.5 );
    y = (int)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<int32_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (int)copy.x;
    y = (int)copy.y;
}

template<>
inline Vec2<uint32_t>::Vec2( const Vec2<float> & copy )
{
    x = (uint32_t)std::floor( copy.x + 0.5f );
    y = (uint32_t)std::floor( copy.y + 0.5f );
}

template<>
inline Vec2<uint32_t>::Vec2( const Vec2<double> & copy )
{
    x = (uint32_t)std::floor( copy.x + 0.5 );
    y = (uint32_t)std::floor( copy.y + 0.5 );
}

template<>
inline Vec2<uint32_t>::Vec2( const Vec2<int32_t> & copy )
{
    x = (uint32_t)copy.x;
    y = (uint32_t)copy.y;
}

template<>
inline Vec2<float>::Vec2( const Vec2<float> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
}

template<>
inline Vec2<float>::Vec2( const Vec2<double> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
}

template<>
inline Vec2<float>::Vec2( const Vec2<int32_t> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
}

template<>
inline Vec2<double>::Vec2( const Vec2<float> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
}

template<>
inline Vec2<double>::Vec2( const Vec2<double> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
}

template<>
inline Vec2<double>::Vec2( const Vec2<int32_t> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
}

template<typename Type>
inline Type Vec2<Type>::length() const
{
    return (Type)floor( sqrt( float( x * x + y * y ) ) + 0.5f );
}

template<>
inline double Vec2<double>::length() const
{
    return sqrt( x * x + y * y );
}

template<>
inline float Vec2<float>::length() const
{
    return sqrt( x * x + y * y );
}

template<typename Type>
inline Vec2<Type> & Vec2<Type>::normalize()
{
    Type f = length();
    if ( f != 0 ) {
        x /= f;
        y /= f;
    }
    return *this;
}

template<typename Type>
inline Vec2<Type> Vec2<Type>::normalize( const Vec2<Type> & vector )
{
    Vec2<Type> dest( vector );
    dest.normalize();
    return dest;
}

template<typename Type>
inline Vec2<Type> Vec2<Type>::round( const Vec2<Type> & vector )
{
    Vec2<Type> dest( vector );
    dest.round();
    return dest;
}

template<typename Type>
inline Vec2<Type> & Vec2<Type>::round()
{
    x = (Type)floorf( x + 0.5f );
    y = (Type)floorf( y + 0.5f );
    return *this;
}

template<typename Type>
inline Type Vec2<Type>::round_value( float value ) const
{
    return (Type)value;
}

template<>
inline int32_t Vec2<int32_t>::round_value( float value ) const
{
    return (int32_t)floorf( value + 0.5f );
}

template<>
inline uint32_t Vec2<uint32_t>::round_value( float value ) const
{
    return (uint32_t)floorf( value + 0.5f );
}

template<>
inline int16_t Vec2<int16_t>::round_value( float value ) const
{
    return (int16_t)floorf( value + 0.5f );
}

template<>
inline uint16_t Vec2<uint16_t>::round_value( float value ) const
{
    return (uint16_t)floorf( value + 0.5f );
}

template<>
inline int8_t Vec2<int8_t>::round_value( float value ) const
{
    return (int8_t)floorf( value + 0.5f );
}

template<>
inline uint8_t Vec2<uint8_t>::round_value( float value ) const
{
    return (uint8_t)floorf( value + 0.5f );
}

// For integers
template<typename Type>
inline Type Vec2<Type>::distance( const Vec2<Type> & vector ) const
{
    float value_x = x - vector.x;
    float value_y = y - vector.y;
    return (Type)floor( sqrt( value_x * value_x + value_y * value_y ) + 0.5f );
}

template<typename Type>
inline double Vec2<Type>::squared_distance( const Vec2<Type> & vector ) const
{
    double value_x = x - vector.x;
    double value_y = y - vector.y;
    return (Type)( value_x * value_x + value_y * value_y );
}

// For floats
template<>
inline float Vec2<float>::distance( const Vec2<float> & vector ) const
{
    float value_x = x - vector.x;
    float value_y = y - vector.y;
    return sqrt( value_x * value_x + value_y * value_y );
}
template<>
inline double Vec2<float>::squared_distance( const Vec2<float> & vector ) const
{
    double value_x = x - vector.x;
    double value_y = y - vector.y;
    return value_x * value_x + value_y * value_y;
}

// For doubles
template<>
inline double Vec2<double>::distance( const Vec2<double> & vector ) const
{
    double value_x = x - vector.x;
    double value_y = y - vector.y;
    return sqrt( value_x * value_x + value_y * value_y );
}
template<>
inline double Vec2<double>::squared_distance( const Vec2<double> & vector ) const
{
    double value_x = x - vector.x;
    double value_y = y - vector.y;
    return value_x * value_x + value_y * value_y;
}

//////////////////////////////////////////////////////////////////////////

typedef Vec2<uint8_t> Vec2ub;
typedef Vec2<int8_t> Vec2b;
typedef Vec2<uint16_t> Vec2us;
typedef Vec2<int16_t> Vec2s;
typedef Vec2<uint32_t> Vec2ui;
typedef Vec2<int32_t> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

#endif
