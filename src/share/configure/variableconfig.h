
#ifndef __SRC_CONFIGURE_VARIABLECONFIG_H__
#define __SRC_CONFIGURE_VARIABLECONFIG_H__

#include "base/types.h"
#include "base/role.h"

#include "utils/types.h"
#include "utils/xmldocument.h"
#include "utils/configcenter.h"

#include "config.h"

namespace meta {

class VariableConfigfile : public utils::IConfigfile
{
public:
    VariableConfigfile();
    virtual ~VariableConfigfile();
    virtual bool load( const char * path );
    virtual void unload();
    virtual utils::IConfigfile * clone() const { return new VariableConfigfile; }

public:
    // 是否从其他服导入数据
    bool isImport() const { return m_IsImport; }

    //
    // 服务器状态
    // 1. 推荐
    // 2. 新区
    // 3. 爆满
    // 4. 合并
    // 5. 维护
    // 6. 关闭
    // 7. 爆满(拒绝创角)
    //
    uint32_t getStatus() const { return m_Status; }

    // 是否是多地区
    bool isMultiRegions() const { return m_Regions.size() > 1; }

    // 在线人数
    uint32_t getOnlineCount() const { return m_OnlineCount; }

    // 获取显示版本号
    std::string getFullVersion() const;

    // APP版本
    const std::string & getAppVersion() const { return m_AppVersion; }

    // 运营版本号/基础版本号
    void clearActivityVersion( const char * path );
    const std::string & getActivityVersion() const { return m_ActivityVersion; }

    // 获取版本列表
    const std::vector<std::string> & getVersionList() const { return m_VersionList; }

    // 获取区服分组ID
    uint32_t getZoneGroupID() const { return m_ZoneGroupID; }

    // 首充重置
    const PayReset * getPayReset( int64_t now ) const;

private:
    bool m_IsImport;                        // 是否从其他服导入数据
    uint32_t m_Status;                      // 服务器状态
    uint32_t m_ZoneGroupID;                 // 区服分组
    uint32_t m_OnlineCount;                 // 在线人数
    std::string m_AppVersion;               // 客户端版本
    std::string m_ActivityVersion;          // 运营资源版本号
    std::vector<std::string> m_Regions;     // 多区域
    std::vector<std::string> m_VersionList; // 支持的客户端版本列表
    PayResetList m_PayResetList;            // 首充重置列表
};

} // namespace meta

#define VARIABLE_CONFIGFILE "variable.xml"
#define g_VariableConfig ( (meta::VariableConfigfile *)GETCONFIG( VARIABLE_CONFIGFILE ) )

#endif
