
#ifndef __SRC_EXTRA_SHELLCOMMAND_H__
#define __SRC_EXTRA_SHELLCOMMAND_H__

#include <string>
#include <unordered_map>
#include "base/format.h"

class ShellCmdDocument
{
public:
    ShellCmdDocument() = default;
    ~ShellCmdDocument() = default;

private :
    struct CmdDocument {
        std::string desc;
        std::string help;
        CmdDocument() = default;
        CmdDocument( const std::string & d, const std::string & h )
            : desc( d ), help( h ) {}
    };

    std::unordered_map<std::string, CmdDocument> m_ShellCommands;

public :
    bool has( const std::string & cmd ) const {
        return m_ShellCommands.find( cmd ) != m_ShellCommands.end();
    }

    std::string docs() const {
        std::string data;
        for ( auto && kv : m_ShellCommands ) {
            data += fmt::format( "{} {} .\n",
                kv.second.desc, kv.second.help.empty() ? kv.first : kv.second.help );
        }
        return data;
    }

    void add( const std::string & cmd, const std::string & desc, const std::string & help = "" ) {
        m_ShellCommands.emplace( cmd, CmdDocument{ desc, help } );
    }

    void initialize() {
        add( "#gmclock", "查看/设置服务器时间", "#gmclock [reset|timestamp]" );
        add( "#zonedata", "查看区服信息" );
        add( "#flushrole", "强制玩家数据落地" );
        add( "#add", "添加角色属性", "#add <prop> <value>" );
        add( "#make", "创建物品到背包中", "#make <itemid> <count>" );
        add( "#pay", "模拟支付", "#pay <package> <number>" );
        add( "#recruit", "招募英雄", "#recruit <heroid>" );
        add( "#attribute", "获取属性", "#attribute <heroid> [type]" );
        add( "#clearattr", "设置战斗属性", "#clearattr <heroid>" );
        add( "#setattr", "设置战斗属性", "#setattr <heroid> <attribtype> <value>" );
        add( "#update_dungeon_progress", "更新关卡进度", "#update_dungeon_progress <dungeonid>" );

        // TODO: 继续添加
    }
};

#endif
