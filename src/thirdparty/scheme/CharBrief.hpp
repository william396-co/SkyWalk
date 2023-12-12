
#ifndef __SQLBIND_GENERATE_CHARBRIEF_H__
#define __SQLBIND_GENERATE_CHARBRIEF_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class CharBrief_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_ACCOUNT_LENGTH;
    static const size_t MAX_DB_STOREACCOUNT_LENGTH;
    static const size_t MAX_DB_NAME_LENGTH;
    static const size_t MAX_DB_LASTERCITY_LENGTH;
    static const size_t MAX_DB_BANNEDREASON_LENGTH;
    static const size_t MAX_DB_OPENSYSTEMS_LENGTH;
    static const size_t MAX_DB_MAINPROGRESS_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    CharBrief_Data();
    CharBrief_Data( uint64_t roleid, const std::string & account );
    virtual ~CharBrief_Data();

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
    void copyfrom( const CharBrief_Data & data );

    // Field: roleid, Index: 0
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[0] = 1; }

    // Field: account, Index: 1
    size_t get_db_account_length() const { return 128; }
    const std::string & get_db_account() const { return m_db_account; }
    void set_db_account( const std::string & value ) { if ( value.size() <= 128 ) { m_db_account = value; m_dirty[1] = 1; } }

    // Field: storeaccount, Index: 2
    size_t get_db_storeaccount_length() const { return 128; }
    const std::string & get_db_storeaccount() const { return m_db_storeaccount; }
    void set_db_storeaccount( const std::string & value ) { if ( value.size() <= 128 ) { m_db_storeaccount = value; m_dirty[2] = 1; } }

    // Field: status, Index: 3
    int8_t get_db_status() const { return m_db_status; }
    void set_db_status( int8_t value ) { m_db_status = value; m_dirty[3] = 1; }

    // Field: name, Index: 4
    size_t get_db_name_length() const { return 128; }
    const std::string & get_db_name() const { return m_db_name; }
    void set_db_name( const std::string & value ) { if ( value.size() <= 128 ) { m_db_name = value; m_dirty[4] = 1; } }

    // Field: zone, Index: 5
    uint16_t get_db_zone() const { return m_db_zone; }
    void set_db_zone( uint16_t value ) { m_db_zone = value; m_dirty[5] = 1; }

    // Field: createtime, Index: 6
    int64_t get_db_createtime() const { return m_db_createtime; }
    void set_db_createtime( int64_t value ) { m_db_createtime = value; m_dirty[6] = 1; }

    // Field: level, Index: 7
    uint32_t get_db_level() const { return m_db_level; }
    void set_db_level( uint32_t value ) { m_db_level = value; m_dirty[7] = 1; }

    // Field: viplevel, Index: 8
    uint32_t get_db_viplevel() const { return m_db_viplevel; }
    void set_db_viplevel( uint32_t value ) { m_db_viplevel = value; m_dirty[8] = 1; }

    // Field: avatar, Index: 9
    uint32_t get_db_avatar() const { return m_db_avatar; }
    void set_db_avatar( uint32_t value ) { m_db_avatar = value; m_dirty[9] = 1; }

    // Field: battlepoint, Index: 10
    uint64_t get_db_battlepoint() const { return m_db_battlepoint; }
    void set_db_battlepoint( uint64_t value ) { m_db_battlepoint = value; m_dirty[10] = 1; }

    // Field: lastercity, Index: 11
    size_t get_db_lastercity_length() const { return 64; }
    const std::string & get_db_lastercity() const { return m_db_lastercity; }
    void set_db_lastercity( const std::string & value ) { if ( value.size() <= 64 ) { m_db_lastercity = value; m_dirty[11] = 1; } }

    // Field: lastlogintime, Index: 12
    int64_t get_db_lastlogintime() const { return m_db_lastlogintime; }
    void set_db_lastlogintime( int64_t value ) { m_db_lastlogintime = value; m_dirty[12] = 1; }

    // Field: lastlogouttime, Index: 13
    int64_t get_db_lastlogouttime() const { return m_db_lastlogouttime; }
    void set_db_lastlogouttime( int64_t value ) { m_db_lastlogouttime = value; m_dirty[13] = 1; }

    // Field: bannedtime, Index: 14
    int64_t get_db_bannedtime() const { return m_db_bannedtime; }
    void set_db_bannedtime( int64_t value ) { m_db_bannedtime = value; m_dirty[14] = 1; }

    // Field: bannedreason, Index: 15
    size_t get_db_bannedreason_length() const { return 512; }
    const std::string & get_db_bannedreason() const { return m_db_bannedreason; }
    void set_db_bannedreason( const std::string & value ) { if ( value.size() <= 512 ) { m_db_bannedreason = value; m_dirty[15] = 1; } }

    // Field: shutuptime, Index: 16
    int64_t get_db_shutuptime() const { return m_db_shutuptime; }
    void set_db_shutuptime( int64_t value ) { m_db_shutuptime = value; m_dirty[16] = 1; }

    // Field: opensystems, Index: 17
    size_t get_db_opensystems_length() const { return 1024; }
    const std::string & get_db_opensystems() const { return m_db_opensystems; }
    void set_db_opensystems( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_opensystems = value; m_dirty[17] = 1; } }

    // Field: mainprogress, Index: 18
    size_t get_db_mainprogress_length() const { return 256; }
    const std::string & get_db_mainprogress() const { return m_db_mainprogress; }
    void set_db_mainprogress( const std::string & value ) { if ( value.size() <= 256 ) { m_db_mainprogress = value; m_dirty[18] = 1; } }

    // Field: friendscount, Index: 19
    uint32_t get_db_friendscount() const { return m_db_friendscount; }
    void set_db_friendscount( uint32_t value ) { m_db_friendscount = value; m_dirty[19] = 1; }

    // Field: settings, Index: 20
    uint64_t get_db_settings() const { return m_db_settings; }
    void set_db_settings( uint64_t value ) { m_db_settings = value; m_dirty[20] = 1; }

    // Field: arenastatus, Index: 21
    int8_t get_db_arenastatus() const { return m_db_arenastatus; }
    void set_db_arenastatus( int8_t value ) { m_db_arenastatus = value; m_dirty[21] = 1; }

private :
    enum { NFIELDS = 22 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_roleid;
    std::string m_db_account;
    std::string m_db_storeaccount;
    int8_t m_db_status;
    std::string m_db_name;
    uint16_t m_db_zone;
    int64_t m_db_createtime;
    uint32_t m_db_level;
    uint32_t m_db_viplevel;
    uint32_t m_db_avatar;
    uint64_t m_db_battlepoint;
    std::string m_db_lastercity;
    int64_t m_db_lastlogintime;
    int64_t m_db_lastlogouttime;
    int64_t m_db_bannedtime;
    std::string m_db_bannedreason;
    int64_t m_db_shutuptime;
    std::string m_db_opensystems;
    std::string m_db_mainprogress;
    uint32_t m_db_friendscount;
    uint64_t m_db_settings;
    int8_t m_db_arenastatus;
};

}

#endif
