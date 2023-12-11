
#ifndef __SRC_BASE_ITEM_H__
#define __SRC_BASE_ITEM_H__

#include "types.h"

// 实体ID列表
typedef std::vector<EntityID> EntityIDList;

// 存放位置
typedef int16_t StorePlace;
enum
{
    eStorePlace_None = 0,      // 非法的存储位置
    eStorePlace_Bag = 1,       // 包裹
    eStorePlace_Hero = 2,      // 英雄
    eStorePlace_TotemBag = 3,  // 图腾背包
    eStorePlace_AmuletBag = 4, // 咒灵背包
};

// 穿戴位置
typedef int8_t DressSlot;
enum
{
    eDressSlot_Weapon = 1,  // 武器
    eDressSlot_Clothes = 2, // 衣服
    eDressSlot_Belts = 3,   // 腰带
    eDressSlot_Shoes = 4,   // 鞋子
    eDressSlot_Trump1 = 7,  // 饰品类宝物
    eDressSlot_Trump2 = 8,  // 收藏类宝物
    eDressSlot_Max,
};

// 物品类型
typedef uint8_t ItemType;
enum
{
    eItemType_None = 0,             // 无效类型
    eItemType_Resource = 1,         // 资源(属性)
    eItemType_Hero = 10,            // 英雄
    eItemType_HeroFrag = 11,        // 英雄碎片
    eItemType_HeroExp = 12,         // 英雄经验
    eItemType_Equip = 13,           // 装备
    eItemType_EquipFrag = 14,       // 装备碎片
    eItemType_Effect = 15,          // 效果类道具(效果类资源)
    eItemType_Gift = 16,            // 礼包

    eItemType_Common = 20,          // 通用道具
};

// 物品操作
enum
{
    eOperate_Use = 0x01,      // 使用
    eOperate_Dress = 0x02,    // 穿戴
    eOperate_Sell = 0x04,     // 出售
    eOperate_Compound = 0x08, // 合成
};

// 装备孔的状态
enum
{
    eEquip_LockSlot = 99999, // 锁定的孔
    eEquip_OpenSlot = 88888, // 开启的孔
    eEquip_AllSlot = 99,     // 万能孔(可以镶嵌任何类型的勾1 )
};

// 物品单元
struct ItemUnit
{
    ItemID id;     // 物品
    int64_t count; // 个数

    ItemUnit()
        : id( eInvalidItemID ), count( 0 ) {}
    ItemUnit( ItemID id_, int64_t count_ )
        : id( id_ ), count( count_ ) {}
};
struct EntityUnit
{
    EntityID id;
    int64_t count;

    EntityUnit()
        : id( eInvalidEntityID ), count( 0 ) {}
    EntityUnit( EntityID id_, int64_t count_ )
        : id( id_ ), count( count_ ) {}
    bool operator==( EntityID tmpid ) const { return id == tmpid; }
};
// 物品单元列表
typedef std::vector<ItemUnit> ItemUnitList;
typedef std::vector<EntityUnit> EntityUnitList;

// 商品记录
struct GoodsRecord
{
    uint32_t id = 0;    // 商品ID
    uint32_t times = 0; // 购买次数

    GoodsRecord() {}
    GoodsRecord( uint32_t id_, uint32_t times_ )
        : id( id_ ), times( times_ ) {}
    bool operator==( uint32_t id ) const { return this->id == id; }
};
typedef std::vector<GoodsRecord> GoodsRecords;

// 三元素参数组
struct ThreeParams
{
    uint32_t param1 = 0;
    uint32_t param2 = 0;
    uint32_t param3 = 0;

    ThreeParams() = default;
    ThreeParams( uint32_t p1, uint32_t p2, uint32_t p3 )
        : param1( p1 ), param2( p2 ), param3( p3 ) {}
};

inline void APPEND_ITEMS( const ItemUnit & from, ItemUnitList & to )
{
    size_t i = 0;

    for ( i = 0; i < to.size(); ++i ) {
        if ( from.id == to[i].id ) {
            to[i].count += from.count;
            break;
        }
    }

    if ( i == to.size() ) {
        to.push_back( from );
    }
}

// 图腾槽位数量
#define TOTEM_SLOT_COUNT 3
// 装备槽位数量
#define EQUIPMENT_SLOT_COUNT 8

#endif
