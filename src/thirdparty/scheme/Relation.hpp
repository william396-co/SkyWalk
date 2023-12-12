
#ifndef __SQLBIND_GENERATE_RELATION_H__
#define __SQLBIND_GENERATE_RELATION_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class Relation_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    // Lua Interface
    static void registering( kaguya::State * state );

    Relation_Data();
    Relation_Data( uint64_t roleid, uint64_t friendid );
    virtual ~Relation_Data();

    // Check Object is dirty
    virtual bool isDirty() const final;
    // Check Keys is dirty
    virtual bool isKeydirty() const final;
    // TableName
    virtual const Tablename & tablename() const final { return TABLENAME; }
    // KeyString
    virtual void keystring( std::string & k ) const final;
    // IndexString
    virtual void indexstring( uint8_t op, std::string & k ) const final;

    // Parse
    virtual bool parse( const Slices & result ) final;
    // AutoIncrease
    // source : eDataSource_Database, eDataSource_Dataserver
    virtual void autoincrease( uint8_t source, const Slice & result ) final;

    // Splicing sql statement (query, insert, update, remove)
    virtual bool query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all = true ) final;
    virtual bool insert( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;
    virtual bool update( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;
    virtual bool remove( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;
    virtual bool replace( std::string & sqlcmd, std::vector<std::string> & escapevalues ) final;

    // Serialize
    // op : eCodec_Dirty, eCodec_All, eCodec_Query
    virtual Slice encode( uint8_t op ) final;
    virtual bool decode( const Slice & result, uint8_t op ) final;

    // Clean Dirty Fields
    virtual void clean() final;
    // Clone
    virtual ISQLData * clone( bool isfull ) const final;
    // Merge
    virtual void merge( const ISQLData * data ) final;
    // Match
    virtual bool match( const ISQLData * data ) const final;
    // Lua Invoke
    virtual bool invoke( kaguya::State * state, const std::string & script ) final;

public :
    // CopyFrom
    void copyfrom( const Relation_Data & data );

    // Field: roleid, Index: 0
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[0] = 1; }

    // Field: friendid, Index: 1
    uint64_t get_db_friendid() const { return m_db_friendid; }
    void set_db_friendid( uint64_t value ) { m_db_friendid = value; m_dirty[1] = 1; }

    // Field: type, Index: 2
    uint8_t get_db_type() const { return m_db_type; }
    void set_db_type( uint8_t value ) { m_db_type = value; m_dirty[2] = 1; }

    // Field: intimacy, Index: 3
    uint32_t get_db_intimacy() const { return m_db_intimacy; }
    void set_db_intimacy( uint32_t value ) { m_db_intimacy = value; m_dirty[3] = 1; }

    // Field: timestamp, Index: 4
    uint64_t get_db_timestamp() const { return m_db_timestamp; }
    void set_db_timestamp( uint64_t value ) { m_db_timestamp = value; m_dirty[4] = 1; }

private :
    enum { NFIELDS = 5 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_roleid;
    uint64_t m_db_friendid;
    uint8_t m_db_type;
    uint32_t m_db_intimacy;
    uint64_t m_db_timestamp;
};

}

#endif
