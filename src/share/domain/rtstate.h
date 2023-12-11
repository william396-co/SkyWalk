#pragma once

#include <deque>
#include <type_traits>
#include <unordered_map>
#include "utils/types.h"
#include "utils/ipcs.h"

namespace rtstate {
template<typename K, typename V, typename A>
struct MetaBlock {
    struct Block {
        K id;
        V value;
    };

    size_t count = 0;
    A data;
    Block * blocklist = nullptr;

    A * get() { return &data; }
    void construct() { new (&data) A; }
};

template<typename K, typename V>
struct MetaBlock<K, V, void> {
    struct Block {
        K id;
        V value;
    };

    size_t count = 0;
    Block * blocklist = nullptr;

    void construct() {}
    void * get() { return nullptr; }
};
} // namespace rtstate

template<typename Key, typename Value, typename Attr = void>
class RTState
{
public :
    RTState( const Key & key = {} )
        : m_EmptyKey( key ) {}
    ~RTState() = default;

    template<class Fn>
        Fn for_each( Fn f ) const {
            for ( auto & kv : m_SlotCache ) {
                if ( kv.second != (size_t)-1 ) f( kv );
            }
            return f;
        }

public :
    bool link( const std::string & file, size_t count ) {
        m_Allocator = new utils::MMapAllocator( file.c_str() );
        if ( m_Allocator == nullptr ) {
            return false;
        }

        m_MetaBlock = (MetaBlock *)m_Allocator->alloc(
            sizeof( MetaBlock ) + sizeof( typename MetaBlock::Block ) * count );
        if ( m_MetaBlock == nullptr ) {
            return false;
        }

        return init( count );
    }

    void unlink( bool removeflag ) {
        if ( m_MetaBlock != nullptr
            && m_Allocator != nullptr ) {
            m_Allocator->free( m_MetaBlock, removeflag );
            m_MetaBlock = nullptr;
        }

        m_SlotCache.clear();
        delete m_Allocator;
    }

    Attr * data() const { return m_MetaBlock->get(); }
    bool owner() const { return m_Allocator->isOwner(); }

    ssize_t find( const Key & id ) const {
        auto pos = m_SlotCache.find( id );
        if ( pos != m_SlotCache.end() ) {
            return pos->second;
        }
        return -1;
    }

    Value * at( size_t slot ) const {
        if ( slot < m_MetaBlock->count ) {
            return &( m_MetaBlock->blocklist[slot].value );
        }
        return nullptr;
    }

    ssize_t search( const Key & id ) {
        auto slot = find( id );
        if ( slot != -1 ) {
            return slot;
        }

        if ( !m_IdleQueue.empty() ) {
            slot = m_IdleQueue.front();
            m_IdleQueue.pop_front();
            if ( slot < (ssize_t)m_MetaBlock->count ) {
                assert( m_EmptyKey
                    == m_MetaBlock->blocklist[slot].id );
                m_MetaBlock->blocklist[slot].id = id;
                m_SlotCache.emplace( id, slot );
                if ( m_IdleQueue.empty()
                    && slot + 1 < (ssize_t)m_MetaBlock->count ) {
                    m_IdleQueue.push_back( slot + 1 );
                }
                return slot;
            }
        }

        return -1;
    }

    void erase( const Key & id ) {
        auto pos = m_SlotCache.find( id );
        if ( pos != m_SlotCache.end() ) {
            auto slot = pos->second;
            m_SlotCache.erase( pos );
            m_IdleQueue.push_front( slot );
            m_MetaBlock->blocklist[slot].id = m_EmptyKey;
            new ( &m_MetaBlock->blocklist[slot].value ) Value;
        }
    }

    void clear() {
        size_t count = m_MetaBlock->count;
        m_MetaBlock->construct();
        m_SlotCache.clear();
        m_IdleQueue.clear();
        m_IdleQueue.push_front( 0 );
        for ( size_t i = 0; i < count; ++i ) {
            m_MetaBlock->blocklist[i].id = m_EmptyKey;
            new ( &m_MetaBlock->blocklist[i].value ) Value;
        }
    }

private :
    using MetaBlock = rtstate::MetaBlock<Key, Value, Attr>;

    bool init( size_t count ) {
        m_MetaBlock->blocklist = (typename MetaBlock::Block *)( m_MetaBlock + 1 );

        if ( m_Allocator->isOwner() ) {
            m_MetaBlock->construct();
            m_MetaBlock->count = count;
            m_IdleQueue.push_front( 0 );
            for ( size_t i = 0; i < count; ++i ) {
                m_MetaBlock->blocklist[i].id = m_EmptyKey;
                new ( &m_MetaBlock->blocklist[i].value ) Value;
            }
        } else {
            // 第一个空闲的槽位
            ssize_t slot = -1;
            for ( ssize_t i = m_MetaBlock->count - 1; i >= 0; --i ) {
                if ( m_MetaBlock->blocklist[i].id == m_EmptyKey ) {
                    // 不连续的空闲槽位直接添加
                    if ( slot != -1 ) m_IdleQueue.push_front( i );
                } else {
                    // 第一个连续的空闲槽位
                    if ( slot == -1 ) slot = i + 1;
                    m_SlotCache.emplace( m_MetaBlock->blocklist[i].id, i );
                }
            }
            // 把连续的空闲槽位放入最后
            if ( slot < (ssize_t)m_MetaBlock->count ) {
                m_IdleQueue.push_back( slot );
            }
            for ( size_t i = m_MetaBlock->count; i < count; ++i ) {
                m_MetaBlock->blocklist[i].id = m_EmptyKey;
                new ( &m_MetaBlock->blocklist[i].value ) Value;
            }
            m_MetaBlock->count = count;
        }

        return true;
    }

    Key m_EmptyKey;
    std::deque<size_t> m_IdleQueue;
    MetaBlock * m_MetaBlock = nullptr;
    utils::MMapAllocator * m_Allocator = nullptr;
    std::unordered_map<Key, size_t> m_SlotCache;
};

