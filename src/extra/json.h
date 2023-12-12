
#ifndef __SRC_EXTRA_JSON_H__
#define __SRC_EXTRA_JSON_H__

#include "base/battle.h"
#include "base/database.h"
#include "base/resource.h"

#include "utils/json.h"

// gm礼包码返回
struct GiftCode
{
    std::string code;
    uint32_t status;
    Resources rewards;

    GiftCode()
        : code( "" ), status( 0 )
    {
        rewards.clear();
    }
};

// Boss位置
struct BossPosition
{
    DungeonID id = 0;
    int64_t point = 0;
    BossPosition() {}
    BossPosition( DungeonID i, int64_t p ) : id(i), point(p) {}
};

// 封号原因
struct BannedReason
{
    int32_t result = 0;
    int32_t module = 0;
    std::string detail;

    BannedReason() = default;
    BannedReason( int32_t r ) : result(r) {}
    BannedReason( int32_t r, int32_t m, const std::string & d )
        : result(r), module(m), detail(d) {}
    BannedReason( int32_t r, int32_t m, int32_t prop )
        : result(r), module(m), detail(std::to_string(prop)) {}
};

// TODO：添加数据结构

#endif
