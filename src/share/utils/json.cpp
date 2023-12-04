
#include "json.h"

namespace utils {

template<typename V> void JsonWriter::put( V const & value ) {}

// 特化常用数值类型
template<> void JsonWriter::put( const int8_t & value ) { m_Writer.Int( value ); }
template<> void JsonWriter::put( const uint8_t & value ) { m_Writer.Uint( value ); }
template<> void JsonWriter::put( const int16_t & value ) { m_Writer.Int( value ); }
template<> void JsonWriter::put( const uint16_t & value ) { m_Writer.Uint( value ); }
template<> void JsonWriter::put( const int32_t & value ) { m_Writer.Int( value ); }
template<> void JsonWriter::put( const uint32_t & value ) { m_Writer.Uint( value ); }
template<> void JsonWriter::put( const int64_t & value ) { m_Writer.Int64( value ); }
template<> void JsonWriter::put( const uint64_t & value ) { m_Writer.Uint64( value ); }
template<> void JsonWriter::put( const double & value ) { m_Writer.Double( value ); }
#if defined( __APPLE__ )
template<> void JsonWriter::put( const size_t & value ) { m_Writer.Uint64( value ); }
template<> void JsonWriter::put( const time_t & value ) { m_Writer.Int64( value ); }
#endif

// 特化STL类型
template<> void JsonWriter::put( const bool & value ) { m_Writer.Bool( value ); }
template<> void JsonWriter::put( const std::string & value ) { m_Writer.String( value.c_str() ); }

template<typename V> void JsonReader::get( V & value ) {}

// 特化常用数值类型
template<> void JsonReader::get( int8_t & value ) { if ( m_CurrValue->IsInt() ) value = m_CurrValue->GetInt(); }
template<> void JsonReader::get( uint8_t & value ) { if ( m_CurrValue->IsUint() ) value = m_CurrValue->GetUint(); }
template<> void JsonReader::get( int16_t & value ) { if ( m_CurrValue->IsInt() ) value = m_CurrValue->GetInt(); }
template<> void JsonReader::get( uint16_t & value ) { if ( m_CurrValue->IsUint() ) value = m_CurrValue->GetUint(); }
template<> void JsonReader::get( int32_t & value ) { if ( m_CurrValue->IsInt() ) value = m_CurrValue->GetInt(); }
template<> void JsonReader::get( uint32_t & value ) { if ( m_CurrValue->IsUint() ) value = m_CurrValue->GetUint(); }
template<> void JsonReader::get( int64_t & value ) { if ( m_CurrValue->IsInt64() ) value = m_CurrValue->GetInt64(); }
template<> void JsonReader::get( uint64_t & value ) { if ( m_CurrValue->IsUint64() ) value = m_CurrValue->GetUint64(); }
template<> void JsonReader::get( bool & value ) { if ( m_CurrValue->IsBool() ) value = m_CurrValue->GetBool(); }
template<> void JsonReader::get( double & value ) { if ( m_CurrValue->IsDouble() ) value = m_CurrValue->GetDouble(); }
template<> void JsonReader::get( std::string & value ) { if ( m_CurrValue->IsString() ) value = m_CurrValue->GetString(); }
} // namespace utils

#if 0

namespace utils
{

struct Res
{
    uint32_t id;
    uint32_t type;
    uint32_t count;
};
typedef std::vector<Res> ResV;

struct Res1
{
    uint32_t id;
    uint32_t type;
    Res res;
    ResV resv;
};

// 测试
#define JSON_RES1_OPE( M, C ) \
    M( "id", C.id );          \
    M( "type", C.type );      \
    M( "res", C.res );        \
    M( "resv", C.resv ); \

template<> void JsonReader::get( Res & res )
{
    get( "id", res.id );
    get( "type", res.type );
    get( "count", res.count );
}

template<> void JsonReader::get( Res1 & res1 )
{
    JSON_RES1_OPE( get, res1 );
}

}

// 测试
int main()
{
    //std::string json = "{\"json\":{\"id\":123456,\"type\":12,\"res\":{\"id\":8,\"type\":6,\"count\":7},\"resv\":[{\"id\":10,\"type\":11,\"count\":12},{\"id\":20,\"type\":21,\"count\":22}]}}";
    //std::string json = "{\"id\":123456,\"type\":12,\"res\":{\"id\":8,\"type\":6,\"count\":7},\"resv\":[{\"id\":10,\"type\":11,\"count\":12},{\"id\":20,\"type\":21,\"count\":22}]}";
    std::string json = "[{\"k\":1,\"v\":2},{\"k\":2,\"v\":3},{\"k\":3,\"v\":4}]";
    utils::JsonReader reader( json.c_str() );
    std::map<uint32_t, uint32_t> res1;
    //utils::Res1 res1;
    reader.get( res1 );
    //printf("%d\n %d\n %d\n ressize(%u)\n", res1.res.id, res1.res.type, res1.res.count, res1.resv.size());
    printf("size : %u key : %d value : %d \n", (uint32_t)res1.size(), 1, res1[1]);

    std::map<uint32_t, uint32_t> res1;
    res1[1] = 1;
    res1[2] = 2;
    res1[3] = 3;
    utils::JsonWriter writer;
    //writer.start();
    writer.put(res1);
    //writer.end();
    printf( "a : %s\n", writer.content() );

}
#endif
