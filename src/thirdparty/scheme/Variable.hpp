
#ifndef __SQLBIND_GENERATE_VARIABLE_H__
#define __SQLBIND_GENERATE_VARIABLE_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class Variable_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    // Lua Interface
    static void registering( kaguya::State * state );

    Variable_Data();
    Variable_Data( int16_t id );
    virtual ~Variable_Data();

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
    void copyfrom( const Variable_Data & data );

    // Field: id, Index: 0
    int16_t get_db_id() const { return m_db_id; }
    void set_db_id( int16_t value ) { m_db_id = value; m_dirty[0] = 1; }

    // Field: roleid, Index: 1
    int64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( int64_t value ) { m_db_roleid = value; m_dirty[1] = 1; }

    // Field: mailid, Index: 2
    uint64_t get_db_mailid() const { return m_db_mailid; }
    void set_db_mailid( uint64_t value ) { m_db_mailid = value; m_dirty[2] = 1; }

    // Field: teamid, Index: 3
    uint64_t get_db_teamid() const { return m_db_teamid; }
    void set_db_teamid( uint64_t value ) { m_db_teamid = value; m_dirty[3] = 1; }

    // Field: entityid, Index: 4
    uint64_t get_db_entityid() const { return m_db_entityid; }
    void set_db_entityid( uint64_t value ) { m_db_entityid = value; m_dirty[4] = 1; }

    // Field: imageid, Index: 5
    uint64_t get_db_imageid() const { return m_db_imageid; }
    void set_db_imageid( uint64_t value ) { m_db_imageid = value; m_dirty[5] = 1; }

    // Field: guildid, Index: 6
    uint32_t get_db_guildid() const { return m_db_guildid; }
    void set_db_guildid( uint32_t value ) { m_db_guildid = value; m_dirty[6] = 1; }

    // Field: heroid, Index: 7
    uint64_t get_db_heroid() const { return m_db_heroid; }
    void set_db_heroid( uint64_t value ) { m_db_heroid = value; m_dirty[7] = 1; }

    // Field: logid, Index: 8
    uint64_t get_db_logid() const { return m_db_logid; }
    void set_db_logid( uint64_t value ) { m_db_logid = value; m_dirty[8] = 1; }

    // Field: messageid, Index: 9
    uint64_t get_db_messageid() const { return m_db_messageid; }
    void set_db_messageid( uint64_t value ) { m_db_messageid = value; m_dirty[9] = 1; }

private :
    enum { NFIELDS = 10 };
    int8_t m_dirty[ NFIELDS ];

private :
    int16_t m_db_id;
    int64_t m_db_roleid;
    uint64_t m_db_mailid;
    uint64_t m_db_teamid;
    uint64_t m_db_entityid;
    uint64_t m_db_imageid;
    uint32_t m_db_guildid;
    uint64_t m_db_heroid;
    uint64_t m_db_logid;
    uint64_t m_db_messageid;
};

}

#endif
