
#ifndef __SRC_DATASERVER_TRANSACTION_H__
#define __SRC_DATASERVER_TRANSACTION_H__

#include "io/io.h"
#include "base/message.h"
#include "base/database.h"
#include "utils/xtime.h"
#include "utils/transaction.h"

#include "scheme/sqlbind.h"

namespace data {

class DatablockTrans final : public utils::Transaction
{
public:
    DatablockTrans()
        : m_StartTimestamp( utils::now() ) {}
    virtual ~DatablockTrans() final {}
    virtual void onTimeout() final;
    virtual void onTrigger( void * data ) final {}

public:
    //
    const std::string & getKeyString() const { return m_KeyString; }
    void setKeyString( const std::string & key ) { m_KeyString = key; }
    // Key
    const std::string & getIdxString() const { return m_IndexString; }
    void setIdxString( const std::string & key ) { m_IndexString = key; }
    // Tablename
    const Tablename & getTable() const { return m_Table; }
    void setTablename( const std::string & tablename ) { m_Table = tablename; }
    // 获取时间
    int64_t getStartTimestamp() const { return m_StartTimestamp; }

private:
    std::string m_Table;
    std::string m_KeyString;
    std::string m_IndexString;
    int64_t m_StartTimestamp;
};

class DataResultTrans final : public utils::Transaction
{
public:
    DataResultTrans()
        : m_Sid( 0 ),
          m_CliTransID( 0 ),
          m_StartTimestamp( utils::now() ) {}
    virtual ~DataResultTrans() final {}
    virtual void onTimeout() final {}
    virtual void onTrigger( void * data ) final {}

public:
    // 消息ID
    MessageID getCmd() const { return m_Cmd; }
    void setCmd( MessageID cmd ) { m_Cmd = cmd; }
    // 会话ID
    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t sid ) { m_Sid = sid; }
    // 事务ID
    TransID getCliTransID() const { return m_CliTransID; }
    void setCliTransID( TransID id ) { m_CliTransID = id; }
    // Key
    const std::string & getIdxString() const { return m_IndexString; }
    void setIdxString( const std::string & key ) { m_IndexString = key; }
    // Tablename
    const Tablename & getTable() const { return m_Table; }
    void setTablename( const std::string & tablename ) { m_Table = tablename; }
    // 获取时间
    int64_t getStartTimestamp() const { return m_StartTimestamp; }

private:
    MessageID m_Cmd;
    sid_t m_Sid;
    TransID m_CliTransID;
    std::string m_Table;
    std::string m_IndexString;
    int64_t m_StartTimestamp;
};

class DataAutoIncrementTrans final : public utils::Transaction
{
public:
    DataAutoIncrementTrans()
        : m_Sid( 0 ),
          m_CliTransID( 0 ),
          m_DataBlock( nullptr )
    {}
    virtual ~DataAutoIncrementTrans() final
    {
        if ( m_DataBlock != nullptr ) {
            delete m_DataBlock;
            m_DataBlock = nullptr;
        }
    }
    virtual void onTimeout() final {}
    virtual void onTrigger( void * data ) final {}

public:
    // 会话ID
    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t sid ) { m_Sid = sid; }
    // 事务ID
    TransID getCliTransID() const { return m_CliTransID; }
    void setCliTransID( TransID id ) { m_CliTransID = id; }
    // 设置Data
    ISQLData * getDataBlock() const { return m_DataBlock; }
    void setDataBlock( ISQLData * data ) { m_DataBlock = data; }
private:
    sid_t m_Sid;
    TransID m_CliTransID;
    ISQLData * m_DataBlock;
};

} // namespace data

#endif
