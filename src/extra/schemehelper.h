
#ifndef __SRC_EXTRA_SCHEMEHELPER_H__
#define __SRC_EXTRA_SCHEMEHELPER_H__

#include "utils/slice.h"
#include "base/base.h"
#include "base/database.h"

namespace data {

// 注册所有表结构
class SchemeHelper;
void schemes_register( SchemeHelper * helper );

// 通用路由函数
uint8_t general_router( uint8_t count, const Tablename & table, const std::string & idxstr );

// 解析异常数据
std::pair<bool, UnitID> parse_exception( const Tablename & table, const Slice & dbdata, const std::string & scripts );

} // namespace data

#endif
