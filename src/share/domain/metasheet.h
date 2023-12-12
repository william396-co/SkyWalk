
#ifndef __SRC_UTILS_METASHEET_H__
#define __SRC_UTILS_METASHEET_H__

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

#include "fmt/format.h"
#include "utils/random.h"
#include "utils/jsarray.h"
#include "utils/reflection.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

namespace meta {

namespace detail {

template<typename T> bool parse( const char * src, T & dst );
template<typename... T> bool parse( const char * src, std::tuple<T...> & dst ) {
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root != nullptr ) {
        size_t j = 0;
        foreach_tuple( dst, [&]( auto & v ) {
            assert( j < root->getLeafCount() );
            parse( root->getLeaf( j++ ), v );
        } );
        return true;
    }
    return false;
}
template<typename... T> bool parse( const char * src, std::vector<std::tuple<T...>> & dst ) {
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root != nullptr ) {
        for ( size_t i = 0; i < root->getChildCount(); ++i ) {
            size_t j = 0;
            std::tuple<T...> value;
            auto node = root->getChild( i );
            foreach_tuple( value, [&]( auto & v ) {
                assert( j < node->getLeafCount() );
                parse( node->getLeaf( j++ ), v );
            } );
            dst.push_back( value );
        }
        return true;
    }
    return false;
}

template<typename T, size_t I = 0>
using ReflecKeyType = std::decay_t<decltype( utils::reflect::get<I>(std::declval<T>()))>;

template<typename C, typename T = void>
using EnableIfSequenceContainer = std::enable_if_t<utils::reflect::is_sequence_container<C>::value, T>;
template<typename C, typename T = void>
using EnableIfAssociatContainer = std::enable_if_t<utils::reflect::is_associat_container<C>::value, T>;

template<typename T>
class EmptyContainer
{
public :
    EmptyContainer() = default;
    ~EmptyContainer() = default;

    void clear() {}
};

template<typename T, typename C, std::size_t Index>
void meta_append( EmptyContainer<T> & container, T & v )
{}

template<typename T, typename C, std::size_t Index>
void meta_append( C & container, T & v, EnableIfSequenceContainer<C> * = nullptr )
{
    container.push_back( v );
}

template<typename T, typename C, std::size_t Index>
void meta_append( C & container, T & v, EnableIfAssociatContainer<C> * = nullptr )
{
    container.emplace( utils::reflect::get<Index>(v), v );
}

template<typename T, typename C>
const EnableIfAssociatContainer<C, T> * meta_random( const C & container )
{
    auto pos = utils::random_thread_local::get( container );
    if ( pos != container.end() ) {
        return &(pos->second);
    }
    return nullptr;
}

template<typename T, typename C>
const EnableIfSequenceContainer<C, T> * meta_random( const C & container )
{
    auto pos = utils::random_thread_local::get( container );
    if ( pos != container.end() ) {
        return &(*pos);
    }
    return nullptr;
}

template<typename T, typename C, typename K, std::size_t Index>
const EnableIfAssociatContainer<C, T> * meta_find( const C & container, const K & key )
{
    auto result = container.find( key );
    if ( result != container.end() ) {
        return &(result->second);
    }

    return nullptr;
}

template<typename T, typename C, typename K, std::size_t Index>
const EnableIfSequenceContainer<C, T> * meta_find( const C & container, const K & key )
{
    for ( const auto & d : container) {
        if ( utils::reflect::get<Index>(d) == key ) {
            return &(d);
        }
    }
    return nullptr;
}

template<typename T, typename C>
const EnableIfSequenceContainer<C, T> * meta_index( const C & container, std::size_t index )
{
    if ( index < container.size() ) {
        return &(container[ index ]);
    }
    return nullptr;
}

template<typename T, typename C>
const EnableIfAssociatContainer<C, T> * meta_index( const C & container, std::size_t index )
{
    assert( false && "Not Support" );
    return nullptr;
}

} // namespace detail

using XmlSheet = rapidxml::xml_node<>;
using ErrorHandler = std::function<void ( const std::string &, const std::string & )>;

template<typename T, typename Container, std::size_t I = 0>
class MetaSheetBase
{
public :
    static constexpr std::size_t Index = I;
    using KeyType = detail::ReflecKeyType<T, I>;

    MetaSheetBase() = default;
    MetaSheetBase( const std::string & name )
        : m_Sheetname( name ) {}
    ~MetaSheetBase() { clear(); }

public:
    template<typename Fn>
        Fn for_each( Fn f ) const {
            std::for_each(
                m_SheetData.begin(),
                m_SheetData.end(), [&] ( auto && p ) { f( p ); } );
            return f;
        }

    void clear() {
        m_SheetData.clear();
    }

    size_t size() const { return m_SheetData.size(); }
    bool empty() const { return m_SheetData.empty(); }
    const std::string & name() const { return m_Sheetname; }
    void name( const std::string & name ) { m_Sheetname = name; }

    const T * random() const {
        return detail::meta_random<T, Container>( m_SheetData );
    }

    const T * at( std::size_t index ) const {
        return detail::meta_index<T, Container>( m_SheetData, index );
    }

    const T * find( const KeyType & key ) const {
        return detail::meta_find<T, Container, KeyType, Index>( m_SheetData, key );
    }

    bool parse( XmlSheet * block,
        const ErrorHandler & errorhandler = nullptr,
        const std::function<bool( T & )> & postparser = nullptr )
    {
        std::unordered_set<KeyType> keymaps;
        for ( auto node = block->first_node();
              node != nullptr;
              node = node->next_sibling() ) {
            T value;
            std::vector<std::string> fields;

            utils::reflect::for_each( value, [node, &fields]( auto & v, size_t i ) {
                bool isvalid = false;
                auto field = utils::reflect::field<T>( i );
                auto data = node->first_attribute( field );
                if ( data != nullptr ) {
                    isvalid = detail::parse( data->value(), v );
                }
                if ( !isvalid ) {
                    fields.push_back( field );
                }
            } );

            const KeyType & key = utils::reflect::get<Index>( value );

            if ( !fields.empty() ) {
                if ( errorhandler != nullptr ) {
                    std::string error;
                    for ( auto & field : fields ) {
                        if ( !error.empty() ) error += ", ";
                        error += field;
                    }
                    errorhandler( block->name(),
                        fmt::format( "{}:'{}'", key, error ) );
                }
                return false;
            }

            if ( keymaps.find( key ) != keymaps.end() ) {
                errorhandler( block->name(),
                    fmt::format( "{}:'{}'", key, "{Conflict Key}" ) );
                return false;
            }

            // 后处理
            if ( postparser != nullptr ) {
                if ( !postparser( value ) ) {
                    return false;
                }
            }

            keymaps.insert( key );
            detail::meta_append<T, Container, Index>( m_SheetData, value );
        }

        return true;
    }

protected :
    Container m_SheetData;
    std::string m_Sheetname;
};

template<typename ...S>
class MetaSheetSet
{
public :
    MetaSheetSet() = default;
    MetaSheetSet( const std::tuple<S...> & sheets )
        : m_Sheets( sheets ) {}
    ~MetaSheetSet() { clear(); }

    template<typename T> T const * get() const { return &std::get<T>( m_Sheets ); }

protected :
    bool read( const std::string & file, const ErrorHandler & handler ) {
        std::vector<std::string> successlist;
        bool rc = read( file, [&]( XmlSheet * node ) -> bool {
            foreach_tuple( m_Sheets, [&]( auto & sheet ) {
                if ( sheet.name() == node->name() ) {
                    if ( sheet.parse( node, handler ) ) {
                        successlist.push_back( node->name() );
                    }
                }
            } );
            return true;
        } );
        if ( rc && successlist.size() == std::tuple_size<std::tuple<S...>>{} ) {
            return true;
        }

        std::string strsl;
        for ( auto & sheet : successlist ) {
            if ( !strsl.empty() ) strsl += ", ";
            strsl += sheet;
        }
        handler( "", strsl );
        return false;
    }

    bool read( const std::string & file, const std::function<bool (XmlSheet *)> & collector ) {
        rapidxml::file<> rawdoc( file.c_str() );
        if ( rawdoc.data() == nullptr ) {
            return false;
        }

        rapidxml::xml_document<> document;
        document.parse<0>( rawdoc.data() );
        rapidxml::xml_node<> * root = document.first_node();
        if ( root == nullptr ) {
            return false;
        }

        for ( auto node = root->first_node();
              node != nullptr; node = node->next_sibling() ) {
            if ( !collector( node ) ) {
                return false;
            }
        }

        return true;
    }

    void clear() {
        foreach_tuple( m_Sheets, [&]( auto & sheet ) { sheet.clear(); } );
    }

    template<typename T> T * get() { return &std::get<T>( m_Sheets ); }

protected :
    std::tuple<S...> m_Sheets;
};

// 默认
template<typename T, std::size_t I = 0>
using MetaSheet = MetaSheetBase<T, std::vector<T>, I>;

// 空容器
template<typename T>
using EmptyMetaSheet = MetaSheetBase<T, detail::EmptyContainer<T>>;

// 排序
template<typename T, std::size_t I = 0>
using SortedMetaSheet = MetaSheetBase<T, std::map<detail::ReflecKeyType<T, I>, T>, I>;

// 无序
template<typename T, std::size_t I = 0>
using UnsortedMetaSheet = MetaSheetBase<T, std::unordered_map<detail::ReflecKeyType<T, I>, T>, I>;

} // namespace meta

#endif
