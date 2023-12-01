#pragma once

#include <vector>

namespace utils {

template<typename Key, typename Score>
class SkipList
{
    using value_type = std::pair<Key, Score>;
    using key_list = std::vector<Key>;
    using key_value_list = std::vector<value_type>;

public:
    // insert element
    void insert( value_type const & value );
    // erase element
    bool erase( value_type const & value );

    // access the last element
    void back( Key & key ) const;
    // access the first element
    bool front( Key & key ) const;
    // get rank by score and key
    uint32_t rank( value_type const & value ) const;

    // get key by rank
    bool element( uint32_t rank, Key & key ) const;
    bool element( uint32_t rank, value_type & value ) const;

    /* get ranklist
     * index - from rank, >=1
     * count - get ranklist size
     */
    int32_t ranklist( uint32_t index, uint32_t count, key_list & list ) const;
    int32_t ranklist( uint32_t index, uint32_t count, key_value_list & list ) const;

    // get score range[min,max]
    int32_t range( Score const & min, Score const & max, uint32_t count, key_list & list, key_list const & excepts = {} ) const;
    int32_t range( Score const & nin, Score const & max, uint32_t count, key_value_list & list, key_list const & excepts = {} ) const;

    // clear
    void clear();

    // return the number of elements
    uint32_t size() const { return m_Len; }

    // print for debug
    void print( int32_t width = 4 );

private:
    struct Node;
    enum
    {
        eMaxLevel = 32
    };

private:
    // random level
    int32_t randomLevel();
    // erase node
    void eraseNode( Node * node, Node ** update );

private:
    struct Node * m_Head;
    struct Node * m_Tail;
    uint32_t m_Len;
    int32_t m_Level;
};

template<typename Key,typename Score>
struct SkipList<Key,Score>::Node{

    struct Level
    {
        Node* forward;
        uint32_t span;
    };
};

} // namespace utils
