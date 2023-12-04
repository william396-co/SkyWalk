#pragma once
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

namespace utils {

//
// 根据RapidJson封装的JsonWriter
// 方便使用者调用
//

class JsonWriter
{
public:
    JsonWriter() : m_Writer( m_Buffer ) {}
    virtual ~JsonWriter() {}

public:
    //
    template<typename V> void put( V const & value );

    //
    template<typename V>
        void put( const std::vector<V> & values )
        {
            m_Writer.StartArray();

            for ( size_t i = 0; i < values.size(); ++i ) {
                put( values[i] );
            }

            m_Writer.EndArray();
        }

    template<typename V>
    void put(const std::set<V>& values){
        m_Writer.StartArray();
        for(const auto& v: values){
            put(v);
        }
        m_Writer.EndArray();
    }

    template<typename V>
        void put(const std::unordered_set<V>& values){
            m_Writer.StartArray();
            for(const auto& v: values){
                put(v);
            }
            m_Writer.EndArray();
        }

    template<typename V>
        void put( const std::map<std::string, V> & maps )
        {
            m_Writer.StartObject();

            for ( const auto & p : maps ) {
                put( p.first, p.second );
            }

            m_Writer.EndObject();
        }

    template<typename K, typename V>
        void put( const std::map<K, V> & maps )
        {
            m_Writer.StartArray();
            for ( const auto & p : maps ) {
                start();
                put( "k", p.first );
                put( "v", p.second );
                end();
            }
            m_Writer.EndArray();
        }

    template<typename K, typename V>
        void put( const std::unordered_map<K, V> & maps )
        {
            m_Writer.StartArray();
            for ( const auto & p : maps ) {
                start();
                put( "k", p.first );
                put( "v", p.second );
                end();
            }
            m_Writer.EndArray();
        }
    //
    template<typename V>
        void put( const std::string & key, const V & value )
        {
            put( key );
            put( value );
        }

    // 重载char[]
    void put( const char * value ) { m_Writer.String( value ); }

public:
    void start( uint8_t type = 1 )
    {
        if ( type == 1 )
            m_Writer.StartObject();
        else if ( type == 2 )
            m_Writer.StartArray();
    }
    void end( uint8_t type = 1 )
    {
        if ( type == 1 )
            m_Writer.EndObject();
        else if ( type == 2 )
            m_Writer.EndArray();
    }

    size_t length() const { return m_Buffer.GetSize(); }
    const char * content() const { return m_Buffer.GetString(); }
    std::string tostring() const { return std::string( m_Buffer.GetString(), m_Buffer.GetSize() ); }

    void clear() {
        m_Buffer.Clear();
        m_Writer.Reset( m_Buffer );
    }

private:
    rapidjson::StringBuffer m_Buffer;
    rapidjson::Writer<rapidjson::StringBuffer> m_Writer;
};

class JsonReader
{
public:
    JsonReader( const std::string & content )
        : m_CurrValue( nullptr )
    {
        m_Document.Parse( content.c_str() );
        m_CurrValue = &m_Document;
    }

    virtual ~JsonReader()
    {}

public:
    template<typename V> void get( V & value );

    template<typename V>
    void get( std::vector<V> & values )
    {
        if ( !m_CurrValue->IsArray() ) {
            return;
        }

        rapidjson::Value * oldvalue = m_CurrValue;

        for ( size_t i = 0; i < m_CurrValue->Size(); ++i ) {
            m_CurrValue = &( *m_CurrValue )[i];

            V value;
            get( value );
            values.push_back( value );
            m_CurrValue = oldvalue;
        }
    }

    template<typename V>
    void get(std::set<V>& values){

        if(!m_CurrValue->IsArray()){
            return ;
        }

        rapidjson::Value* oldvalue = m_CurrValue;
        for(size_t i=0;i < m_CurrValue->Size();++i){
            m_CurrValue = &(*m_CurrValue)[i];

            V value;
            get(value);
            values.emplace(value);
            m_CurrValue = oldvalue;
        }
    }

    template<typename V>
    void get(std::unordered_set<V>& values){

        if(!m_CurrValue->IsArray())
            return;

        rapidjson::Value* oldvalue = m_CurrValue;
        for(size_t i=0;i < m_CurrValue->Size();++i){
            m_CurrValue = &(*m_CurrValue)[i];

            V value;
            get(value);
            values.emplace(value);
            m_CurrValue = oldvalue;
        }
    }

    template<typename V>
    void get( const std::string & key, V & value )
    {
        if ( m_Document.HasParseError() ) {
            return;
        }

        if ( m_CurrValue == nullptr
            || !m_CurrValue->IsObject() ) {
            return;
        }

        rapidjson::Value * oldvalue = m_CurrValue;

        auto result = m_CurrValue->FindMember( key.c_str() );
        if ( result != m_CurrValue->MemberEnd() ) {
            m_CurrValue = &( *m_CurrValue )[key.c_str()];
            get( value );
        }

        m_CurrValue = oldvalue;
    }

    template<typename K, typename V>
    void get( std::map<K, V> & maps )
    {
        if ( !m_CurrValue->IsArray() ) {
            return;
        }

        rapidjson::Value * oldvalue = m_CurrValue;

        for ( size_t i = 0; i < m_CurrValue->Size(); ++i ) {
            m_CurrValue = &( *m_CurrValue )[i];

            K key;
            V value;

            get( "k", key );
            get( "v", value );
            maps.emplace( key, value );

            m_CurrValue = oldvalue;
        }
    }

private:
    rapidjson::Document m_Document;
    rapidjson::Value * m_CurrValue;
};

// json解析器
class IReaderHandler
{
public:
    IReaderHandler() {}
    virtual ~IReaderHandler() {}

public:
    virtual bool Null() { return true; }
    virtual bool Bool( bool value ) { return true; }
    virtual bool Int( int value ) { return true; }
    virtual bool Uint( unsigned value ) { return true; }
    virtual bool Int64( int64_t value ) { return true; }
    virtual bool Uint64( uint64_t value ) { return true; }
    virtual bool Double( double value ) { return true; }
    virtual bool String( const char * str, size_t length, bool copy ) { return true; }
    virtual bool StartObject() { return true; }
    virtual bool Key( const char * str, size_t length, bool copy ) { return true; }
    virtual bool EndObject( size_t memberCount ) { return true; }
    virtual bool StartArray() { return true; }
    virtual bool EndArray( size_t elementCount ) { return true; }
};

} // namespace utils

