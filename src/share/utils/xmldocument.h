
#ifndef __SRC_UTILS_XMLDOCUMENT_H__
#define __SRC_UTILS_XMLDOCUMENT_H__

#include <map>
#include <string>
#include <stdint.h>

namespace rapidxml {
template<class Ch> class file;
template<class Ch> class xml_node;
template<class Ch> class xml_document;
template<class Ch> class xml_attribute;
} // namespace rapidxml

namespace utils {

typedef rapidxml::file<char> XmlFile;
typedef rapidxml::xml_node<char> XmlNode;
typedef rapidxml::xml_document<char> XmlRawDoc;
typedef rapidxml::xml_attribute<char> XmlAttribute;

//
// Xml记录集
//

class IXmlRecords
{
public:
    IXmlRecords() {}
    virtual ~IXmlRecords() {}
    virtual bool append() = 0;
    virtual void cleanup() = 0;

protected:
    template<typename V> bool get( const char * key, V & value );

    bool get( const char * key, bool & value );
    bool get( const char * key, int8_t & value );
    bool get( const char * key, uint8_t & value );
    bool get( const char * key, int16_t & value );
    bool get( const char * key, uint16_t & value );
    bool get( const char * key, int32_t & value );
    bool get( const char * key, uint32_t & value );
    bool get( const char * key, int64_t & value );
    bool get( const char * key, uint64_t & value );
    bool get( const char * key, float & value );
    bool get( const char * key, std::string & value );
    bool get( const char * key, char * value, size_t length );

private:
    friend class XmlParser;
    void setNode( XmlNode * node );

private:
    XmlNode * m_Node;
};

//
// 解析Xml文件
//
class XmlParser
{
public:
    XmlParser();
    ~XmlParser();
    // 解析某个标签
    bool open( const char * path );
    bool parse( const char * label, IXmlRecords * set );
    void close();

private:
    XmlFile * m_RawFile;
    XmlRawDoc * m_Document;
};

template<class D=XmlNode *> D xmlget( XmlNode * node, const std::string & tag );

//
// 生成Xml文件
//

class XmlWriter
{
public:
    XmlWriter();
    ~XmlWriter();

public:
    // 复制内容
    void content( std::string & text );
    // 添加节点
    void append( XmlNode * parent, XmlNode * node );
    void append( XmlNode * node, XmlAttribute * attrib );
    // 创建节点
    XmlNode * node( const std::string & field );
    XmlNode * node(
        const std::string & key, const std::string & value );
    // 创建属性
    XmlAttribute * attribute(
        const std::string & key, const std::string & value );

private:
    XmlRawDoc * m_Document;
};

} // namespace utils

#endif
