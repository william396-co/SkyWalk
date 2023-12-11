
#ifndef __SRC_EXTRA_ROBOT_H__
#define __SRC_EXTRA_ROBOT_H__

#include "base/hero.h"
#include "unit.h"

class Robot final : public Unit
{
public:
    Robot( UnitID id, int32_t baseid );
    virtual ~Robot() final;
    virtual int32_t getBaseID() const final { return m_BaseID; }
    virtual int32_t getLevel() const final { return m_Level; }
    virtual int32_t getVipLevel() const final { return m_VipLevel; }
    virtual RaceType getRaceType() const final { return 0; }
    virtual const char * getName() const final { return m_RobotName.c_str(); }
    virtual uint32_t getAvatarID() const final { return m_AvatarID; }
    virtual void getAvatarInfo( AvatarInfo & avatar ) const final {}

public:
    bool initialize();
    void finalize();

    BriefRole brief() const;
    int64_t getBattlepoint() const { return m_Battlepoint; }
    const BasicHeros & getHeroList() const { return m_HeroList; }

public:
    int64_t getBattlepoint( const TrainingHero * hero, const Attribute<> & attribute, const SkillList & spells );
    int64_t getAttribute( const TrainingHero * hero, const TrainingHero * helper, Attribute<> & attribute, SkillList & spells );

private:
    // 计算属性
    bool calculate( const TrainingHero * hero, Attribute<double> & attribute, AttribValues & percent );

private:
    int32_t m_BaseID = 0;      // 配置ID
    int32_t m_Level = 1;       // 等级
    int32_t m_VipLevel = 0;    // vip等级
    std::string m_RobotName;   // 机器名
    uint32_t m_AvatarID = 0;   // 头像
    int64_t m_Battlepoint = 0; // 战斗力
    BasicHeros m_HeroList;     // 伙伴列表
};

#endif // __SRC_EXTRA_ROBOT_H__
