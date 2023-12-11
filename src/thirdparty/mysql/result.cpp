
#include <cstdio>
#include <cstdlib>

#include "result.h"

// 内存块
struct MysqlResults::Block
{
    char * buffer;   // 内存块的地址
    uint32_t length; // 长度
    uint32_t offset; // 当前使用的偏移量

    Block()
        : buffer( nullptr ),
          length( 0 ),
          offset( 0 )
    {}

    Block( uint32_t len )
    {
        offset = 0;
        length = len;
        buffer = (char *)std::malloc( len );
        assert( buffer != nullptr && "malloc Buffer failed" );
    }

    ~Block()
    {
        if ( buffer != nullptr ) {
            free( buffer );
        }

        length = 0;
        offset = 0;
        buffer = nullptr;
    }

    const char * pos() const
    {
        return pos( offset );
    }

    const char * pos( uint32_t off ) const
    {
        return (const char *)buffer + off;
    }

    void expand( uint32_t len )
    {
        uint32_t needlen = offset + len;

        if ( needlen <= length ) {
            return;
        }

        for ( ; length < needlen; ) {
            length <<= 1;
        }

        buffer = (char *)std::realloc( buffer, length );
        assert( buffer != nullptr && "realloc Buffer failed" );
    }
};

MysqlResults::MysqlResults()
{}

MysqlResults::~MysqlResults()
{
    for ( size_t i = 0; i < m_Blocks.size(); ++i ) {
        delete m_Blocks[i];
    }

    m_Blocks.clear();
    m_Results.clear();
}

void MysqlResults::newrow()
{
    m_Results.push_back( std::vector<Field>() );
}

void MysqlResults::append( const Slice & field )
{
    Block * block = nullptr;

    if ( !m_Blocks.empty() ) {
        block = m_Blocks.back();
    }

    // 第一块或者上一块已经满了
    if ( block == nullptr
        || block->length >= eBlock_MaxSize ) {
        block = new Block( eBlock_MinSize );
        assert( block != nullptr && "create Block failed" );
        m_Blocks.push_back( block );
    }

    // 确保内存是足够的
    block->expand( field.size() );

    uint32_t offset = block->offset;
    const char * pos = block->pos();

    block->offset += field.size();
    memcpy( (void *)pos, field.data(), field.size() );

    m_Results.back().push_back( Field( m_Blocks.size() - 1, offset, field.size() ) );
}

Slices MysqlResults::row( size_t index ) const
{
    Slices result;

    if ( index > m_Results.size() ) {
        return result;
    }

    for ( size_t i = 0; i < m_Results[index].size(); ++i ) {
        const Field & field = m_Results[index][i];

        assert( field.blkidx < m_Blocks.size() && "Field.blkidx is INVALID" );

        result.push_back(
            Slice(
                m_Blocks[field.blkidx]->pos( field.offset ), field.length ) );
    }

    return result;
}
