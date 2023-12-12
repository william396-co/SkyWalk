
#ifndef __SQLBIND_GENERATE_ATTRIBUTE_H__
#define __SQLBIND_GENERATE_ATTRIBUTE_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class Attribute_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_SKILLS_LENGTH;
    static const size_t MAX_DB_ATTRIBUTE_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    Attribute_Data();
    Attribute_Data( uint64_t id, int32_t formation, int32_t order, uint64_t roleid );
    virtual ~Attribute_Data();

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
    void copyfrom( const Attribute_Data & data );

    // Field: id, Index: 0
    uint64_t get_db_id() const { return m_db_id; }
    void set_db_id( uint64_t value ) { m_db_id = value; m_dirty[0] = 1; }

    // Field: roleid, Index: 1
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[1] = 1; }

    // Field: formation, Index: 2
    int32_t get_db_formation() const { return m_db_formation; }
    void set_db_formation( int32_t value ) { m_db_formation = value; m_dirty[2] = 1; }

    // Field: order, Index: 3
    int32_t get_db_order() const { return m_db_order; }
    void set_db_order( int32_t value ) { m_db_order = value; m_dirty[3] = 1; }

    // Field: level, Index: 4
    int32_t get_db_level() const { return m_db_level; }
    void set_db_level( int32_t value ) { m_db_level = value; m_dirty[4] = 1; }

    // Field: star, Index: 5
    int32_t get_db_star() const { return m_db_star; }
    void set_db_star( int32_t value ) { m_db_star = value; m_dirty[5] = 1; }

    // Field: grade, Index: 6
    int32_t get_db_grade() const { return m_db_grade; }
    void set_db_grade( int32_t value ) { m_db_grade = value; m_dirty[6] = 1; }

    // Field: quality, Index: 7
    int32_t get_db_quality() const { return m_db_quality; }
    void set_db_quality( int32_t value ) { m_db_quality = value; m_dirty[7] = 1; }

    // Field: rate, Index: 8
    int32_t get_db_rate() const { return m_db_rate; }
    void set_db_rate( int32_t value ) { m_db_rate = value; m_dirty[8] = 1; }

    // Field: battlepoint, Index: 9
    int64_t get_db_battlepoint() const { return m_db_battlepoint; }
    void set_db_battlepoint( int64_t value ) { m_db_battlepoint = value; m_dirty[9] = 1; }

    // Field: baseid, Index: 10
    int32_t get_db_baseid() const { return m_db_baseid; }
    void set_db_baseid( int32_t value ) { m_db_baseid = value; m_dirty[10] = 1; }

    // Field: helper, Index: 11
    int32_t get_db_helper() const { return m_db_helper; }
    void set_db_helper( int32_t value ) { m_db_helper = value; m_dirty[11] = 1; }

    // Field: skills, Index: 12
    size_t get_db_skills_length() const { return 4096; }
    const std::string & get_db_skills() const { return m_db_skills; }
    void set_db_skills( const std::string & value ) { if ( value.size() <= 4096 ) { m_db_skills = value; m_dirty[12] = 1; } }

    // Field: attribute, Index: 13
    size_t get_db_attribute_length() const { return 1024; }
    const std::string & get_db_attribute() const { return m_db_attribute; }
    void set_db_attribute( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_attribute = value; m_dirty[13] = 1; } }

private :
    enum { NFIELDS = 14 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_id;
    uint64_t m_db_roleid;
    int32_t m_db_formation;
    int32_t m_db_order;
    int32_t m_db_level;
    int32_t m_db_star;
    int32_t m_db_grade;
    int32_t m_db_quality;
    int32_t m_db_rate;
    int64_t m_db_battlepoint;
    int32_t m_db_baseid;
    int32_t m_db_helper;
    std::string m_db_skills;
    std::string m_db_attribute;
};

}

#endif
