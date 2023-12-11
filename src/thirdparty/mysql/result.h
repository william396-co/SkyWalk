#pragma once

#include <vector>
#include <stdint.h>

#include "utils/slice.h"

//
// MySQL查询结果集合
// 由于从数据库返回的字段基本都为小块string
// 为了防止出现大量的内存空洞, 所以增加了这个对象，提高内存利用率
//
class MysqlResults
{
public:
    MysqlResults();
    ~MysqlResults();

public:
    // 追加一行数据
    void newrow();
    // 添加字段
    void append( const Slice & field );
    // 取出这一行的数据
    Slices row( size_t index ) const;
    // 行数
    size_t size() const { return m_Results.size(); }

private:
    // 内存块
    struct Block;

    // 内存块尺寸范围
    enum
    {
        eBlock_MinSize = 1024 * 1024,      // 结果集默认大小1M
        eBlock_MaxSize = 32 * 1024 * 1024, // 结果集最大大小32M
    };

    // 单个字段
    struct Field
    {
        uint32_t blkidx; // 第几个Block
        uint32_t offset; // 相对于Buffer的偏移量
        uint32_t length; // 字段长度

        Field() : blkidx( 0 ), offset( 0 ), length( 0 ) {}
        Field( uint32_t idx, uint32_t off, uint32_t len ) : blkidx( idx ), offset( off ), length( len ) {}
    };

private:
    std::vector<Block *> m_Blocks;
    std::vector<std::vector<Field>> m_Results;
};
