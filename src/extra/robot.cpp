
#include "robot.h"

#include "configure/robotconfig.h"
#include "configure/roleconfig.h"
#include "configure/breakconfig.h"
#include "configure/levelconfig.h"

Robot::Robot( UnitID id, int32_t baseid )
    : Unit( id ),
      m_BaseID( baseid )
{}

Robot::~Robot()
{}

bool Robot::initialize()
{
    // 机器人配置
    auto conf = g_RobotBaseConfig->find( m_BaseID );
    if ( conf == nullptr ) {
        return false;
    }

    m_Level = conf->level;
    m_VipLevel = conf->viplevel;
    m_AvatarID = conf->avatar;
    m_RobotName = g_RobotConfig->randRobotname();

    // 获取阵型
    conf->for_each( [&]( const auto * hero, const auto * helper ) {
        BasicHero basic;
        basic.id = 0;
        basic.baseid = hero->baseid;
        basic.helper = ( helper != nullptr ? helper->baseid : 0 );
        basic.star = 1;
        basic.grade = 0;
        basic.quality = hero->quality;
        basic.rate = g_RoleBaseConfig->getRate( hero->baseid );
        basic.level = hero->level;
        basic.battlepoint = getAttribute( hero, helper, basic.attribute, basic.spells );
        m_Battlepoint += basic.battlepoint;
        m_HeroList.emplace_back( std::move( basic ) );
    } );

    return true;
}

void Robot::finalize()
{
    m_HeroList.clear();
}

BriefRole Robot::brief() const
{
    BriefRole brief;

    brief.id = getID();
    brief.avatar = m_AvatarID;
    brief.battlepoint = m_Battlepoint;
    brief.level = m_Level;
    brief.rolename = m_RobotName;
    std::for_each( m_HeroList.begin(), m_HeroList.end(), [&] ( auto && h ) {
        BriefHero hero;
        hero.id = h.id;
        hero.baseid = h.baseid;
        hero.helper = h.helper;
        hero.star = h.star;
        hero.grade = h.grade;
        hero.quality = h.quality;
        hero.rate = h.rate;
        hero.level = h.level;
        hero.battlepoint = h.battlepoint;
        hero.hp = h.hp;
        hero.maxhp = h.attribute[Attribute<>::MaxHp];
        brief.herolist.emplace_back( std::move( hero ) );
    } );

    return brief;
}

int64_t Robot::getAttribute( const TrainingHero * hero, const TrainingHero * helper, Attribute<> & attribute, SkillList & spells )
{
    AttribValues percent;
    Attribute<double> base, total;

    // 英雄基础属性
    calculate( hero, total, percent );

    // TODO: 叠加其他的基础属性

    // 基础属性计算完成
    base = total;

    // TODO: 计算其他模块的属性

    // 则算整形的属性
    attribute = total;

    // 支援位的属性加成
    if ( helper != nullptr ) {
        SkillList hspells;
        Attribute<> hattribute;
        getAttribute( helper, nullptr, hattribute, hspells );
        // TODO: 折算百分比叠加到hero上
    }

    // 计算战斗力
    return getBattlepoint( hero, attribute, spells );
}

int64_t Robot::getBattlepoint( const TrainingHero * hero, const Attribute<> & attribute, const SkillList & spells )
{
    // TODO:

    return 100;
}

bool Robot::calculate( const TrainingHero * hero, Attribute<double> & attribute, AttribValues & percent )
{
    auto conf = g_RoleBaseConfig->find( hero->baseid );
    if ( conf == nullptr
        || conf->lvAttrFix.size() < 3 ) {
        return false;
    }

    // 基础属性
    attribute += conf->attribute;

    auto rankconf = g_RankAttrConfig->find( hero->quality );
    if ( rankconf == nullptr
        || rankconf->attrFix.size() < 3
        || rankconf->rankAttrFix.size() < 3 ) {
        return false;
    }

    // 品质基础修正
    attribute[Attribute<>::MaxHp] *= PERCENT( rankconf->attrFix[0] );
    attribute[Attribute<>::Attack] *= PERCENT( rankconf->attrFix[1] );
    attribute[Attribute<>::Defence] *= PERCENT( rankconf->attrFix[2] );

    auto levelconf = g_LevelAttrConfig->find( hero->level );
    if ( levelconf == nullptr
        || levelconf->attr.size() < 3 ) {
        return false;
    }

    auto jobconf = g_JobAttrConfig->find( conf->job );
    if ( jobconf == nullptr
        || jobconf->attrFix.size() < 3 ) {
        return false;
    }

    // 等级提升
    std::for_each( levelconf->attr.begin(), levelconf->attr.end(), [&]( auto && addon ) {
        if ( addon.prop == Attribute<>::MaxHp ) {
            attribute[Attribute<>::MaxHp]
                += addon.value * PERCENT( jobconf->attrFix[0] ) * PERCENT( rankconf->rankAttrFix[0] ) * PERCENT( conf->lvAttrFix[0] );
        } else if ( addon.prop == Attribute<>::Attack ) {
            attribute[Attribute<>::Attack]
                += addon.value * PERCENT( jobconf->attrFix[1] ) * PERCENT( rankconf->rankAttrFix[1] ) * PERCENT( conf->lvAttrFix[1] );
        } else if ( addon.prop == Attribute<>::Defence ) {
            attribute[Attribute<>::Defence]
                += addon.value * PERCENT( jobconf->attrFix[2] ) * PERCENT( rankconf->rankAttrFix[2] ) * PERCENT( conf->lvAttrFix[2] );
        }
    } );

    return true;
}
