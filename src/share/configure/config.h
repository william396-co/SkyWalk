
#ifndef __SRC_CONFIGURE_CONFIG_H__
#define __SRC_CONFIGURE_CONFIG_H__

#include "base/base.h"
#include "domain/metasheet.h"
#include "utils/configcenter.h"

// 全局变量
extern utils::ConfigCenter * g_ConfigCenter;

#define GETCONFIG( path ) g_ConfigCenter->get( ( path ) )
#define ADDCONFIG( path, file ) g_ConfigCenter->add( ( path ), ( file ) )
#define CHKADDCONFIG( path, file ) g_ConfigCenter->add( ( path ), ( file ), true )

// 重新加载配置中心
void ReloadConfigCenter();

// 卸载配置中心
void UnloadConfigCenter();

// 加载配置中心
bool LoadConfigCenter( const char * path );

// 基础配置文件
template<typename... S>
class BaseConfigfile : public utils::IConfigfile, public meta::MetaSheetSet<S...>
{
public :
    BaseConfigfile() = default;
    virtual ~BaseConfigfile() = default;
    // 前缀和tuple构造
    BaseConfigfile( const std::string & prefix ) : m_Prefix( prefix ) {}
    BaseConfigfile( const std::string & prefix, const std::tuple<S...> & sheets )
        : meta::MetaSheetSet<S...>(sheets), m_Prefix( prefix ) {}

    // 加载配置文件
    virtual bool load( const char * path ) {
        auto handler = [&] ( auto & sheetname, auto & fields ) {
            if ( sheetname.empty() ) {
                LOG_FATAL( "{}::load('{}') failed, missing some Sheets('{}') .\n", m_Prefix, path, fields );
            } else {
                LOG_FATAL( "{}::load('{}') : parse the Sheet(Sheetname:'{}', {}) failed .\n", m_Prefix, path, sheetname, fields );
            }
        };

        bool rc = meta::MetaSheetSet<S...>::read( path, handler );
        if ( rc ) {
            LOG_TRACE( "{}::load('{}') succeed .\n", m_Prefix, path );
        }
        return rc;
    }

    // 卸载配置文件
    virtual void unload() { meta::MetaSheetSet<S...>::clear(); }

private :
    std::string m_Prefix;
};

#endif
