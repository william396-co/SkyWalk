
#include <string.h>
#include <sys/time.h>

#include "base/base.h"
#include "utils/utility.h"

#include "result.h"
#include "engine.h"

MysqlEngine::MysqlEngine( const std::string & host, uint16_t port )
    : m_Host( host ),
      m_Port( port ),
      m_Errorcode( 0 ),
      m_DBHandler( nullptr ),
      m_MaxPacketLength( 0 )
{}

MysqlEngine::~MysqlEngine()
{}

void MysqlEngine::start()
{
    mysql_library_init( 0, nullptr, nullptr );
}

void MysqlEngine::stop()
{
    mysql_library_end();
}

bool MysqlEngine::createdb( const std::string & database, const std::string & charsets )
{
    char sqlcmd[1024];

    if ( !charsets.empty() ) {
        snprintf( sqlcmd, 1023,
                "CREATE DATABASE IF NOT EXISTS %s Character Set %s", database.c_str(), charsets.c_str() );
    } else {
        snprintf( sqlcmd, 1023,
                "CREATE DATABASE IF NOT EXISTS %s Character Set %s", database.c_str(), m_Charsets.c_str() );
    }

    // 创建数据库
    return mysql_real_query( m_DBHandler, sqlcmd, strlen( sqlcmd ) ) == 0;
}

bool MysqlEngine::selectdb( const std::string & database )
{
    m_Database = database;

    if ( m_DBHandler != nullptr ) {
        return 0 == mysql_select_db( m_DBHandler, m_Database.c_str() );
    }

    return false;
}

void MysqlEngine::setCharsets( const std::string & charsets )
{
    m_Charsets = charsets;
}

void MysqlEngine::setToken( const std::string & user, const std::string & passwd )
{
    m_Username = user;
    m_Password = passwd;
}

bool MysqlEngine::initialize()
{
    // 初始化句柄
    m_DBHandler = mysql_init( nullptr );
    if ( m_DBHandler == nullptr ) {
        return false;
    }

    mysql_thread_init();

    // 设置属性
    bool flags = true;
    int32_t timeout = 0;

    // 尝试重新连接
    mysql_options( m_DBHandler, MYSQL_OPT_RECONNECT, (const char *)&flags );
    // 设置字符集
    mysql_options( m_DBHandler, MYSQL_SET_CHARSET_NAME, m_Charsets.c_str() );
    // 连接超时
    timeout = eMysqlEngine_ConnectTimeout;
    mysql_options( m_DBHandler, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout );
    // 读超时
    timeout = eMysqlEngine_ReadTimeout;
    mysql_options( m_DBHandler, MYSQL_OPT_READ_TIMEOUT, (const char *)&timeout );
    // 写超时
    timeout = eMysqlEngine_WriteTimeout;
    mysql_options( m_DBHandler, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout );

    // 连接MySQL服务器
    if ( mysql_real_connect( m_DBHandler,
             m_Host.c_str(),
             m_Username.c_str(), m_Password.c_str(),
             m_Database.c_str(), m_Port, nullptr, 0 ) == nullptr ) {
        mysql_close( m_DBHandler );
        m_DBHandler = nullptr;
        return false;
    }

    return true;
}

void MysqlEngine::finalize()
{
    if ( m_DBHandler != nullptr ) {
        mysql_close( m_DBHandler );
        m_DBHandler = nullptr;
        mysql_thread_end();
    }
}

void MysqlEngine::keepalive()
{
    int32_t rc = 0;

    rc = mysql_ping( m_DBHandler );
    if ( rc != 0 ) {
        LOG_ERROR( "MysqlEngine::keepalive() : Result:{}, Error:({},'{}') .\n",
            rc, mysql_errno( m_DBHandler ), mysql_error( m_DBHandler ) );
    }

    // 查询最大长度
    //m_MaxPacketLength = queryMaxPacketLength();
}

void MysqlEngine::version( std::string & ver ) const
{
    unsigned long ver_number = mysql_get_client_version();

    uint32_t major_version = ver_number / 10000;
    uint32_t release_version = ( ver_number % 10000 ) / 100;
    uint32_t sub_version = ( ver_number % 10000 ) % 100;

    utils::Utility::snprintf( ver, 256, "%u.%u.%u", major_version, release_version, sub_version );
}

void MysqlEngine::escape( const std::string & src, std::string & dst )
{
    // 重置dst长度
    dst.resize( src.size() * 2 + 1 );

    char * to = const_cast<char *>( dst.data() );
    size_t len = mysql_real_escape_string(
        m_DBHandler, to, (const char *)src.data(), src.size() );

    dst.resize( len );
}

bool MysqlEngine::sqlbind( std::string & sqlcmd,
    const std::string & from, const std::vector<std::string> & values )
{
    size_t index = 0;

    for ( size_t i = 0; i < from.size(); ++i ) {
        if ( from[i] != '?' ) {
            sqlcmd.push_back( from[i] );
            continue;
        }

        if ( index >= values.size() ) {
            sqlcmd.clear();
            return false;
        }

        sqlcmd += values[index++];
    }

    return true;
}

void MysqlEngine::escapesqlcmd(
    std::string & sqlcmd, MysqlEngine * engine,
    const std::string & from, const std::vector<std::string> & values )
{
    std::vector<std::string> escapevalues;

    if ( values.empty() ) {
        sqlcmd = from;
        return;
    }

    for ( size_t i = 0; i < values.size(); ++i ) {
        std::string escapevalue;
        engine->escape( values[i], escapevalue );
        escapevalues.push_back( escapevalue );
    }

    MysqlEngine::sqlbind( sqlcmd, from, escapevalues );
}

bool MysqlEngine::execute( const std::string & sqlcmd )
{
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::execute(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    // 获取影响的行数
    LOG_DEBUG( "MysqlEngine::execute(SQLCMD:'{}') : Elapsed:{}(msecs) .\n", sqlcmd, MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::insert( const std::string & sqlcmd, uint64_t & insertid )
{
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::insert(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    // 获取返回的自增ID
    insertid = mysql_insert_id( m_DBHandler );
    LOG_DEBUG( "MysqlEngine::insert(SQLCMD:'{}') : InsertID:{}, Elapsed:{}(msecs) .\n", sqlcmd, insertid, MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::query( const std::string & sqlcmd, MysqlResults * results )
{
    MYSQL_ROW row;
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    MYSQL_RES * result = mysql_store_result( m_DBHandler );
    if ( result == nullptr ) {
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Store Result Failed .\n", sqlcmd );
        return false;
    }

    uint32_t nfields = mysql_num_fields( result );

    while ( ( row = mysql_fetch_row( result ) ) != nullptr ) {
        unsigned long * lengths = mysql_fetch_lengths( result );

        results->newrow();

        for ( uint32_t i = 0; i < nfields; ++i ) {
            results->append( Slice( row[i], lengths[i] ) );
        }
    }

    mysql_free_result( result );
    LOG_DEBUG( "MysqlEngine::query(SQLCMD:'{}') : ResultCount:{}, Elapsed:{}(msecs) .\n", sqlcmd, results->size(), MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::query( const std::string & sqlcmd, IResultHandler * handler )
{
    MYSQL_ROW row;
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    MYSQL_RES * result = mysql_store_result( m_DBHandler );
    if ( result == nullptr ) {
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Store Result Failed .\n", sqlcmd );
        return false;
    }

    uint32_t nresults = mysql_num_rows( result );
    uint32_t nfields = mysql_num_fields( result );

    // 预留handler
    handler->reserve( nresults );

    while ( ( row = mysql_fetch_row( result ) ) != nullptr ) {
        Slices slicerow;
        unsigned long * lengths = mysql_fetch_lengths( result );

        for ( uint32_t i = 0; i < nfields; ++i ) {
            slicerow.push_back( Slice( row[i], lengths[i] ) );
        }

        // 回调
        handler->process( slicerow );
    }

    mysql_free_result( result );
    LOG_DEBUG( "MysqlEngine::query(SQLCMD:'{}') : ResultCount:{}, Elapsed:{}(msecs) .\n", sqlcmd, nresults, MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::query( const std::string & sqlcmd, std::vector<std::vector<std::string>> & results )
{
    MYSQL_ROW row;
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    MYSQL_RES * result = mysql_store_result( m_DBHandler );
    if ( result == nullptr ) {
        LOG_ERROR( "MysqlEngine::query(SQLCMD:'{}') : Store Result Failed .\n", sqlcmd );
        return false;
    }

    uint32_t nfields = mysql_num_fields( result );

    while ( ( row = mysql_fetch_row( result ) ) != nullptr ) {
        std::vector<std::string> strrow;
        unsigned long * lengths = mysql_fetch_lengths( result );

        for ( uint32_t i = 0; i < nfields; ++i ) {
            strrow.push_back( std::string( row[i], lengths[i] ) );
        }

        results.push_back( strrow );
    }

    mysql_free_result( result );
    LOG_DEBUG( "MysqlEngine::query(SQLCMD:'{}') : ResultCount:{}, Elapsed:{}(msecs) .\n", sqlcmd, results.size(), MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::update( const std::string & sqlcmd, uint32_t & naffected )
{
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::update(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    // 获取影响的行数
    naffected = mysql_affected_rows( m_DBHandler );
    LOG_DEBUG( "MysqlEngine::update(SQLCMD:'{}') : AffectedRows:{}, Elapsed:{}(msecs) .\n", sqlcmd, naffected, MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::remove( const std::string & sqlcmd, uint32_t & naffected )
{
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::remove(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    // 获取影响的行数
    naffected = mysql_affected_rows( m_DBHandler );
    LOG_DEBUG( "MysqlEngine::remove(SQLCMD:'{}') : AffectedRows:{}, Elapsed:{}(msecs) .\n", sqlcmd, naffected, MysqlEngine::now() - start_time );

    return true;
}

bool MysqlEngine::replace( const std::string & sqlcmd, uint64_t & insertid, uint32_t & naffected )
{
    int32_t rc = 0;
    int64_t start_time = MysqlEngine::now();

    rc = mysql_real_query( m_DBHandler, sqlcmd.data(), sqlcmd.size() );
    if ( rc != 0 ) {
        m_Errorcode = mysql_errno( m_DBHandler );
        LOG_ERROR( "MysqlEngine::replace(SQLCMD:'{}') : Result:{}, Error:({},'{}') .\n",
            sqlcmd, rc, m_Errorcode, mysql_error( m_DBHandler ) );
        return false;
    }

    // 获取返回的自增ID
    // 获取影响的行数
    insertid = mysql_insert_id( m_DBHandler );
    naffected = mysql_affected_rows( m_DBHandler );
    LOG_DEBUG( "MysqlEngine::replace(SQLCMD:'{}') : InsertID:{}, AffectedRows:{}, Elapsed:{}(msecs) .\n", sqlcmd.c_str(), insertid, naffected, MysqlEngine::now() - start_time );

    return true;
}

int64_t MysqlEngine::now()
{
    int64_t now = 0;
    struct timeval tv;

    if ( ::gettimeofday( &tv, nullptr ) == 0 ) {
        now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    return now;
}

ssize_t MysqlEngine::queryMaxPacketLength()
{
    std::vector<std::vector<std::string>> results;
    query( "show VARIABLES like 'max_allowed_packet'", results );

    if ( results.empty()
        || results.size() != 1 ) {
        return 0;
    }

    if ( ( *results.begin() ).empty()
        || ( *results.begin() ).size() != 2 ) {
        return 0;
    }

    return std::atoll( ( *results.begin() )[1].c_str() );
}
