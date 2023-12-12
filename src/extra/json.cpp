
#include "base/item.h"
#include "base/resource.h"
#include "base/role.h"
#include "base/hero.h"
#include "base/dungeon.h"
#include "domain/vec2.h"
#include "domain/vec3.h"

#include "json.h"

namespace utils
{

template<> void JsonWriter::put( const FixType & value ) { m_Writer.Int( (int32_t)value ); }
template<> void JsonReader::get( FixType & value ) { if ( m_CurrValue->IsInt() ) value = (FixType)m_CurrValue->GetInt(); }

#define DEFINE_JSON_OPE( T ) \
template<> void JsonReader::get( T & info ) \
{ \
    JSON_##T##_OPE( get, info ); \
} \
template<> void JsonWriter::put( const T & info ) \
{ \
    start(); \
    JSON_##T##_OPE( put, info ); \
    end(); \
} \

////////////////////////////////////////////////////////////////////////////////

#define JSON_Resource_OPE( M, C ) \
    M( "type", C.type ); \
    M( "object", C.object ); \
    M( "count", C.count ); \

#define JSON_Pair_OPE( M, C ) \
    M( "first", C.first ); \
    M( "second", C.second ); \

#define JSON_GiftCode_OPE( M, C ) \
    M( "code", C.code ); \
    M( "status", C.status ); \
    M( "rewards", C.rewards ); \

#define JSON_Vec2i_OPE( M, C ) \
    M( "x", C.x ); \
    M( "y", C.y ); \

#define JSON_Vec3i_OPE( M, C ) \
    M( "x", C.x ); \
    M( "y", C.y ); \
    M( "z", C.z ); \

#define JSON_BannedReason_OPE( M, C ) \
    M( "result", C.result ); \
    M( "module", C.module ); \
    M( "detail", C.detail ); \

#define JSON_ChargeRecord_OPE( M, C ) \
    M( "package", C.package ); \
    M( "times", C.times ); \
    M( "resetid", C.resetid ); \
    M( "timestamp", C.timestamp ); \

#define JSON_Skill_OPE( M, C ) \
    M( "id", C.id ); \
    M( "type", C.type ); \
    M( "level", C.level ); \

#define JSON_BriefHero_OPE( M, C ) \
    M( "id", C.id ); \
    M( "baseid", C.baseid ); \
    M( "helper", C.helper ); \
    M( "star", C.star ); \
    M( "grade", C.grade ); \
    M( "quality", C.quality ); \
    M( "rate", C.rate ); \
    M( "level", C.level ); \
    M( "hp", C.hp ); \
    M( "maxhp", C.maxhp ); \
    M( "battlepoint", C.battlepoint ); \

#define JSON_BriefRole_OPE( M, C ) \
    M( "id", C.id ); \
    M( "name", C.rolename ); \
    M( "level", C.level ); \
    M( "avatar", C.avatar ); \
    M( "battlepoint", C.battlepoint ); \
    M( "herolist", C.herolist ); \

#define JSON_ArenaHighLight_OPE( M, C ) \
    M( "type", C.type ); \
    M( "roleid", C.roleid ); \
    M( "value", C.value ); \

#define JSON_ArenaState_OPE( M, C ) \
    M( "season", C.season ); \
    M( "timestamp", C.timestamp ); \
    M( "giverank", C.giverank ); \
    M( "cleanrank", C.cleanrank ); \
    M( "robotcount", C.robotcount ); \
    M( "highlights", C.highlights ); \

#define JSON_ArenaNotes_OPE( M, C ) \
    M( "season", C.season ); \
    M( "board", C.board ); \

#define JSON_ArenaDaily_OPE( M, C ) \
    M( "times", C.times ); \

////////////////////////////////////////////////////////////////////////////////

DEFINE_JSON_OPE( Resource );
DEFINE_JSON_OPE( Pair );
DEFINE_JSON_OPE( GiftCode );
DEFINE_JSON_OPE( Vec2i );
DEFINE_JSON_OPE( Vec3i );
DEFINE_JSON_OPE( BannedReason );
DEFINE_JSON_OPE( ChargeRecord );
DEFINE_JSON_OPE( Skill );
DEFINE_JSON_OPE( BriefHero );
DEFINE_JSON_OPE( BriefRole );
DEFINE_JSON_OPE( ArenaHighLight );
DEFINE_JSON_OPE( ArenaState );
DEFINE_JSON_OPE( ArenaNotes );
DEFINE_JSON_OPE( ArenaDaily );

}
