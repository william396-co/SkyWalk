
#ifndef __SRC_MASTERSERVER_TRANSACTION_H__
#define __SRC_MASTERSERVER_TRANSACTION_H__

#include "io/io.h"
#include "base/endpoint.h"
#include "utils/transaction.h"

namespace master {

class ServerStatusTransaction : public utils::Transaction
{
public:
    ServerStatusTransaction() {}
    virtual ~ServerStatusTransaction() {}
    virtual void onTimeout();
    virtual void onTrigger( void * data );

public:
    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t sid ) { m_Sid = sid; }
    const char * getVersion() { return m_Version.c_str(); }
    void setVersion( const std::string & ver ) { m_Version = ver; }
    void setHostID( HostID id ) { m_HostID = id; }
    void setConnection( const std::string & ver ) { m_Connection = ver; }
    void sendStatus( HostID id, HostType type, const std::string & status );

private:
    sid_t m_Sid = 0;
    HostID m_HostID = 0;
    std::string m_Version;
    std::string m_Connection;
};

class ResultTransaction : public utils::Transaction
{
public :
    ResultTransaction() {}
    virtual ~ResultTransaction() {}
    virtual void onTimeout();
    virtual void onTrigger( void * data );

public :
    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t sid ) { m_Sid = sid; }
    const char * getVersion() { return m_Version.c_str(); }
    void setVersion( const std::string & ver ) { m_Version = ver; }
    const char * getConnection() { return m_Connection.c_str(); }
    void setConnection( const std::string & ver ) { m_Connection = ver; }

private:
    sid_t m_Sid = 0;
    std::string m_Version;
    std::string m_Connection;
};

class QueryRoleTransaction : public utils::Transaction
{
public :
    QueryRoleTransaction() {}
    virtual ~QueryRoleTransaction() {}
    virtual void onTimeout();
    virtual void onTrigger( void * data );

public :
    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t sid ) { m_Sid = sid; }
    const char * getVersion() { return m_Version.c_str(); }
    void setVersion( const std::string & ver ) { m_Version = ver; }
    const char * getConnection() { return m_Connection.c_str(); }
    void setConnection( const std::string & ver ) { m_Connection = ver; }

private:
    sid_t m_Sid = 0;
    std::string m_Version;
    std::string m_Connection;
};

} // namespace master

#endif
