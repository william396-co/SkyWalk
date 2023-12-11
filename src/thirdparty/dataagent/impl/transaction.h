
#ifndef __SRC_DATAAGENT_IMPL_TRANSACTION_H__
#define __SRC_DATAAGENT_IMPL_TRANSACTION_H__

#include "base/database.h"
#include "utils/transaction.h"

namespace data {

class IDataAdapter;
class DataAgentImpl;

class DataResultTrans final : public utils::Transaction
{
public:
    DataResultTrans();
    virtual ~DataResultTrans() final;
    virtual void onTimeout() final;
    virtual void onTrigger( void * data ) final;

public:
    int32_t getRef() const { return m_Ref; }
    void setRef( int32_t ref ) { m_Ref = ref; }

    void setAgent( DataAgentImpl * agent ) { m_Agent = agent; }
    void setAdapter( IDataAdapter * a ) { m_Adapter = a; }

    void addIndexString( const Tablename & table, const std::string & idxstr ) {
        m_IndexStringList.emplace_back( table, idxstr );
    }

private:
    typedef std::pair<Tablename, std::string> QueryIndex;

    int32_t m_Ref;
    IDataAdapter * m_Adapter;
    DataAgentImpl * m_Agent;
    std::vector<QueryIndex> m_IndexStringList;
};

class DataAutoIncrementTrans final : public utils::Transaction
{
public:
    DataAutoIncrementTrans();
    virtual ~DataAutoIncrementTrans() final;
    virtual void onTimeout() final;
    virtual void onTrigger( void * data ) final;

public:
    void setAdapter( IDataAdapter * a ) { m_Adapter = a; }

private:
    IDataAdapter * m_Adapter;
};

} // namespace data

#endif
