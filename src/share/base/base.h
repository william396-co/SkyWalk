#pragma once

#include <cassert>
#include <cstdlib>

#include <cstdint>
#include "format.h"

#include "utils/file.h"

// 全局日志模块
extern utils::LogFile * g_Logger;

// 定义数据库文件META目录
constexpr auto APP_META_PATH = "meta";

// 定义建库脚本以及修复脚本
constexpr auto DATASERVER_BUILD_SCRIPT = "build_database.sql";
constexpr auto DATASERVER_MODIFY_SCRIPT = "modify_database.sql";

// 日志宏定义
// #define LOGGER( level, ... ) 		    g_Logger->printp( level, "%T %P [%L] - %F - \t : ", Sourcefile(__FILE__, __LINE__), __VA_ARGS__ )
#define LOGGER( level, ... ) g_Logger->printp( level, "%T %P <%L>\t : ", __VA_ARGS__ )
#define LOG_FATAL( ... ) LOGGER( utils::LogFile::eLogLevel_Fatal, fmt::format( __VA_ARGS__ ) )
#define LOG_ERROR( ... ) LOGGER( utils::LogFile::eLogLevel_Error, fmt::format( __VA_ARGS__ ) )
#define LOG_WARN( ... ) LOGGER( utils::LogFile::eLogLevel_Warn, fmt::format( __VA_ARGS__ ) )
#define LOG_INFO( ... ) LOGGER( utils::LogFile::eLogLevel_Info, fmt::format( __VA_ARGS__ ) )
#define LOG_TRACE( ... ) LOGGER( utils::LogFile::eLogLevel_Trace, fmt::format( __VA_ARGS__ ) )
#define LOG_DEBUG( ... ) LOGGER( utils::LogFile::eLogLevel_Debug, fmt::format( __VA_ARGS__ ) )

