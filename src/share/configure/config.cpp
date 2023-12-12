
#include <cassert>

#include "keywordconfig.h"
#include "versionconfig.h"
#include "variableconfig.h"

#include "gameoptconfig.h"
#include "itemconfig.h"
#include "roleconfig.h"
#include "levelconfig.h"
#include "breakconfig.h"
#include "skillconfig.h"
#include "skillunlockconfig.h"
#include "dungeonconfig.h"
#include "arenaconfig.h"
#include "robotconfig.h"

#include "config.h"

// 全局变量
utils::ConfigCenter * g_ConfigCenter;

bool LoadConfigCenter( const char * path )
{
    // 创建
    g_ConfigCenter = new utils::ConfigCenter;
    assert( g_ConfigCenter != nullptr && "create utils::ConfigCenter failed" );

    // 添加搜索路径
    g_ConfigCenter
        ->addPath( path )
        ->addPath( "config" )
        ->addPath( "scripts" );

    // 配置中心注册

    // keyword.xml
    ADDCONFIG( KEYWORDS_CONFIGFILE, [] { return new meta::KeywordConfigfile; } );
    // exportversion.xml
    ADDCONFIG( VERSION_CONFIGFILE, [] { return new meta::VersionConfigfile; } );
    // variable.xml
    CHKADDCONFIG( VARIABLE_CONFIGFILE, [] { return new meta::VariableConfigfile; } );

    // 逻辑配置
    ADDCONFIG( GAMEOPTION_CONFIGFILE, [] { return new meta::GameOptConfigfile; } );
    ADDCONFIG( ROLELIST_CONFIGFILE, [] { return new meta::RoleConfigfile; } );
    ADDCONFIG( ITEMTOTAL_CONFIGFILE, [] { return new meta::ItemConfigfile; } );
    ADDCONFIG( ROLELEVEL_CONFIGFILE, [] { return new meta::LevelConfigfile; } );
    ADDCONFIG( ROLEBREAK_CONFIGFILE, [] { return new meta::BreakConfigfile; } );
    ADDCONFIG( SKILL_CONFIGFILE, [] { return new meta::SkillConfigfile; } );
    ADDCONFIG( SKILLUNLOCK_CONFIGFILE, [] { return new meta::SkillUnlockConfigfile; } );
    ADDCONFIG( DUNGEON_CONFIGFILE, [] { return new meta::DungeonConfigfile; } );
    ADDCONFIG( ARENA_CONFIGFILE, [] { return new meta::ArenaConfigfile; } );
    ADDCONFIG( ROBOT_CONFIGFILE, [] { return new meta::RobotConfigfile; } );

    // TODO: 添加配置文件
    return g_ConfigCenter->load();
}

void ReloadConfigCenter()
{
    g_ConfigCenter->reload();
}

void UnloadConfigCenter()
{
    g_ConfigCenter->unload();
    delete g_ConfigCenter;
}
