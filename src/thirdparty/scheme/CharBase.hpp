
#ifndef __SQLBIND_GENERATE_CHARBASE_H__
#define __SQLBIND_GENERATE_CHARBASE_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class CharBase_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_NAME_LENGTH;
    static const size_t MAX_DB_ACCOUNT_LENGTH;
    static const size_t MAX_DB_LASTPOSITION_LENGTH;
    static const size_t MAX_DB_OPENSYSTEMS_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    CharBase_Data();
    CharBase_Data( uint64_t roleid );
    virtual ~CharBase_Data();

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
    void copyfrom( const CharBase_Data & data );

    // Field: roleid, Index: 0
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[0] = 1; }

    // Field: name, Index: 1
    size_t get_db_name_length() const { return 255; }
    const std::string & get_db_name() const { return m_db_name; }
    void set_db_name( const std::string & value ) { if ( value.size() <= 255 ) { m_db_name = value; m_dirty[1] = 1; } }

    // Field: account, Index: 2
    size_t get_db_account_length() const { return 255; }
    const std::string & get_db_account() const { return m_db_account; }
    void set_db_account( const std::string & value ) { if ( value.size() <= 255 ) { m_db_account = value; m_dirty[2] = 1; } }

    // Field: zone, Index: 3
    uint16_t get_db_zone() const { return m_db_zone; }
    void set_db_zone( uint16_t value ) { m_db_zone = value; m_dirty[3] = 1; }

    // Field: createtime, Index: 4
    uint64_t get_db_createtime() const { return m_db_createtime; }
    void set_db_createtime( uint64_t value ) { m_db_createtime = value; m_dirty[4] = 1; }

    // Field: lastlogintime, Index: 5
    int64_t get_db_lastlogintime() const { return m_db_lastlogintime; }
    void set_db_lastlogintime( int64_t value ) { m_db_lastlogintime = value; m_dirty[5] = 1; }

    // Field: lastlogouttime, Index: 6
    int64_t get_db_lastlogouttime() const { return m_db_lastlogouttime; }
    void set_db_lastlogouttime( int64_t value ) { m_db_lastlogouttime = value; m_dirty[6] = 1; }

    // Field: exp, Index: 7
    int64_t get_db_exp() const { return m_db_exp; }
    void set_db_exp( int64_t value ) { m_db_exp = value; m_dirty[7] = 1; }

    // Field: money, Index: 8
    int64_t get_db_money() const { return m_db_money; }
    void set_db_money( int64_t value ) { m_db_money = value; m_dirty[8] = 1; }

    // Field: diamond, Index: 9
    int64_t get_db_diamond() const { return m_db_diamond; }
    void set_db_diamond( int64_t value ) { m_db_diamond = value; m_dirty[9] = 1; }

    // Field: gamecoin, Index: 10
    int64_t get_db_gamecoin() const { return m_db_gamecoin; }
    void set_db_gamecoin( int64_t value ) { m_db_gamecoin = value; m_dirty[10] = 1; }

    // Field: arenacoin, Index: 11
    int64_t get_db_arenacoin() const { return m_db_arenacoin; }
    void set_db_arenacoin( int64_t value ) { m_db_arenacoin = value; m_dirty[11] = 1; }

    // Field: strength, Index: 12
    int64_t get_db_strength() const { return m_db_strength; }
    void set_db_strength( int64_t value ) { m_db_strength = value; m_dirty[12] = 1; }

    // Field: strengthtimestamp, Index: 13
    int64_t get_db_strengthtimestamp() const { return m_db_strengthtimestamp; }
    void set_db_strengthtimestamp( int64_t value ) { m_db_strengthtimestamp = value; m_dirty[13] = 1; }

    // Field: vipexp, Index: 14
    int64_t get_db_vipexp() const { return m_db_vipexp; }
    void set_db_vipexp( int64_t value ) { m_db_vipexp = value; m_dirty[14] = 1; }

    // Field: lastcity, Index: 15
    uint32_t get_db_lastcity() const { return m_db_lastcity; }
    void set_db_lastcity( uint32_t value ) { m_db_lastcity = value; m_dirty[15] = 1; }

    // Field: lastposition, Index: 16
    size_t get_db_lastposition_length() const { return 50; }
    const std::string & get_db_lastposition() const { return m_db_lastposition; }
    void set_db_lastposition( const std::string & value ) { if ( value.size() <= 50 ) { m_db_lastposition = value; m_dirty[16] = 1; } }

    // Field: totembagcapacity, Index: 17
    int32_t get_db_totembagcapacity() const { return m_db_totembagcapacity; }
    void set_db_totembagcapacity( int32_t value ) { m_db_totembagcapacity = value; m_dirty[17] = 1; }

    // Field: guildcoin, Index: 18
    int64_t get_db_guildcoin() const { return m_db_guildcoin; }
    void set_db_guildcoin( int64_t value ) { m_db_guildcoin = value; m_dirty[18] = 1; }

    // Field: opensystems, Index: 19
    size_t get_db_opensystems_length() const { return 1024; }
    const std::string & get_db_opensystems() const { return m_db_opensystems; }
    void set_db_opensystems( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_opensystems = value; m_dirty[19] = 1; } }

private :
    enum { NFIELDS = 20 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_roleid;
    std::string m_db_name;
    std::string m_db_account;
    uint16_t m_db_zone;
    uint64_t m_db_createtime;
    int64_t m_db_lastlogintime;
    int64_t m_db_lastlogouttime;
    int64_t m_db_exp;
    int64_t m_db_money;
    int64_t m_db_diamond;
    int64_t m_db_gamecoin;
    int64_t m_db_arenacoin;
    int64_t m_db_strength;
    int64_t m_db_strengthtimestamp;
    int64_t m_db_vipexp;
    uint32_t m_db_lastcity;
    std::string m_db_lastposition;
    int32_t m_db_totembagcapacity;
    int64_t m_db_guildcoin;
    std::string m_db_opensystems;
};

}

#endif
