
#ifndef __SQLBIND_GENERATE_COMBATJOURNAL_H__
#define __SQLBIND_GENERATE_COMBATJOURNAL_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class CombatJournal_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_NAME_LENGTH;
    static const size_t MAX_DB_STATISTICS_LENGTH;
    static const size_t MAX_DB_EXTRAINFO_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    CombatJournal_Data();
    CombatJournal_Data( uint64_t id, uint64_t roleid );
    virtual ~CombatJournal_Data();

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
    void copyfrom( const CombatJournal_Data & data );

    // Field: id, Index: 0
    uint64_t get_db_id() const { return m_db_id; }
    void set_db_id( uint64_t value ) { m_db_id = value; m_dirty[0] = 1; }

    // Field: roleid, Index: 1
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[1] = 1; }

    // Field: module, Index: 2
    int32_t get_db_module() const { return m_db_module; }
    void set_db_module( int32_t value ) { m_db_module = value; m_dirty[2] = 1; }

    // Field: result, Index: 3
    int32_t get_db_result() const { return m_db_result; }
    void set_db_result( int32_t value ) { m_db_result = value; m_dirty[3] = 1; }

    // Field: opponent, Index: 4
    uint64_t get_db_opponent() const { return m_db_opponent; }
    void set_db_opponent( uint64_t value ) { m_db_opponent = value; m_dirty[4] = 1; }

    // Field: robotid, Index: 5
    int32_t get_db_robotid() const { return m_db_robotid; }
    void set_db_robotid( int32_t value ) { m_db_robotid = value; m_dirty[5] = 1; }

    // Field: avatar, Index: 6
    uint32_t get_db_avatar() const { return m_db_avatar; }
    void set_db_avatar( uint32_t value ) { m_db_avatar = value; m_dirty[6] = 1; }

    // Field: name, Index: 7
    size_t get_db_name_length() const { return 128; }
    const std::string & get_db_name() const { return m_db_name; }
    void set_db_name( const std::string & value ) { if ( value.size() <= 128 ) { m_db_name = value; m_dirty[7] = 1; } }

    // Field: level, Index: 8
    int32_t get_db_level() const { return m_db_level; }
    void set_db_level( int32_t value ) { m_db_level = value; m_dirty[8] = 1; }

    // Field: viplevel, Index: 9
    int32_t get_db_viplevel() const { return m_db_viplevel; }
    void set_db_viplevel( int32_t value ) { m_db_viplevel = value; m_dirty[9] = 1; }

    // Field: change, Index: 10
    int32_t get_db_change() const { return m_db_change; }
    void set_db_change( int32_t value ) { m_db_change = value; m_dirty[10] = 1; }

    // Field: timestamp, Index: 11
    int64_t get_db_timestamp() const { return m_db_timestamp; }
    void set_db_timestamp( int64_t value ) { m_db_timestamp = value; m_dirty[11] = 1; }

    // Field: statistics, Index: 12
    size_t get_db_statistics_length() const { return 2048; }
    const std::string & get_db_statistics() const { return m_db_statistics; }
    void set_db_statistics( const std::string & value ) { if ( value.size() <= 2048 ) { m_db_statistics = value; m_dirty[12] = 1; } }

    // Field: extrainfo, Index: 13
    size_t get_db_extrainfo_length() const { return 1024; }
    const std::string & get_db_extrainfo() const { return m_db_extrainfo; }
    void set_db_extrainfo( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_extrainfo = value; m_dirty[13] = 1; } }

private :
    enum { NFIELDS = 14 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_id;
    uint64_t m_db_roleid;
    int32_t m_db_module;
    int32_t m_db_result;
    uint64_t m_db_opponent;
    int32_t m_db_robotid;
    uint32_t m_db_avatar;
    std::string m_db_name;
    int32_t m_db_level;
    int32_t m_db_viplevel;
    int32_t m_db_change;
    int64_t m_db_timestamp;
    std::string m_db_statistics;
    std::string m_db_extrainfo;
};

}

#endif
