
#ifndef __SRC_BASE_HERO_H__
#define __SRC_BASE_HERO_H__

#include "role.h"
#include "battle.h"

// 简要英雄信息
struct BriefHero {
    HeroID id = 0;               // 流水id
    int32_t baseid = 0;          // 配置id
    int32_t helper = 0;          // 支援位
    int32_t star = 0;            // 星级
    int32_t grade = 0;           // 品阶
    int32_t quality = 0;         // 品质
    int32_t rate = 0;            // 评级
    int32_t level = 0;           // 等级
    int64_t battlepoint = 0;     // 战斗力
    Attribute<>::Type hp = 0;    // 当前血量
    Attribute<>::Type maxhp = 0; // 最大血量
};

// 简要角色信息
struct BriefRole {
    UnitID id = 0;                   // 角色id
    std::string rolename;            // 角色名
    int32_t level = 0;               // 等级
    uint32_t avatar = 0;             // 形象
    int64_t battlepoint = 0;         // 战斗力
    std::vector<BriefHero> herolist; // 英雄列表
};

// 基础英雄信息
struct BasicHero {
    HeroID id = 0;                    // 流水ID
    int32_t baseid = 0;               // 配置ID
    int32_t helper = 0;               // 支援位
    int32_t star = 0;                 // 星级
    int32_t grade = 0;                // 品级
    int32_t quality = 0;              // 品质
    int32_t rate = 0;                 // 评级
    int32_t level = 0;                // 等级
    int64_t battlepoint = 0;          // 战斗力
    SkillList spells;                 // 技能
    Attribute<> attribute;            // 属性
    Attribute<>::Type hp = {};        // 当前血量
    Attribute<>::Type indamage = {};  // 输入伤害
    Attribute<>::Type outdamage = {}; // 输出伤害

    BasicHero() = default;
    BasicHero( HeroID id_, int32_t baseid_ ) : id(id_), baseid(baseid_) {}
    bool operator== ( HeroID other ) const { return id == other; }
};
using BasicHeros = std::vector<BasicHero>;

// 英雄培养信息
struct TrainingHero {
    int32_t baseid = 0;                 // 配置ID
    int32_t level = 0;                  // 等级
    int32_t quality = 0;                // 品质
    int32_t equipquality = 0;           // 装备品质
    int32_t equiplevel = 0;             // 装杯等级
    int32_t iequiplevel = 0;            // 专属装备等级
    int32_t mysteryid = 0;              // 装备奥义id
    int32_t mysteryquality = 0;         // 装备奥义品质
    int32_t mysterylevel = 0;           // 装备奥义等级

    TrainingHero() = default;
};
using TrainingHeroList = std::vector<TrainingHero>;

#endif
