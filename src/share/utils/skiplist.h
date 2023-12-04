#pragma once

#include <vector>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace utils {

template<typename Key, typename Score>
class SkipList
{
public:
    using value_type = std::pair<Key, Score>;
    using key_list = std::vector<Key>;
    using key_value_list = std::vector<value_type>;

public:
    SkipList();
    ~SkipList();

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
    int32_t range( Score const & min, Score const & max, uint32_t count, key_value_list & list, key_list const & excepts = {} ) const;

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
    Node() = default;

    Node( value_type const & value )
        : key( value.first ), score { value.second }, backward { nullptr }
    {
    }

    uint32_t span(int32_t lv)const {
        return levels[lv].span;
    }
    Node* forward(int32_t lv)const {
        return levels[lv].forward;
    }

    Key key;
    Score score;
    struct Node* backward{ nullptr };
    struct Level levels[];
};

template<typename Key,typename Score>
SkipList<Key, Score>::SkipList()
    :m_Tail{ nullptr }, m_Len{ 0 }, m_Level{ 1 }
{
	m_Head = new(malloc(sizeof(Node) + eMaxLevel * sizeof(typename Node::Level)))Node{};
    for (auto i = 0; i < eMaxLevel; ++i) {
        m_Head->levels[i].span = 0;
        m_Head->levels[i].forward = nullptr;
    }
}

template<typename Key, typename Score>
inline SkipList<Key, Score>::~SkipList()
{
    Node* next = nullptr;
    Node* node = m_Head->levels[0].forward;
    free(m_Head);

    while (node) {
        next = node->levels[0].forward;
        free(node);
        node = next;
    }
}

template<typename Key, typename Score>
inline void SkipList<Key, Score>::insert(value_type const& value)
{
    int32_t level;
    int32_t rank[eMaxLevel];

    Node* cur = m_Head;
    Node* update[eMaxLevel];

    // span 某一层两个相连节点间间隔距离
    // rank记录的是经过路径(每一层)的最后一个节点是第几个
    // update记录的是 经过路径(每一层)的最后一个节点的前驱点
    for (auto i = m_Level - 1; i >= 0; --i) {
        rank[i] = i == (m_Level - 1) ? 0 : rank[i + 1];

        while (cur->forward(i) &&
            (cur->forward(i)->score > value.second ||
                (cur->forward(i)->key < value.first && cur->forward(i)->score == value.second))) {
            rank[i] += cur->span(i);
            cur = cur->forward(i);
        }
        update[i] = cur;
    }
    level = randomLevel();

    if (level > m_Level) {
        for (auto i = m_Level ; i <level; ++i) {
            rank[i] = 0;
            update[i] = m_Head;
            update[i]->levels[i].span = m_Len;
        }
        m_Level = level;
    }

    cur = new ( malloc( sizeof( Node ) + level * sizeof( typename Node::Level ) ) ) Node( value );
    for ( auto i = 0; i < level; ++i ) {
        cur->levels[i].forward = update[i]->levels[i].forward;
        update[i]->levels[i].forward = cur;

        // update span covered by update[i] as cur is insert here
        cur->levels[i].span = update[i]->levels[i].span - ( rank[0] - rank[i] );
        update[i]->levels[i].span = ( rank[0] - rank[i] ) + 1;
    }

    /* increment span for untouched levels */
    for (auto i = level; i < m_Level; ++i) {
        ++update[i]->levels[i].span;
    }

    cur->backward = (update[0] == m_Head) ? nullptr : update[0];
    if (cur->levels[0].forward) {
        cur->levels[0].forward->backward = cur;
    }
    else {
        m_Tail = cur;
    }
    ++m_Len;
}

template<typename Key, typename Score>
inline bool SkipList<Key, Score>::erase(value_type const& value)
{
    Node* cur = m_Head;
    Node* update[eMaxLevel];

    for ( auto i = m_Level - 1; i >= 0; --i ) {
        while ( cur->forward( i )
            && ( cur->forward( i )->score > value.second
                || ( cur->forward( i )->key < value.first 
                    && cur->forward( i )->score == value.second ) ) ) {
            cur = cur->forward( i );
        }
        update[i] = cur;
    }

    cur = cur->levels[0].forward;
    if (cur && cur->key == value.first) {
        eraseNode(cur, update);
        free(cur);
        return true;
    }
    return false;
}

template<typename Key, typename Score>
inline void SkipList<Key, Score>::back(Key& key) const
{
    if (m_Tail) {
        key = m_Tail->key;
    }
}

template<typename Key, typename Score>
inline bool SkipList<Key, Score>::front(Key& key) const
{
    Node* cur = m_Head;
    if (cur->levels[0].forward) {
        cur = cur->levels[0].forward;
        key = cur->ley;
        return true;
    }
    return false;
}

template<typename Key, typename Score>
inline uint32_t SkipList<Key, Score>::rank(value_type const& value) const
{
    Node* cur = m_Head;
    uint32_t rank = 0;
    for (auto i = m_Level - 1; i >= 0; --i) {
        while (cur->forward(i)
            && (cur->forward(i)->score > value.second
                || (cur->forward(i)->key <= value.first && cur->forward(i)->score == value.second))) {
            rank += cur->span(i);
            cur = cur->forward(i);
        }
        if (cur != m_Head && cur->key == value.first)
            return rank;
    }
    
    return 0;
}

template<typename Key, typename Score>
inline bool SkipList<Key, Score>::element(uint32_t rank, Key& key) const
{
    value_type value;
    if (element(rank, value)) {
        key = value.first;
        return true;
    }
    return false;
}

template<typename Key, typename Score>
inline bool SkipList<Key, Score>::element(uint32_t rank, value_type& value) const
{
    Node* cur = m_Head;
    uint32_t travesed = 0;
    for (auto i = m_Level - 1; i >= 0; --i) {
        while (cur->forward(0) && travesed + cur->span(i) < rank) {
            travesed += cur->span(i);
            cur = cur->forward(i);
        }
    }
    ++travesed;
    cur = cur->forward(0);    
    if (cur != nullptr) {
        value = value_type{ cur->key,cur->score };
    }
    return cur != nullptr;
}

template<typename Key, typename Score>
inline int32_t SkipList<Key, Score>::ranklist(uint32_t index, uint32_t count, key_list& list) const
{
    key_value_list rangelist;
    int32_t nget = rangelist(index, count, rangelist);
    for (auto i = 0; i < rangelist.size(); ++i) {
        list.push_back(rangelist[i].first);
    }
    return nget;
}

template<typename Key, typename Score>
inline int32_t SkipList<Key, Score>::ranklist(uint32_t index, uint32_t count, key_value_list& list) const
{
    Node* cur = m_Head;
    int nget = 0;
    uint32_t traversed = 0;
    index = std::max<uint32_t>(index, 1);
    
    for (auto i = m_Level - 1; i >= 0; --i) {
        while (cur->forward(i) && traversed + cur->span(i) < index) {
            traversed += cur->span(i);
            cur = cur->forward(i);
        }
    }
    ++traversed;
    cur = cur->forward(0);

    while (cur && traversed <= index + count - 1) {
        Node* next = cur->forward(0);
        ++nget;
        ++traversed;
        list.push_back(value_type(cur->key, cur->score));
        cur = next;
    }
    return nget;
}

template<typename Key, typename Score>
inline int32_t SkipList<Key, Score>::range(Score const& min, Score const& max, uint32_t count, key_list& list, key_list const& excepts) const
{
    key_value_list rangelist;
    int32_t nget = range(min, max, count, rangelist, excepts);
    for (auto i = 0; i < rangelist.size(); ++i) {
        list.push_back(rangelist[i].first);
    }
    return nget;
}

template<typename Key, typename Score>
inline int32_t SkipList<Key, Score>::range(Score const& min, Score const& max, uint32_t count, key_value_list& list, key_list const& excepts) const
{
	if (min > max) { return 0; }
	if (m_Tail == nullptr || m_Tail->score > max) {
		return 0;
	}
	if (m_Head->forward(0) == nullptr || m_Head->forward(0)->score < min) {
		return 0;
	}

	int32_t nget = 0;
	Node* cur = m_Head;
    for (auto i = m_Level - 1; i >= 0; --i) {
        while (cur->forward(i) && cur->forward(i)->score > max) {
            cur = cur->forward(i);
        }
    }
    cur = cur->forward(0);
	while (cur && list.size() < count && cur->score >= min) {
		if (excepts.end() == std::find(excepts.begin(), excepts.end(), cur->key)) {
			++nget;
			list.push_back(value_type(cur->key, cur->score));
		}
		cur = cur->forward(0);
	}
	return nget;
}

template<typename Key, typename Score>
inline void SkipList<Key, Score>::clear()
{
    Node* next = nullptr;
    Node* node = m_Head->forward(0);
    
    while (node) {
        next = node->forward(0);
        free(node);
        node = next;
    }

    // init m_Head
    for (auto i = 0; i < eMaxLevel; ++i) {
        m_Head->levels[i].span = 0;
        m_Head->levels[i].forward = nullptr;
    }
    m_Level = 1;
    m_Len = 0;
    m_Tail = nullptr;
}

template<typename Key, typename Score>
inline void SkipList<Key, Score>::print( int32_t width )
{
    Node * p = nullptr;
    Node * q = nullptr;
    std::cout << "level: " << m_Level << "\n";

    // from top level search
    for ( auto i = m_Level - 1; i >= 0; --i ) {
        p = m_Head;
        while ( ( q = p->forward( i ) ) != nullptr ) {
                        for ( auto j = 0; j < p->span( i ) - 1; ++j ) {
                            for ( auto k = 0; k < width; ++k ) {
                                std::cout << " ";
                            }
                        }
                        std::cout << std::setw( width ) << q->key << " " << q->score;
                        p = q;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

template<typename Key, typename Score>
inline int32_t SkipList<Key, Score>::randomLevel()
{
	constexpr auto SKIPLIST_P = 0.25;
    int32_t level = 1;
	while ((random() & 0xfff) < (SKIPLIST_P * 0xfff)) {
		level += 1;
	}
	return level < eMaxLevel ? level : eMaxLevel;
}

template<typename Key, typename Score>
inline void SkipList<Key, Score>::eraseNode(Node* node, Node** update)
{
    for (auto i = 0; i < m_Level; ++i) {
        if (update[i]->forwad(i) == node) {
            update[i]->levels[i].span += node->levels[i].span - 1;
            update[i]->levels[i].forward = node->levels[i].forward;
        }
        else {
            update[i]->levels[i].span -= 1;
        }
    }

    if (node->levels[0].forward) {
        node->levels[0].forward->backward = node->backward;
    }
    else {
        m_Tail = node->backward;
    }

    while (m_Level > 1 && m_Head->levels[m_Level - 1].forward == nullptr) {
        --m_Level;
    }
    --m_Len;
}

} // namespace utils
