
#include "base/role.h"
#include "base/resource.h"
#include "base/battle.h"
#include "base/hero.h"
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

template<> bool parse( const char * src, std::vector<std::string> & list )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getLeafCount(); ++i ) {
        list.push_back( root->getLeaf( i ) );
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

    if ( root->getLeafCount() == 2 ) {
        dst = { std::atoi( root->getLeaf( 0 ) ), std::atoll( root->getLeaf( 1 ) ) };
        return true;
    }

    return root->getLeafCount() == 0;
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
        if ( node != nullptr
            && node->getLeafCount() >= 2 ) {
            APPEND_FEELIST( dst, { std::atoi( node->getLeaf( 0 ) ), std::atoll( node->getLeaf( 1 ) ) } );
        }
    }

    return true;
}

template<> bool parse( const char * src, Vec2i & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() == 2 ) {
        dst.x = std::atoi( root->getLeaf( 0 ) );
        dst.y = std::atoi( root->getLeaf( 1 ) );
        return true;
    }

    return root->getLeafCount() == 0;
}

template<> bool parse( const char * src, std::vector<Vec2i> & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr
            && node->getLeafCount() >= 2 ) {
            dst.push_back( Vec2i( std::atoi(node->getLeaf(0)), std::atoi(node->getLeaf(1)) ) );
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

template<> bool parse( const char * src, Range & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() == 1 ) {
        dst = { std::atoll( root->getLeaf( 0 ) ), std::atoll( root->getLeaf( 0 ) ) };
        return true;
    } else if ( root->getLeafCount() == 2 ) {
        dst = { std::atoll( root->getLeaf( 0 ) ), std::atoll( root->getLeaf( 1 ) ) };
        return true;
    }

    return root->getLeafCount() == 0;
}

template<> bool parse( const char * src, std::vector<Range> & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr
            && node->getLeafCount() >= 1 ) {
            if ( node->getLeafCount() == 1 ) {
                dst.push_back( Range( std::atoi(node->getLeaf(0)), std::atoi(node->getLeaf(0)) ) );
            } else {
                dst.push_back( Range( std::atoi(node->getLeaf(0)), std::atoi(node->getLeaf(1)) ) );
            }
        }
    }

    return true;
}

template<> bool parse( const char * src, AttribValue & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() == 2 ) {
        dst.value = std::atoll( root->getLeaf(1) );
        dst.type = FixType::Value;
        dst.prop = std::atoi( root->getLeaf(0) );
        return true;
    } else if ( root->getLeafCount() == 3 ) {
        dst.value = std::atoll( root->getLeaf(2) );
        dst.prop = std::atoi( root->getLeaf(0) );
        dst.type = (FixType)std::atoi( root->getLeaf(1) );
        return true;
    }

    return root->getLeafCount() == 0;
}

template<> bool parse( const char * src, AttribValues & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr ) {
            if ( node->getLeafCount() == 2 ) {
                AttribValue d;
                d.value = std::atoll( node->getLeaf( 1 ) );
                d.type = FixType::Value;
                d.prop = std::atoi( node->getLeaf( 0 ) );
                dst.emplace_back( std::move(d) );
            } else if ( node->getLeafCount() == 3 ) {
                AttribValue d;
                d.value = std::atoll( node->getLeaf( 2 ) );
                d.prop = std::atoi( node->getLeaf( 0 ) );
                d.type = (FixType)std::atoi( node->getLeaf( 1 ) );
                dst.emplace_back( std::move(d) );
            }
        }
    }

    return true;
}

template<> bool parse( const char * src, TrainingHero & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    if ( root->getLeafCount() == 0 ) {
        return true;
    }

    dst.baseid = std::atoi( root->getLeaf( 0 ) );
    dst.level = std::atoi( root->getLeaf( 1 ) );
    dst.quality = std::atoi( root->getLeaf( 2 ) );
    dst.equipquality = std::atoi( root->getLeaf( 3 ) );
    dst.equiplevel = std::atoi( root->getLeaf( 4 ) );
    dst.iequiplevel = std::atoi( root->getLeaf( 5 ) );
    if ( root->getLeafCount() > 6 ) {
        dst.mysteryid = std::atoi( root->getLeaf( 6 ) );
        dst.mysteryquality = std::atoi( root->getLeaf( 7 ) );
        dst.mysterylevel = std::atoi( root->getLeaf( 8 ) );
    }

    return true;
}

template<> bool parse( const char * src, TrainingHeroList & dst )
{
    utils::JsArray parser( src );
    utils::JsNode * root = parser.parse();
    if ( root == nullptr ) {
        return false;
    }

    for ( size_t i = 0; i < root->getChildCount(); ++i ) {
        auto node = root->getChild( i );
        if ( node != nullptr ) {
            if ( node->getLeafCount() == 0 ) {
                continue;
            }
            TrainingHero hero;
            hero.baseid = std::atoi( node->getLeaf( 0 ) );
            hero.level = std::atoi( node->getLeaf( 1 ) );
            hero.quality = std::atoi( node->getLeaf( 2 ) );
            hero.equipquality = std::atoi( node->getLeaf( 3 ) );
            hero.equiplevel = std::atoi( node->getLeaf( 4 ) );
            hero.iequiplevel = std::atoi( node->getLeaf( 5 ) );
            if ( node->getLeafCount() > 6 ) {
                hero.mysteryid = std::atoi( node->getLeaf( 6 ) );
                hero.mysteryquality = std::atoi( node->getLeaf( 7 ) );
                hero.mysterylevel = std::atoi( node->getLeaf( 8 ) );
            }
            dst.emplace_back( std::move( hero ) );
        }
    }

    return true;
}

} // namespace detail
} // namespace utils
