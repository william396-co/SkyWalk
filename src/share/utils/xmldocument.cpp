
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <string>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "xmldocument.h"

namespace utils {

template<> XmlNode * xmlget( XmlNode * node, const std::string & tag ) {
    return node->first_node( tag.c_str() );
}

template<> std::string xmlget( XmlNode * node, const std::string & tag ) {
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return (std::string)n->value();
    }
    return "";
}

template <> bool xmlget( XmlNode * node, const std::string & tag )
{
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return std::atoi( n->value() ) == 1;
    }
    return false;
}
template <> int32_t xmlget( XmlNode * node, const std::string & tag )
{
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return std::atoi( n->value() );
    }
    return 0;
}
template <> uint32_t xmlget( XmlNode * node, const std::string & tag )
{
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return std::atoi( n->value() );
    }
    return 0;
}

template <> int64_t xmlget( XmlNode * node, const std::string & tag )
{
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return std::atoll( n->value() );
    }
    return 0;
}
template <> uint64_t xmlget( XmlNode * node, const std::string & tag )
{
    auto n = node->first_node( tag.c_str() );
    if ( n != nullptr ) {
        return std::atoll( n->value() );
    }
    return 0;
}

bool IXmlRecords::get( const char * key, bool & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (bool)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, int8_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (int8_t)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, uint8_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (uint8_t)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, int16_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (int16_t)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, uint16_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (uint16_t)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, int32_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, uint32_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (uint32_t)std::atoi( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, int64_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = std::atoll( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, uint64_t & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = (uint64_t)std::atoll( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, float & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = std::atof( _value->value() );
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, std::string & value )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            value = _value->value();
            return true;
        }
    }

    return false;
}

bool IXmlRecords::get( const char * key, char * value, size_t length )
{
    rapidxml::xml_attribute<> * _value = nullptr;

    if ( m_Node ) {
        _value = m_Node->first_attribute( key );
        assert( _value != nullptr && "key invalid" );

        if ( _value != nullptr ) {
            std::strncpy( value, _value->value(), length );
            return true;
        }
    }

    return false;
}

void IXmlRecords::setNode( XmlNode * node )
{
    m_Node = node;
}

////////////////////////////////////////////////////////////////////////////////

XmlParser::XmlParser()
    : m_RawFile( nullptr ),
      m_Document( nullptr )
{}

XmlParser::~XmlParser()
{
    close();
}

bool XmlParser::open( const char * path )
{
    // 打开原始文件
    m_RawFile = new XmlFile( path );
    if ( m_RawFile == nullptr || m_RawFile->data() == nullptr ) {
        return false;
    }
    // 打开xml解释器
    m_Document = new XmlRawDoc;
    if ( m_Document == nullptr ) {
        return false;
    }
    // 解析
    m_Document->parse<0>( m_RawFile->data() );
    return true;
}

bool XmlParser::parse( const char * label, IXmlRecords * set )
{
    XmlNode * node = nullptr;

    if ( label == nullptr ) {
        node = m_Document->first_node();
    } else {
        node = m_Document->first_node()->first_node( label );
    }

    if ( node == nullptr ) {
        return false;
    }

    for ( XmlNode * child = node->first_node(); child != nullptr; child = child->next_sibling() ) {
        set->setNode( child );

        if ( !set->append() ) {
            return false;
        }

        set->setNode( nullptr );
    }

    return true;
}

void XmlParser::close()
{
    if ( m_RawFile ) {
        delete m_RawFile;
        m_RawFile = nullptr;
    }

    if ( m_Document ) {
        m_Document->clear();
        delete m_Document;
        m_Document = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

XmlWriter::XmlWriter()
{
    m_Document = new XmlRawDoc;
    assert( m_Document != nullptr );

    m_Document->append_node(
        m_Document->allocate_node(
            rapidxml::node_pi,
            m_Document->allocate_string( "xml version='1.0' encoding='utf-8'" ) ) );
}

XmlWriter::~XmlWriter()
{
    if ( m_Document != nullptr ) {
        delete m_Document;
        m_Document = nullptr;
    }
}

XmlNode * XmlWriter::node( const std::string & field )
{
    return m_Document->allocate_node(
        rapidxml::node_element,
        m_Document->allocate_string( field.c_str() ),
        nullptr );
}

XmlNode * XmlWriter::node( const std::string & key, const std::string & value )
{
    return m_Document->allocate_node(
        rapidxml::node_element,
        m_Document->allocate_string( key.c_str() ),
        m_Document->allocate_string( value.c_str() ) );
}

XmlAttribute * XmlWriter::attribute( const std::string & key, const std::string & value )
{
    return m_Document->allocate_attribute(
        m_Document->allocate_string( key.c_str() ),
        m_Document->allocate_string( value.c_str() ) );
}

void XmlWriter::append( XmlNode * parent, XmlNode * node )
{
    if ( parent != nullptr ) {
        parent->append_node( node );
    } else {
        m_Document->append_node( node );
    }
}

void XmlWriter::append( XmlNode * node, XmlAttribute * attrib )
{
    if ( node != nullptr ) {
        node->append_attribute( attrib );
    } else {
        m_Document->append_attribute( attrib );
    }
}

void XmlWriter::content( std::string & text )
{
    rapidxml::print( std::back_inserter( text ), *m_Document, 0 );
}

} // namespace utils
