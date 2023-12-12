
#ifndef __SRC_UTILS_JSARRAY_H__
#define __SRC_UTILS_JSARRAY_H__

#include <vector>
#include <string>

#include <stdint.h>

namespace utils {

class JsArray;

class JsNode
{
public:
    JsNode();
    ~JsNode();

public:
    // 获取元素
    // type     - 1: 叶子; 2: 子节点
    size_t getElemCount() const { return m_Elem.size(); }
    const void * getElem( size_t index, int32_t & type ) const;

    // 获取叶子
    const char * getLeaf( size_t index ) const;
    size_t getLeafCount() const { return m_Leaf.size(); }

    // 获取子节点
    JsNode * getChild( size_t index ) const;
    size_t getChildCount() const { return m_Child.size(); }

private:
    // 添加子节点
    JsNode * addChild();

    // 添加叶子节点
    void addLeaf( const char * leaf, int32_t len );

    // 设置parent
    void setParent( JsNode * n ) { m_Parent = n; }
    JsNode * getParent() const { return m_Parent; }

private:
    friend class JsArray;

    struct JsonElem
    {
        void * elem;
        int32_t type;
    };

    JsNode * m_Parent;
    std::vector<JsonElem> m_Elem;
    std::vector<std::string> m_Leaf;
    std::vector<JsNode *> m_Child;
};

class JsArray
{
public:
    JsArray( const std::string & data );
    JsArray( const char * data, uint32_t len );
    ~JsArray();

public:
    // 解释
    // 返回根节点
    JsNode * parse();

    // 获取根节点
    JsNode * getRoot() const { return m_RootNode; }

    // 打印
    static void print( JsNode * node );

private:
    std::string m_Data;
    JsNode * m_RootNode;
};

} // namespace utils

#endif
