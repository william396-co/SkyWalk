
#ifndef __SRC_DOMAIN_VEC3_H__
#define __SRC_DOMAIN_VEC3_H__

#include <math.h>
#include <stdint.h>

class Angle;

/// \brief 3D vector
///
/// These vector templates are defined for:\n
/// int8_t (Vec3c), uint8_t (Vec3uc), int16_t (Vec3s),\n
/// uint16_t (Vec3us), int (Vec3i), uint32_t (Vec3ui), float (Vecdf), double (Vec3d)
template<typename Type>
class Vec3
{
public:
    typedef Type value_type;
    typedef Vec3<float> float_type;
    typedef Vec3<double> double_type;
    static const int array_size;

    union { Type x; Type s; Type r; };
    union { Type y; Type t; Type g; };
    union { Type z; Type u; Type b; };

    Vec3()
        : x( 0 ), y( 0 ), z( 0 ) {}
    explicit Vec3( const Type & scalar )
        : x( scalar ), y( scalar ), z( scalar ) {}

    Vec3( const Vec3<double> & copy );
    Vec3( const Vec3<float> & copy );
    Vec3( const Vec3<int32_t> & copy );

    explicit Vec3( const Type & p1, const Type & p2, const Type & p3 )
        : x( p1 ), y( p2 ), z( p3 ) {}
    explicit Vec3( const Type * array_xyz )
        : x( array_xyz[0] ), y( array_xyz[1] ), z( array_xyz[2] ) {}

    /// \brief Normalizes a vector
    ///
    /// \param vector = Vector to use
    ///
    /// Operates in the native datatype
    static Vec3<Type> normalize( const Vec3<Type> & vector );

    /// \brief Dot products between two vectors.
    ///
    /// \return The dot product
    static Type dot( const Vec3<Type> & vector1, const Vec3<Type> & vector2 ) { return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z; }

    /// \brief Calculate the cross product between two vectors.
    ///
    /// \param vector1 = The first vector
    /// \param vector2 = The second vector
    /// \return The cross product
    static Vec3<Type> cross( const Vec3<Type> & vector1, const Vec3<Type> & vector2 );

    /// \brief Rotate a vector around an axis. Same as glRotate[f|d](angle, a);
    ///
    /// \param vector = Vector to use
    /// \param angle = Angle to rotate.
    /// \param axis = Rotation axis.
    /// \return The rotated vector
    static Vec3<Type> rotate( const Vec3<Type> & vector, const Angle & angle, const Vec3<Type> & axis );

    /// \brief Rounds all components on a vector
    ///
    /// Uses Asymmetric Arithmetic Rounding
    /// \param vector = Vector to use
    /// \return The rounded vector
    static Vec3<Type> round( const Vec3<Type> & vector );

    /// \brief Calculate the reflection direction for an incident vector
    ///
    /// Normal vector should be normalized
    static Vec3<Type> reflect( const Vec3<Type> & incident, const Vec3<Type> & normal );

    /// \brief Returns true if equal within the bounds of an epsilon
    ///
    /// \param first = Value A
    /// \param second = Value B
    /// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
    static bool is_equal( const Vec3<Type> & first, const Vec3<Type> & second, Type epsilon )
    {
        Type diff_x = second.x - first.x;
        Type diff_y = second.y - first.y;
        Type diff_z = second.z - first.z;
        return ( diff_x >= -epsilon && diff_x <= epsilon && diff_y >= -epsilon && diff_y <= epsilon && diff_z >= -epsilon && diff_z <= epsilon );
    }

    /// \name Attributes
    /// \{

public:
    /// \brief Returns the length (magnitude) of this vector.
    ///
    /// Operates in the native datatype
    /// \return the length of the vector
    Type length() const;

    /// \brief Normalizes this vector
    ///
    /// Operates in the native datatype
    /// \return reference to this object
    Vec3<Type> & normalize();

    /// \brief Dot products this vector with an other vector.
    ///
    /// Operates in the native datatype
    ///
    /// \param vector Second vector used for the dot product.
    /// \return The resulting dot product of the two vectors.
    Type dot( const Vec3<Type> & vector ) const { return x * vector.x + y * vector.y + z * vector.z; }

    /// \brief Calculate the angle between this vector and an other vector.
    ///
    /// \param vector Second vector used to calculate angle.
    ///
    /// \return The angle between the two vectors.
    Angle angle( const Vec3<Type> & vector ) const;

    /// \brief Calculate the angle between this vector and an other vector, where the vectors are unit vectors
    ///
    /// \param vector Second vector used to calculate angle.
    ///
    /// \return The angle between the two vectors.
    Angle angle_normed( const Vec3<Type> & vector ) const;

    /// \brief Calculate the distance between this vector and an other vector.
    ///
    /// \param vector = Second vector used to calculate distance.
    ///
    /// \return The distance between the two vectors.
    Type distance( const Vec3<Type> & vector ) const;

    /// \brief Calculate the cross product between this vector and an other vector.
    ///
    /// Operates in the native datatype
    /// \param vector = Second vector used to perform the calculation.
    /// \return reference to this object
    Vec3<Type> & cross( const Vec3<Type> & vector );

    /// \brief Rotate this vector around an axis. Same as glRotate[f|d](angle, a);
    ///
    /// \param angle Angle to rotate.
    /// \param axis Rotation axis.
    /// \return reference to this object
    Vec3<Type> & rotate( const Angle & angle, const Vec3<Type> & axis );

    /// \brief Rounds all components on this vector
    ///
    //// Uses Asymmetric Arithmetic Rounding
    /// \return reference to this object
    Vec3<Type> & round();

    /// \brief To Array
    ///
    ///
    void to_array( Type * array ) const
    {
        array[0] = x;
        array[1] = y;
        array[2] = z;
    }

    /// \brief Returns true if equal within the bounds of an epsilon
    ///
    /// \param other = Other value
    /// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
    bool is_equal( const Vec3<Type> & other, Type epsilon ) const { return Vec3<Type>::is_equal( *this, other, epsilon ); }

    /// 坐标是否在区间内
    bool is_in_region( const Vec3<Type> & leftbottom, const Vec3<Type> & righttop ) const
    {
        return leftbottom.x <= x && x <= righttop.x && leftbottom.y <= y && y <= righttop.y && leftbottom.z <= z && z <= righttop.z;
    }

    /// \}
    /// \name Operators
    /// \{

public:
    /// \brief += operator.
    void operator+=( const Vec3<Type> & vector )
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
    }

    /// \brief += operator.
    void operator+=( Type value )
    {
        x += value;
        y += value;
        z += value;
    }

    /// \brief -= operator.
    void operator-=( const Vec3<Type> & vector )
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
    }

    /// \brief -= operator.
    void operator-=( Type value )
    {
        x -= value;
        y -= value;
        z -= value;
    }

    /// \brief - operator.
    Vec3<Type> operator-() const { return Vec3<Type>( -x, -y, -z ); }

    /// \brief *= operator.
    void operator*=( const Vec3<Type> & vector )
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
    }

    /// \brief *= operator.
    void operator*=( Type value )
    {
        x *= value;
        y *= value;
        z *= value;
    }

    /// \brief /= operator.
    void operator/=( const Vec3<Type> & vector )
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
    }

    /// \brief /= operator.
    void operator/=( Type value )
    {
        x /= value;
        y /= value;
        z /= value;
    }

    /// \brief = operator.
    Vec3<Type> & operator=( const Vec3<Type> & vector )
    {
        x = vector.x;
        y = vector.y;
        z = vector.z;
        return *this;
    }

    /// \brief == operator.
    bool operator==( const Vec3<Type> & vector ) const { return ( ( x == vector.x ) && ( y == vector.y ) && ( z == vector.z ) ); }

    /// \brief != operator.
    bool operator!=( const Vec3<Type> & vector ) const { return ( ( x != vector.x ) || ( y != vector.y ) || ( z != vector.z ) ); }

    /// \brief < operator.
    bool operator<( const Vec3<Type> & vector ) const { return z < vector.z || ( z == vector.z && ( y < vector.y || ( y == vector.y && x < vector.x ) ) ); }
    /// \}
};

template<typename Type> const int Vec3<Type>::array_size = 3;

/// \brief + operator.
template<typename Type>
Vec3<Type> operator+( const Vec3<Type> & v1, const Vec3<Type> & v2 )
{
    return Vec3<Type>( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

/// \brief + operator.
template<typename Type>
Vec3<Type> operator+( Type s, const Vec3<Type> & v )
{
    return Vec3<Type>( s + v.x, s + v.y, s + v.z );
}

/// \brief + operator.
template<typename Type>
Vec3<Type> operator+( const Vec3<Type> & v, Type s )
{
    return Vec3<Type>( v.x + s, v.y + s, v.z + s );
}

/// \brief - operator.
template<typename Type>
Vec3<Type> operator-( const Vec3<Type> & v1, const Vec3<Type> & v2 )
{
    return Vec3<Type>( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

/// \brief - operator.
template<typename Type>
Vec3<Type> operator-( Type s, const Vec3<Type> & v )
{
    return Vec3<Type>( s - v.x, s - v.y, s - v.z );
}

/// \brief - operator.
template<typename Type>
Vec3<Type> operator-( const Vec3<Type> & v, Type s )
{
    return Vec3<Type>( v.x - s, v.y - s, v.z - s );
}

/// \brief * operator.
template<typename Type>
Vec3<Type> operator*( const Vec3<Type> & v1, const Vec3<Type> & v2 )
{
    return Vec3<Type>( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z );
}

/// \brief * operator.
template<typename Type>
Vec3<Type> operator*( Type s, const Vec3<Type> & v )
{
    return Vec3<Type>( s * v.x, s * v.y, s * v.z );
}

/// \brief * operator.
template<typename Type>
Vec3<Type> operator*( const Vec3<Type> & v, Type s )
{
    return Vec3<Type>( v.x * s, v.y * s, v.z * s );
}

/// \brief / operator.
template<typename Type>
Vec3<Type> operator/( const Vec3<Type> & v1, const Vec3<Type> & v2 )
{
    return Vec3<Type>( v1.x / v2.x, v1.y / v2.y, v1.z / v2.z );
}

/// \brief / operator.
template<typename Type>
Vec3<Type> operator/( Type s, const Vec3<Type> & v )
{
    return Vec3<Type>( s / v.x, s / v.y, s / v.z );
}

/// \brief / operator.
template<typename Type>
Vec3<Type> operator/( const Vec3<Type> & v, Type s )
{
    return Vec3<Type>( v.x / s, v.y / s, v.z / s );
}

template<>
inline Vec3<uint8_t>::Vec3( const Vec3<float> & copy )
{
    x = (uint8_t)floor( copy.x + 0.5f );
    y = (uint8_t)floor( copy.y + 0.5f );
    z = (uint8_t)floor( copy.z + 0.5f );
}

template<>
inline Vec3<uint8_t>::Vec3( const Vec3<double> & copy )
{
    x = (uint8_t)floor( copy.x + 0.5 );
    y = (uint8_t)floor( copy.y + 0.5 );
    z = (uint8_t)floor( copy.z + 0.5 );
}

template<>
inline Vec3<uint8_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (uint8_t)copy.x;
    y = (uint8_t)copy.y;
    z = (uint8_t)copy.z;
}

template<>
inline Vec3<int8_t>::Vec3( const Vec3<float> & copy )
{
    x = (int8_t)floor( copy.x + 0.5f );
    y = (int8_t)floor( copy.y + 0.5f );
    z = (int8_t)floor( copy.z + 0.5f );
}

template<>
inline Vec3<int8_t>::Vec3( const Vec3<double> & copy )
{
    x = (int8_t)floor( copy.x + 0.5 );
    y = (int8_t)floor( copy.y + 0.5 );
    z = (int8_t)floor( copy.z + 0.5 );
}

template<>
inline Vec3<int8_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (int8_t)copy.x;
    y = (int8_t)copy.y;
    z = (int8_t)copy.z;
}

template<>
inline Vec3<uint16_t>::Vec3( const Vec3<float> & copy )
{
    x = (uint16_t)floor( copy.x + 0.5f );
    y = (uint16_t)floor( copy.y + 0.5f );
    z = (uint16_t)floor( copy.z + 0.5f );
}

template<>
inline Vec3<uint16_t>::Vec3( const Vec3<double> & copy )
{
    x = (uint16_t)floor( copy.x + 0.5 );
    y = (uint16_t)floor( copy.y + 0.5 );
    z = (uint16_t)floor( copy.z + 0.5 );
}

template<>
inline Vec3<uint16_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (uint16_t)copy.x;
    y = (uint16_t)copy.y;
    z = (uint16_t)copy.z;
}

template<>
inline Vec3<int16_t>::Vec3( const Vec3<float> & copy )
{
    x = (int16_t)floor( copy.x + 0.5f );
    y = (int16_t)floor( copy.y + 0.5f );
    z = (int16_t)floor( copy.z + 0.5f );
}

template<>
inline Vec3<int16_t>::Vec3( const Vec3<double> & copy )
{
    x = (int16_t)floor( copy.x + 0.5 );
    y = (int16_t)floor( copy.y + 0.5 );
    z = (int16_t)floor( copy.z + 0.5 );
}

template<>
inline Vec3<int16_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (int16_t)copy.x;
    y = (int16_t)copy.y;
    z = (int16_t)copy.z;
}

template<>
inline Vec3<int32_t>::Vec3( const Vec3<float> & copy )
{
    x = (int)floor( copy.x + 0.5f );
    y = (int)floor( copy.y + 0.5f );
    z = (int)floor( copy.z + 0.5f );
}

template<>
inline Vec3<int32_t>::Vec3( const Vec3<double> & copy )
{
    x = (int)floor( copy.x + 0.5 );
    y = (int)floor( copy.y + 0.5 );
    z = (int)floor( copy.z + 0.5 );
}

template<>
inline Vec3<int32_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (int)copy.x;
    y = (int)copy.y;
    z = (int)copy.z;
}

template<>
inline Vec3<uint32_t>::Vec3( const Vec3<float> & copy )
{
    x = (uint32_t)floor( copy.x + 0.5f );
    y = (uint32_t)floor( copy.y + 0.5f );
    z = (uint32_t)floor( copy.z + 0.5f );
}

template<>
inline Vec3<uint32_t>::Vec3( const Vec3<double> & copy )
{
    x = (uint32_t)floor( copy.x + 0.5 );
    y = (uint32_t)floor( copy.y + 0.5 );
    z = (uint32_t)floor( copy.z + 0.5 );
}

template<>
inline Vec3<uint32_t>::Vec3( const Vec3<int32_t> & copy )
{
    x = (uint32_t)copy.x;
    y = (uint32_t)copy.y;
    z = (uint32_t)copy.z;
}

template<>
inline Vec3<float>::Vec3( const Vec3<float> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
    z = (float)copy.z;
}

template<>
inline Vec3<float>::Vec3( const Vec3<double> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
    z = (float)copy.z;
}

template<>
inline Vec3<float>::Vec3( const Vec3<int32_t> & copy )
{
    x = (float)copy.x;
    y = (float)copy.y;
    z = (float)copy.z;
}

template<>
inline Vec3<double>::Vec3( const Vec3<float> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
    z = (double)copy.z;
}

template<>
inline Vec3<double>::Vec3( const Vec3<double> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
    z = (double)copy.z;
}

template<>
inline Vec3<double>::Vec3( const Vec3<int32_t> & copy )
{
    x = (double)copy.x;
    y = (double)copy.y;
    z = (double)copy.z;
}

template<typename Type>
inline Type Vec3<Type>::length() const
{
    return (Type)floor( sqrt( float( x * x + y * y + z * z ) ) + 0.5f );
}

template<>
inline double Vec3<double>::length() const
{
    return sqrt( x * x + y * y + z * z );
}

template<>
inline float Vec3<float>::length() const
{
    return sqrt( x * x + y * y + z * z );
}

template<typename Type>
inline Vec3<Type> & Vec3<Type>::normalize()
{
    Type f = length();
    if ( f != 0 ) {
        x /= f;
        y /= f;
        z /= f;
    }
    return *this;
}

template<typename Type>
inline Vec3<Type> Vec3<Type>::normalize( const Vec3<Type> & vector )
{
    Vec3<Type> dest( vector );
    dest.normalize();
    return dest;
}

template<typename Type>
inline Vec3<Type> & Vec3<Type>::round()
{
    x = (Type)floorf( x + 0.5f );
    y = (Type)floorf( y + 0.5f );
    z = (Type)floorf( z + 0.5f );
    return *this;
}

template<typename Type>
inline Vec3<Type> Vec3<Type>::round( const Vec3<Type> & vector )
{
    Vec3<Type> dest( vector );
    dest.round();
    return dest;
}

template<typename Type>
inline Vec3<Type> Vec3<Type>::cross( const Vec3<Type> & v1, const Vec3<Type> & v2 )
{
    Vec3<Type> tmp = Vec3<Type>( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
    return tmp;
}

template<typename Type>
inline Vec3<Type> & Vec3<Type>::cross( const Vec3<Type> & v )
{
    *this = cross( *this, v );
    return *this;
}

template<typename Type>
inline Vec3<Type> Vec3<Type>::reflect( const Vec3<Type> & incident, const Vec3<Type> & normal )
{
    return incident - normal.dot( incident ) * normal * Type( 2 );
}

// For integers
template<typename Type>
inline Type Vec3<Type>::distance( const Vec3<Type> & vector ) const
{
    float value_x, value_y, value_z;
    value_x = x - vector.x;
    value_y = y - vector.y;
    value_z = z - vector.z;
    return (Type)floor( sqrt( value_x * value_x + value_y * value_y + value_z * value_z ) + 0.5f );
}

// For floats
template<>
inline float Vec3<float>::distance( const Vec3<float> & vector ) const
{
    float value_x, value_y, value_z;
    value_x = x - vector.x;
    value_y = y - vector.y;
    value_z = z - vector.z;
    return sqrt( value_x * value_x + value_y * value_y + value_z * value_z );
}

// For doubles
template<>
inline double Vec3<double>::distance( const Vec3<double> & vector ) const
{
    double value_x, value_y, value_z;
    value_x = x - vector.x;
    value_y = y - vector.y;
    value_z = z - vector.z;
    return sqrt( value_x * value_x + value_y * value_y + value_z * value_z );
}

typedef Vec3<uint8_t> Vec3ub;
typedef Vec3<int8_t> Vec3b;
typedef Vec3<uint16_t> Vec3us;
typedef Vec3<int16_t> Vec3s;
typedef Vec3<uint32_t> Vec3ui;
typedef Vec3<int32_t> Vec3i;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

#endif
