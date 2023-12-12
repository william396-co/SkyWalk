#pragma once

#include <list>
#include <cassert>
#include <unordered_map>

#include "types.h"

namespace utils {

template<typename Key, typename Value>
struct DefaultEvictor
{
    void operator()( Key key, Value value ) const
    {}
};

// Key      - 键
// Value    - 值
// Evictor  - 淘汰函数
template<typename Key, typename Value, class Evictor = DefaultEvictor<Key, Value>>
class LRUCache
{
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef size_t size_type;

public:
    explicit LRUCache( size_type capacity )
        : m_Size( 0 ),
          m_Capacity( capacity )
    {
        assert( capacity != 0 && "Capacity Must be > 0" );
    }

    ~LRUCache()
    {
        this->clear();
    }

public:
    // 获取Key对应的Value, 并更新
    bool get( const key_type & key, value_type & value )
    {
        auto result = m_Table.find( key );
        if ( result == m_Table.end() ) {
            return false;
        }

        Entry * entry = result->second;
        assert( entry != nullptr && "HashTable Entry is Invalid" );

        // 返回值
        value = entry->value;

        // touch linker
        m_List.erase( entry->link );
        m_List.push_back( entry );
        entry->link = --m_List.end();

        return true;
    }

    // 向Cache中添加元素
    bool put( const key_type & key, const value_type & value )
    {
        Entry * entry = nullptr;

        auto rc = m_Table.find( key );
        if ( rc != m_Table.end() ) {
            // 更新Value
            entry = rc->second;
            assert( entry != nullptr && "HashTable Entry is Invalid" );

            // 淘汰函数
            m_Evictor( entry->key, entry->value );

            // 更新
            entry->value = value;
            m_List.erase( entry->link );
        } else {
            entry = new Entry( key, value );
            if ( entry == nullptr ) {
                return false;
            }

            ++m_Size;
            m_Table.emplace( key, entry );
        }

        // 更新link
        m_List.push_back( entry );
        entry->link = --m_List.end();

        // 淘汰最早的元素
        while ( m_Size > m_Capacity ) {
            entry = m_List.front();
            assert( entry != nullptr && "List::front() Error" );

            remove( entry->key );
        }

        return true;
    }

    // 从Cache中删除指定Key的元素
    void remove( const key_type & key )
    {
        Entry * entry = nullptr;

        auto rc = m_Table.find( key );
        if ( rc != m_Table.end() ) {
            // 个数变化
            --m_Size;

            entry = rc->second;
            m_Table.erase( rc );
        }

        if ( entry != nullptr ) {
            m_List.erase( entry->link );
            m_Evictor( entry->key, entry->value );
            delete entry;
        }
    }

    // 取出Key对应的Value
    bool take( const key_type & key, value_type & value )
    {
        Entry * entry = nullptr;

        auto rc = m_Table.find( key );
        if ( rc != m_Table.end() ) {
            entry = rc->second;
            m_Table.erase( rc );
        }

        if ( entry == nullptr ) {
            return false;
        }

        value = entry->value;
        m_List.erase( entry->link );
        delete entry;
        --m_Size;

        return true;
    }

    // 第一个元素
    bool front( key_type & key, value_type & value )
    {
        if ( m_List.empty() ) {
            return false;
        }

        Entry * entry = m_List.front();
        assert( entry != nullptr && "List::front() Error" );

        key = entry->key;
        value = entry->value;
        return true;
    }

    // 最后一个元素
    bool back( key_type & key, value_type & value )
    {
        if ( m_List.empty() ) {
            return false;
        }

        Entry * entry = m_List.back();
        if ( entry == nullptr ) {
            return false;
        }

        key = entry->key;
        value = entry->value;
        return true;
    }

    void clear()
    {
        for ( const auto & p : m_Table ) {
            m_Evictor( p.first, p.second->value );
            delete p.second;
        }

        m_Size = 0;
        m_List.clear();
        m_Table.clear();
    }

    // 遍历
    template<class Fn>
    Fn foreach ( Fn f )
    {
        for ( auto & value : m_List ) {
            f( value->key, value->value );
        }

        return f;
    }

    void reserve( size_t capacity )
    {
        assert( capacity != 0 && "Capacity Must be > 0" );
        if ( capacity == 0 ) {
            return;
        }

        m_Capacity = capacity;

        while ( m_Size > m_Capacity ) {
            Entry * entry = m_List.front();
            assert( entry != nullptr && "List::front() Error" );

            remove( entry->key );
        }
    }

    inline size_type size() const { return m_Size; }
    inline size_type capacity() const { return m_Capacity; }

private:
    struct Entry;
    using LRUList = std::list<Entry *>;
    using LRUListLink = typename LRUList::iterator;
    using HashTable = std::unordered_map<key_type, Entry *>;

    struct Entry
    {
        Entry( key_type k, value_type v )
            : key( k ),
              value( v )
        {}

        key_type key;
        value_type value;
        LRUListLink link;
    };

private:
    size_type m_Size;
    size_type m_Capacity;
    LRUList m_List;
    HashTable m_Table;
    Evictor m_Evictor;
};

} // namespace utils

