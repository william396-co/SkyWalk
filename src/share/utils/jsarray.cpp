
#include <stack>
#include <cassert>
#include <cstdio>

#include "jsarray.h"

namespace utils {

JsNode::JsNode()
    : m_Parent( nullptr )
{}

JsNode::~JsNode()
{
    m_Elem.clear();
    m_Leaf.clear();

    for ( auto node : m_Child ) {
        if ( node != nullptr ) {
            delete node;
        }
    }
    m_Child.clear();
}

const void * JsNode::getElem( size_t index, int32_t & type ) const
{
    if ( index < m_Elem.size() ) {
        type = m_Elem.at( index ).type;
        return m_Elem.at( index ).elem;
    }
    return nullptr;
}

const char * JsNode::getLeaf( size_t index ) const
{
    if ( index < m_Leaf.size() ) {
        return (char *)m_Leaf.at( index ).c_str();
    }

    return nullptr;
}

JsNode * JsNode::getChild( size_t index ) const
{
    if ( index < m_Child.size() ) {
        return m_Child.at( index );
    }
    return nullptr;
}

JsNode * JsNode::addChild()
{
    JsNode * node = new JsNode;
    if ( node ) {
        node->setParent( this );
        m_Child.push_back( node );

        JsonElem elem;
        elem.type = 2;
        elem.elem = static_cast<void *>( node );
        m_Elem.push_back( elem );
    }
    return node;
}

void JsNode::addLeaf( const char * leaf, int32_t len )
{
    char * pos = (char *)leaf;

    if ( len > 0 ) {
        char * end = pos + len - 1;

        while ( len > 0
            && ( *pos == ' ' || *pos == '\t' || *pos == '"' ) ) {
            ++pos;
            --len;
        }
        while ( len > 0
            && ( *end == ' ' || *end == '\t' || *end == '"' ) ) {
            --end;
            --len;
        }
    }

    if ( len >= 0 ) {
        m_Leaf.push_back( std::string( pos, len ) );

        JsonElem elem;
        elem.type = 1;
        elem.elem = (void *)getLeaf( getLeafCount() - 1 );
        m_Elem.push_back( elem );
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

JsArray::JsArray( const std::string & data )
    : m_Data( data ),
      m_RootNode( nullptr )
{
    m_RootNode = new JsNode;
    assert( m_RootNode != nullptr && "create JsNode failed" );
}

JsArray::JsArray( const char * data, uint32_t len )
    : m_Data( data, len ),
      m_RootNode( nullptr )
{
    m_RootNode = new JsNode;
    assert( m_RootNode != nullptr && "create JsNode failed" );
}

JsArray::~JsArray()
{
    if ( m_RootNode ) {
        delete m_RootNode;
        m_RootNode = nullptr;
    }
}

JsNode * JsArray::parse()
{
    // pos记录关键节点的位置, 包括","
    int32_t pos = 0;
    JsNode * node = nullptr;
    std::stack<char> checkstack;

    for ( size_t i = 0; i < m_Data.size(); ++i ) {
        switch ( m_Data[i] ) {
            case '"': {
                if ( !checkstack.empty()
                    && checkstack.top() == '"' ) {
                    checkstack.pop();
                } else {
                    checkstack.push( m_Data[i] );
                }
            } break;

            case ',': {
                // "" 中间的内容忽略
                if ( !checkstack.empty()
                    && checkstack.top() == '"' ) {
                    continue;
                }

                // 添加叶子节点
                if ( node != nullptr
                    && m_Data[pos] != ']' ) {
                    node->addLeaf( m_Data.c_str() + pos + 1, i - pos - 1 );
                }

                pos = i;
            } break;

            case ']': {
                // "" 中间的内容忽略
                if ( !checkstack.empty()
                    && checkstack.top() == '"' ) {
                    continue;
                }

                // 忽略[]空叶子节点
                // 忽略]]这种情况
                if ( node != nullptr
                    && m_Data[pos] != ']'
                    && m_Data[i - 1] != '[' ) {
                    node->addLeaf( m_Data.c_str() + pos + 1, i - pos - 1 );
                }

                pos = i;
                checkstack.pop();
                node = node->getParent();
            } break;

            case '[': {
                // "" 中间的内容忽略
                if ( !checkstack.empty()
                    && checkstack.top() == '"' ) {
                    continue;
                }

                if ( node == nullptr ) {
                    node = m_RootNode;
                } else {
                    node = node->addChild();
                    assert( node != nullptr && "add child node failed" );
                }

                pos = i;
                checkstack.push( m_Data[i] );
            } break;
        }
    }

    return checkstack.empty() ? m_RootNode : nullptr;
}

void JsArray::print( JsNode * node )
{
    size_t i = 0;

    std::printf( "Node: %p, LeafCount:%ld, ChildCount:%ld\n",
        node,
        node->getLeafCount(),
        node->getChildCount() );

    for ( i = 0; i < node->getLeafCount(); ++i ) {
        std::printf( "\tleaf: %s\n", node->getLeaf( i ) );
    }

    if ( node->getChildCount() > 0 ) {
        std::printf( "\tchild: " );
        for ( i = 0; i < node->getChildCount(); ++i ) {
            printf( "%p ", node->getChild( i ) );
        }
        std::printf( "\n" );
    }

    for ( i = 0; i < node->getChildCount(); ++i ) {
        JsArray::print( (JsNode *)node->getChild( i ) );
    }
}

} // namespace utils

#if 0
#include <string.h>
int main()
{
    const char * data = "[[],[,,,],[22222,4444,222],\"2,2\",\"xx\",,[8888,99999]]";
    utils::JsArray parser( data, strlen(data) );

    printf( "%s\n", data );
    parser.parse();
    parser.print( parser.getRoot() );

    return 0;
}
#endif
