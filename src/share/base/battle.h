#pragma once


#include <map>
#include <deque>
#include <sstream>
#include <unordered_map>
#include <cassert>

#include "role.h"
#include "types.h"
#include "domain/vec2.h"

//
// 战报长度
//
#define COMBATVIDEOLEN 65535

//
// 类型定义
//
typedef int32_t EffectType; // 效果类型

//
// 常量定义
//

// 战斗模式
enum class CombatMode
{
    None = 0,   // 非法
    Auto = 1,   // 自动
    Manual = 2, // 手动
    SemiAuto = 3, // 半自动
};

// 攻击模式
enum
{
    eAttackMode_None = 0,      // 非法
    eAttackMode_Peace = 101,   // 和平模式
    eAttackMode_Neutral = 201, // 中立模式
    eAttackMode_Killall = 301, // 通杀模式
};

// 战斗方
enum CombatSide
{
    eCombatSide_None = 0,  // 非法
    eCombatSide_Left = 1,  // 攻方
    eCombatSide_Right = 2, // 守方
    eCombatSide_Draw = 3,  // 平局
};

// 结果
enum CombatResult
{
    eCombatResult_None = 0, // 无结果
    eCombatResult_Win = 1, // 获胜
    eCombatResult_Lose = 2, // 失败
    eCombatResult_Draw = 3, // 平局
};

// 技能类型
enum
{
    eSkillType_Normal = 0,  // 普攻
    eSkillType_Kill = 1,    // 必杀
    eSkillType_Combo = 2,   // 连招
    eSkillType_Passive = 4, // 被动
};

// 对象类型
enum
{
    eObjectType_None = 0,      // 非法
    eObjectType_Role = 1,      // 角色 or 机器人
    eObjectType_Monster = 2,   // 怪物
    eObjectType_WorldBoss = 3, // 世界BOSS
    eObjectType_Trap = 4,      // 陷阱
    eObjectType_Summon = 5,    // 召唤物
    eObjectType_Stuff = 6,     // 掉落物品
};

// 对象分类
enum
{
    eObjectCategory_None = 0,    // 非法
    eObjectCategory_Role = 1,    // 角色
    eObjectCategory_Monster = 2, // 怪物
    eObjectCategory_Stuff = 4,   // 道具
    eObjectCategory_Trap = 8,    // 陷阱
    eObjectCategory_All = eObjectCategory_Role | eObjectCategory_Monster | eObjectCategory_Stuff | eObjectCategory_Trap,
};

// 控制类状态
enum
{
    eCtrlState_Freeze = 1,    // 冰冻
    eCtrlState_Hidden = 2,    // 隐身
    eCtrlState_Disabling = 3, // 无法攻击
    eCtrlState_Precasting = 4, // 施法前摇
    eCtrlState_Casting = 5,    // 释法中
    eCtrlState_Talking = 6,    // 对话
    eCtrlState_Taunt = 7,      // 嘲讽
};

// 战斗指令
struct BattleOrder
{
    UnitID roleid = 0;              // 角色ID
    int64_t heroid = 0;             // 英雄ID
    int32_t action = 0;             // 行为
    Int64Vec values;                // 参数列表

    BattleOrder() = default;
    BattleOrder( UnitID r, int64_t h, int32_t a, Int64Vec && v )
        : roleid(r), heroid(h), action(a), values(std::move(v)) {}
};
using BattleOrders = std::vector<BattleOrder>;

struct BattleFrame
{
    int32_t frame = 0;              // 帧数
    BattleOrders orderlist;         // 指令集合

    BattleFrame() = default;
    BattleFrame( int32_t f, BattleOrders && orders )
        : frame( f ), orderlist( std::move(orders) ) {}
    bool operator== ( int32_t f ) const { return frame == f; }
};
using BattleFrames = std::vector<BattleFrame>;

// 属性值
struct AttribValue
{
    FixType type = FixType::None; // 值类型
    int32_t prop = 0;             // 属性类型
    int64_t value = 0;            // 值

    AttribValue() = default;
    AttribValue( FixType t, int32_t p, int64_t v )
        : type( t ), prop( p ), value( v ) {}

    bool valid() const { return type != FixType::None && prop != 0; }
    bool operator==( const AttribValue & v ) const { return v.type == type && v.prop == prop; }
};

typedef std::vector<AttribValue> AttribValues;

// 战斗相关属性
template<typename T = int64_t>
struct Attribute
{
    enum {
        None = 0,           // 非法属性
        Hp = 1,             // 当前生命值
        CurEnergy = 2,      // 当前的能量
        HpRate = 3,         // HP百分比
        Hard = 4,           // 硬度
        MaxHp = 11,         // 生命上限
        Attack = 12,        // 攻击力
        Defence = 13,       // 防御力
        Speed = 14,         // 速度
        Crit = 15,          // 暴击率
        CritResist = 16,    // 暴击抵抗率
        CritDmg = 17,       // 暴击伤害率
        CritDmgResist = 18, // 暴伤抵抗率
        Hit = 19,           // 命中率
        Dodge = 20,         // 闪避率
        Parry = 21,         // 格挡率
        ParryValue = 22,    // 格挡值
        Heal = 23,          // 治疗率
        BeHealed = 24,      // 受治疗率
        EffectHit = 25,     // 效果命中率
        EffectResist = 26,  // 效果抵抗率
        AmplifyDmg = 27,    // 伤害增加率
        ReduceDmg = 28,     // 伤害减少率
        Blood = 29,         // 吸血率
        Energy = 30,        // 能量效率
        AmplifyCtl = 31,    // 控制增加率
        ReduceCtl = 32,     // 控制减少率
        PvpAmplifyDmg = 33, // PVP伤害增加率
        PvpReduceDmg = 34,  // PVP伤害减少率
    };

    static const int Begin = MaxHp;
    static const int End = PvpReduceDmg + 1;

    typedef T Type;
    std::vector<T> values;

    Attribute() {
        values.resize(End-Begin);
    }

    Attribute( const std::vector<T> & attrlist ) {
        values.resize(End-Begin);
        assert( values.size() == attrlist.size() );
        for ( size_t i = 0; i < values.size(); ++i ) {
            values[i] = attrlist[i];
        }
    }

    void clear() {
        std::for_each( values.begin(),
            values.end(), [] ( auto && v ) { v = {}; } );
    }

    template <class Fn>
        Fn for_each( Fn f ) const {
            std::for_each( values.begin(),
                values.end(),
                [&]( auto && v ) { f( v ); } );
            return f;
        }

    template <typename V>
        Attribute( const Attribute<V> & attr ) {
            attr.for_each(
                [&]( auto && v ) { values.push_back( v ); } );
            values.resize( End - Begin );
        }

    void set( int32_t type, T value ) {
        size_t index = type - Begin;
        if ( index < values.size() ) {
            values[index] = value;
        }
    }

    T & get( int32_t type ) { return values.at( type - Begin ); }
    T get( int32_t type ) const { return values.at( type - Begin ); }

    T & operator[]( int32_t type ) { return get( type ); }
    T operator[]( int32_t type ) const { return get( type ); }

    template<typename V>
        Attribute<T> & operator=( const Attribute<V> & addon ) {
            assert( values.size() == addon.values.size() );
            for ( size_t i = 0; i < values.size(); ++i ) {
                values[i] = addon.values[i];
            }
            return *this;
        }

    template<typename V>
        Attribute<T> & operator*=( V factor ) {
            std::for_each(
                values.begin(), values.end(),
                [&] ( auto && v ) { v *= factor; } );
            return *this;
        }

    template<typename V>
        Attribute<T> & operator+=( V factor ) {
            std::for_each(
                values.begin(), values.end(),
                [&] ( auto && v ) { v += factor; } );
            return *this;
        }

    template<typename V>
        Attribute<T> & operator-=( V factor ) {
            std::for_each(
                values.begin(), values.end(),
                [&] ( auto && v ) { v -= factor; } );
            return *this;
        }

    template<typename V>
        Attribute<T> & operator-=( const Attribute<V> & addon ) {
            assert( values.size() == addon.values.size() );
            for ( size_t i = 0; i < values.size(); ++i ) {
                values[i] -= addon.values[i];
            }
            return *this;
        }

    template<typename V>
        Attribute<T> & operator+=( const Attribute<V> & addon ) {
            assert( values.size() == addon.values.size() );
            for ( size_t i = 0; i < values.size(); ++i ) {
                values[i] += addon.values[i];
            }
            return *this;
        }

    void add( int32_t type, T value ) {
        size_t index = type - Begin;
        if ( index < values.size() ) {
            values[index] += value;
        }
    }

    void add( FixType type, int32_t prop, T base, T addon ) {
        if ( type == FixType::Percent ) {
            addon = base * ( addon / (double)PERCENT_BASE_VALUE );
        }
        add( prop, addon );
    }

    void add( const AttribValue & addon, AttribValues & percent ) {
        if ( addon.type == FixType::Value ) {
            add( addon.prop, addon.value );
        } else if ( addon.type == FixType::Percent ) {
            auto result = std::find(
                percent.begin(), percent.end(), addon );
            if ( result == percent.end() ) {
                percent.push_back( addon );
            } else {
                result->value += addon.value;
            }
        }
    }

    // 转换
    std::map<std::string, T> convert() const {
        std::map<std::string, T> value;
        for ( size_t i = 0; i < values.size(); ++i ) {
            value.emplace( tostring(Begin + i), values[i] );
        }
        return value;
    }

    static std::string tostring( int32_t type ) {
        std::string value;
        switch ( type ) {
            case MaxHp : value = "maxhp"; break;
            case Attack : value = "attack"; break;
            case Defence : value = "defence"; break;
            case Speed : value = "speed"; break;
            case Crit : value = "crit"; break;
            case CritResist : value = "critresist"; break;
            case CritDmg : value = "critdmg"; break;
            case CritDmgResist : value = "critdmgresist"; break;
            case Hit : value = "hit"; break;
            case Dodge : value = "dodge"; break;
            case Parry : value = "parry"; break;
            case ParryValue : value = "parryvalue"; break;
            case Heal : value = "heal"; break;
            case BeHealed : value = "behealed"; break;
            case EffectHit : value = "effecthit"; break;
            case EffectResist : value = "effectresist"; break;
            case AmplifyDmg : value = "amplifydmg"; break;
            case ReduceDmg : value = "reducedmg"; break;
            case Blood : value = "blood"; break;
            case Energy : value = "energy"; break;
            case AmplifyCtl : value = "amplifyctl"; break;
            case ReduceCtl : value = "reducectl"; break;
            case PvpAmplifyDmg : value = "pvpamplifydmg"; break;
            case PvpReduceDmg : value = "pvpreducedmg"; break;
        }
        return value;
    }
};

// 血量
typedef std::pair<HeroID, Attribute<>::Type> HeroHP;
typedef std::map<HeroID, Attribute<>::Type> FormationHP;
// 伤害列表
typedef std::pair<UnitID, Attribute<>::Type> DamageUnit;
typedef std::vector<std::pair<UnitID, Attribute<>::Type>> DamageTable;

struct DamageResult
{
    double damage = 0.0f;
    int32_t element = 0;
    DamageResult() = default;
    DamageResult( double d, int32_t e ) : damage(d), element(e) {}
};

struct FormulaTable
{
    double paramA = 0.0f;
    double paramB = 0.0f;
    double paramC = 0.0f;
    double paramD = 0.0f;
    double paramE = 0.0f;
    double paramF = 0.0f;
    double lowervalue = 0.0f;
    double uppervalue = 0.0f;

    FormulaTable() = default;
    FormulaTable( double a, double b, double c, double d, double e, double f, double lower, double upper )
        : paramA(a), paramB(b), paramC(c), paramD(d), paramE(e), paramF(f), lowervalue(lower), uppervalue(upper) {}
};

struct FactorInfo
{
    FixType type = FixType::None;
    double factor = 0.0f;
    int64_t fixvalue = 0;
    FactorInfo() {}
    FactorInfo(FixType t, double f, int64_t v) : type(t),factor(f),fixvalue(v) {}
};
typedef std::unordered_map<int32_t, FactorInfo> FactorTable;

