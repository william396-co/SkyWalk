
#include "base/role.h"
#include "base/resource.h"
#include "base/battle.h"
#include "utils/jsarray.h"
#include "domain/metasheet.h"

namespace meta {
namespace detail {

template<> bool parse( const char * src, std::string & dst ) { dst = src; return true; }
template<> bool parse( const char * src, bool & dst ) { dst = std::atoi( src ) == 1; return true; }
template<> bool parse( const char * src, float & dst ) { dst = std::atof( src ); return true; }
template<> bool parse( const char * src, double & dst ) { dst = std::atof( src ); return true; }
template<> bool parse( const char * src, int8_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, uint8_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, int16_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, uint16_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, int32_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, uint32_t & dst ) { dst = std::atoi( src ); return true; }
template<> bool parse( const char * src, int64_t & dst ) { dst = std::atoll( src ); return true; }
template<> bool parse( const char * src, uint64_t & dst ) { dst = std::atoll( src ); return true; }

template<> bool parse( const char * src, IntVec & list )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getLeafCount(); ++i ) {
        list.push_back( std::atoi( root->getLeaf( i ) ) );
    }

    return true;
}

template<> bool parse( const char * src, Int64Vec & list )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getLeafCount(); ++i ) {
        list.push_back( std::atoll( root->getLeaf( i ) ) );
    }

    return true;
}

template<> bool parse( const char * src, std::vector<double> & list )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getLeafCount(); ++i ) {
        list.push_back( std::atof( root->getLeaf( i ) ) );
    }

    return true;
}

template<> bool parse( const char * src, Fee & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() >= 2 ) {
        dst = { std::atoi( root->getLeaf( 0 ) ), std::atoll( root->getLeaf( 1 ) ) };
        return true;
    }

    return false;
}

template<> bool parse( const char * src, std::vector<Fee> & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr ) {
            if ( node->getLeafCount() >= 2 ) {
                dst.push_back( { std::atoi( node->getLeaf( 0 ) ), std::atoll( node->getLeaf( 1 ) ) } );
            }
        }
    }

    return true;
}

template<> bool parse( const char * src, Resource & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() == 2 ) {
        dst = { eResourceType_Item,
                std::atoi( root->getLeaf( 0 ) ), std::atoi( root->getLeaf( 1 ) ) };
    } else if ( root->getLeafCount() == 3 ) {
        dst = { std::atoi( root->getLeaf( 0 ) ),
                std::atoi( root->getLeaf( 1 ) ), std::atoi( root->getLeaf( 2 ) ) };
    } else if ( root->getLeafCount() == 4 ) {
        dst = { std::atoi( root->getLeaf( 0 ) ), std::atoi( root->getLeaf( 1 ) ),
                std::atoi( root->getLeaf( 2 ) ), std::atoi( root->getLeaf( 3 ) ) };
    } else if ( root->getLeafCount() == 5 ) {
        dst = { std::atoi( root->getLeaf( 0 ) ),
                std::atoi( root->getLeaf( 1 ) ), std::atoi( root->getLeaf( 2 ) ),
                std::atoi( root->getLeaf( 3 ) ), std::atoi( root->getLeaf( 4 ) ) };
    }

    return true;
}

template<> bool parse( const char * src, Resources & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr ) {
            Resource value;
            if ( node->getLeafCount() == 2 ) {
                value = { eResourceType_Item,
                        std::atoi( node->getLeaf( 0 ) ), std::atoi( node->getLeaf( 1 ) ) };
            } else if ( node->getLeafCount() == 3 ) {
                value = { std::atoi( node->getLeaf( 0 ) ),
                        std::atoi( node->getLeaf( 1 ) ), std::atoi( node->getLeaf( 2 ) ) };
            } else if ( node->getLeafCount() == 4 ) {
                value = { std::atoi( node->getLeaf( 0 ) ), std::atoi( node->getLeaf( 1 ) ),
                        std::atoi( node->getLeaf( 2 ) ), std::atoi( node->getLeaf( 3 ) ) };
            } else if ( node->getLeafCount() == 5 ) {
                value = { std::atoi( node->getLeaf( 0 ) ),
                        std::atoi( node->getLeaf( 1 ) ), std::atoi( node->getLeaf( 2 ) ),
                        std::atoi( node->getLeaf( 3 ) ), std::atoi( node->getLeaf( 4 ) ) };
            } else {
                continue;
            }
            dst.push_back( value );
        }
    }

    return true;
}

} // namespace detail
} // namespace utils
