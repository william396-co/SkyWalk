
#ifndef __SQLBIND_GENERATE_ARENA_H__
#define __SQLBIND_GENERATE_ARENA_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class Arena_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_OPPONENTS_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    Arena_Data();
    Arena_Data( uint64_t roleid );
    virtual ~Arena_Data();

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
    void copyfrom( const Arena_Data & data );

    // Field: roleid, Index: 0
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[0] = 1; }

    // Field: timestamp, Index: 1
    int64_t get_db_timestamp() const { return m_db_timestamp; }
    void set_db_timestamp( int64_t value ) { m_db_timestamp = value; m_dirty[1] = 1; }

    // Field: opponents, Index: 2
    size_t get_db_opponents_length() const { return 256; }
    const std::string & get_db_opponents() const { return m_db_opponents; }
    void set_db_opponents( const std::string & value ) { if ( value.size() <= 256 ) { m_db_opponents = value; m_dirty[2] = 1; } }

    // Field: lastrank, Index: 3
    int32_t get_db_lastrank() const { return m_db_lastrank; }
    void set_db_lastrank( int32_t value ) { m_db_lastrank = value; m_dirty[3] = 1; }

    // Field: gettimestamp, Index: 4
    int64_t get_db_gettimestamp() const { return m_db_gettimestamp; }
    void set_db_gettimestamp( int64_t value ) { m_db_gettimestamp = value; m_dirty[4] = 1; }

    // Field: drawtimestamp, Index: 5
    int64_t get_db_drawtimestamp() const { return m_db_drawtimestamp; }
    void set_db_drawtimestamp( int64_t value ) { m_db_drawtimestamp = value; m_dirty[5] = 1; }

    // Field: drawhltimestamp, Index: 6
    int64_t get_db_drawhltimestamp() const { return m_db_drawhltimestamp; }
    void set_db_drawhltimestamp( int64_t value ) { m_db_drawhltimestamp = value; m_dirty[6] = 1; }

private :
    enum { NFIELDS = 7 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_roleid;
    int64_t m_db_timestamp;
    std::string m_db_opponents;
    int32_t m_db_lastrank;
    int64_t m_db_gettimestamp;
    int64_t m_db_drawtimestamp;
    int64_t m_db_drawhltimestamp;
};

}

#endif
