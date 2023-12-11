
#include "scheme/sqlbind.h"

#include "querycache.h"

namespace data {

inline std::string KEYJOIN( const std::string & table, const std::string & key = "" )
{
    std::string k = table;

    k += "#";
    k += key;

    return k;
}

QueryCache::~QueryCache()
{
    for ( const auto & entity : m_DirtyCache ) {
        for ( const auto & block : entity.second ) {
            delete block.block;
        }
    }

    m_DirtyCache.clear();
    m_QueryCache.clear();
}

bool QueryCache::isQuery( const Tablename & table, const std::string & idxstr )
{
    if ( m_QueryCache.end()
        != m_QueryCache.find( KEYJOIN( table, idxstr ) ) ) {
        return true;
    }

    // 查询整表的情况
    return m_QueryCache.end() != m_QueryCache.find( KEYJOIN( table ) );
}

ISQLData * QueryCache::mergedirty( const Tablename & table, const std::string & keystr, ISQLData * data )
{
    return mergedirty( KEYJOIN( table, keystr ), data );
}

void QueryCache::clearqueue( const Tablename & table, const std::string & idxstr, TransID transid )
{
    auto result = m_QueryCache.find( KEYJOIN( table, idxstr ) );
    if ( result == m_QueryCache.end() ) {
        return;
    }

    for ( auto tid : result->second.translist ) {
        // NOTE: transid必须忽略掉，因为该事务已经回调过了
        if ( tid != transid ) {
            utils::Transaction * trans = m_TransManager->get( tid );
            if ( trans != nullptr ) {
                trans->onTrigger( nullptr );
            }
        }
    }

    m_QueryCache.erase( result );
}

void QueryCache::inqueue( const Tablename & table, const std::string & idxstr, TransID transid )
{
    std::string key = KEYJOIN( table, idxstr );

    auto result = m_QueryCache.find( key );
    if ( result == m_QueryCache.end() ) {
        // 如果查询了全表, 所有数据不再记录在单个索引下
        if ( !idxstr.empty() ) {
            result = m_QueryCache.find( KEYJOIN( table ) );
        }

        if ( result == m_QueryCache.end() ) {
            IndexEntry entry;
            entry.translist.push_back( transid );
            m_QueryCache.emplace( key, std::move(entry) );
        }
    }

    if ( result != m_QueryCache.end() ) {
        if ( result->second.translist.end()
            == std::find(
                result->second.translist.begin(),
                result->second.translist.end(), transid ) ) {
            result->second.translist.push_back( transid );
        }
    }
}

void QueryCache::outqueue( const Tablename & table, const std::string & idxstr, TransID transid )
{
    auto result = m_QueryCache.find( KEYJOIN( table, idxstr ) );
    if ( result == m_QueryCache.end() ) {
        return;
    }

    IndexEntry & entry = result->second;

    // 移除事务ID
    auto pos = std::find(
        entry.translist.begin(), entry.translist.end(), transid );
    if ( pos != entry.translist.end() ) {
        entry.translist.erase( pos );
    }

    // 查询的事务全部超时的情况下
    if ( entry.translist.empty() ) {
        // 脏数据缓存着已经没有意义了，所以全部清除
        for ( const auto & key : entry.dirtykeylist ) {
            auto dpos = m_DirtyCache.find( key );
            if ( dpos != m_DirtyCache.end() ) {
                for ( const auto & entry : dpos->second ) {
                    delete entry.block;
                }

                dpos->second.clear();
                m_DirtyCache.erase( dpos );
            }
        }

        m_QueryCache.erase( result );
    }
}

void QueryCache::add2DirtyList( const Tablename & table, const std::string & idxstr, const std::string & datakey )
{
    std::string indexkey = KEYJOIN( table, idxstr );

    // 将datakey添加到索引缓存的脏数据列表中
    // NOTICE: 查询全表的情况下，需要插入全局索引表中
    //         其他情况下，indexkey一定会在QueryCache中
    auto iresult = m_QueryCache.find( indexkey );
    if ( iresult == m_QueryCache.end() ) {
        // 插入全局索引表中
        if ( !idxstr.empty() ) {
            add2DirtyList( table, "", datakey );
        }
    } else {
        if ( iresult->second.dirtykeylist.end()
            == std::find(
                iresult->second.dirtykeylist.begin(),
                iresult->second.dirtykeylist.end(), datakey ) ) {
            iresult->second.dirtykeylist.push_back( datakey );
        }
    }
}

ISQLData * QueryCache::mergedirty( const std::string & key, ISQLData * data )
{
    // 取出脏数据
    auto result = m_DirtyCache.find( key );
    if ( result == m_DirtyCache.end() ) {
        return data;
    }

    for ( const auto & entry : result->second ) {
        if ( data == nullptr ) {
            // 当前数据作为原始ISQLData
            // 1. Intert
            // 2. Replace
            // 3. Invoke指定了replace参数
            if ( entry.block->get_method() == DBMethod::Insert
                || entry.block->get_method() == DBMethod::Replace
                || ( entry.block->get_method() == DBMethod::Invoke && entry.isreplace ) ) {
                data = entry.block;
            }
            continue;
        }

        // 继续合并
        switch ( entry.block->get_method() ) {
                // 销毁数据
            case DBMethod::Remove:
                delete data;
                data = nullptr;
                break;

                // 合并脏数据
            case DBMethod::Update:
            case DBMethod::Replace:
                data->merge( entry.block );
                break;

                // 调用脚本
            case DBMethod::Invoke:
                data->invoke( m_ProtoType->state(), entry.script );
                break;

            default:
                break;
        }

        // 销毁脏数据缓存
        delete entry.block;
    }

    // 删除脏数据
    m_DirtyCache.erase( result );
    return data;
}

void QueryCache::popdirty( const Tablename & table, const std::string & idxstr, std::vector<ISQLData *> & dirtylist )
{
    auto result = m_QueryCache.find( KEYJOIN( table, idxstr ) );
    if ( result == m_QueryCache.end() ) {
        return;
    }

    // 合并查询缓存中的所有主键(包括已经存在的主键)
    // 在datasets()中没有调用IDataAgent::merge()的情况下
    // 逻辑层的数据回调会导致无法挽回的灾难。
    auto & keylist = result->second.dirtykeylist;
    while ( !keylist.empty() ) {
        // NOTICE: 合并完成的数据返回nullptr
        ISQLData * data = mergedirty( keylist.front() );
        if ( data != nullptr ) {
            dirtylist.push_back( data );
        }

        keylist.erase( keylist.begin() );
    }
}

void QueryCache::pushdirty( const std::string & keystr, const std::string & idxstr, ISQLData * block, const std::string & script, bool isreplace )
{
    ISQLData * d = block->clone( true );
    assert( d != nullptr && "block::clone() failed" );

    // 计算缓存key以及数据key
    std::string datakey = KEYJOIN( d->tablename(), keystr );

    // 将datakey添加到索引缓存的脏数据列表中
    add2DirtyList( d->tablename(), idxstr, datakey );

    // 添加到脏数据缓存中
    auto dresult = m_DirtyCache.find( datakey );
    if ( dresult != m_DirtyCache.end() ) {
        dresult->second.emplace_back( d, script, isreplace );
    } else {
        m_DirtyCache.emplace( datakey, DataEntries{ {d, script, isreplace} } );
    }
}

} // namespace data
