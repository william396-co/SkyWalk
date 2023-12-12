
#ifndef __SQLBIND_GENERATE_MAIL_H__
#define __SQLBIND_GENERATE_MAIL_H__

#include <strings.h>
#include "sqlbind.h"

namespace data
{

class Mail_Data : public ISQLData
{
public :
    // Tablename
    static const std::string TABLENAME;
    // VARCHAR Length Limit
    static const size_t MAX_DB_SENDERNAME_LENGTH;
    static const size_t MAX_DB_TITLE_LENGTH;
    static const size_t MAX_DB_CONTENT_LENGTH;
    static const size_t MAX_DB_PARAMLIST_LENGTH;
    static const size_t MAX_DB_ATTACHMENT_LENGTH;
    // Lua Interface
    static void registering( kaguya::State * state );

    Mail_Data();
    Mail_Data( uint64_t mailid, uint64_t roleid );
    virtual ~Mail_Data();

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
    void copyfrom( const Mail_Data & data );

    // Field: mailid, Index: 0
    uint64_t get_db_mailid() const { return m_db_mailid; }
    void set_db_mailid( uint64_t value ) { m_db_mailid = value; m_dirty[0] = 1; }

    // Field: type, Index: 1
    uint8_t get_db_type() const { return m_db_type; }
    void set_db_type( uint8_t value ) { m_db_type = value; m_dirty[1] = 1; }

    // Field: sendtime, Index: 2
    int64_t get_db_sendtime() const { return m_db_sendtime; }
    void set_db_sendtime( int64_t value ) { m_db_sendtime = value; m_dirty[2] = 1; }

    // Field: templateid, Index: 3
    uint32_t get_db_templateid() const { return m_db_templateid; }
    void set_db_templateid( uint32_t value ) { m_db_templateid = value; m_dirty[3] = 1; }

    // Field: status, Index: 4
    uint8_t get_db_status() const { return m_db_status; }
    void set_db_status( uint8_t value ) { m_db_status = value; m_dirty[4] = 1; }

    // Field: sender, Index: 5
    uint64_t get_db_sender() const { return m_db_sender; }
    void set_db_sender( uint64_t value ) { m_db_sender = value; m_dirty[5] = 1; }

    // Field: sendername, Index: 6
    size_t get_db_sendername_length() const { return 255; }
    const std::string & get_db_sendername() const { return m_db_sendername; }
    void set_db_sendername( const std::string & value ) { if ( value.size() <= 255 ) { m_db_sendername = value; m_dirty[6] = 1; } }

    // Field: roleid, Index: 7
    uint64_t get_db_roleid() const { return m_db_roleid; }
    void set_db_roleid( uint64_t value ) { m_db_roleid = value; m_dirty[7] = 1; }

    // Field: tag, Index: 8
    uint32_t get_db_tag() const { return m_db_tag; }
    void set_db_tag( uint32_t value ) { m_db_tag = value; m_dirty[8] = 1; }

    // Field: keepdays, Index: 9
    int32_t get_db_keepdays() const { return m_db_keepdays; }
    void set_db_keepdays( int32_t value ) { m_db_keepdays = value; m_dirty[9] = 1; }

    // Field: title, Index: 10
    size_t get_db_title_length() const { return 255; }
    const std::string & get_db_title() const { return m_db_title; }
    void set_db_title( const std::string & value ) { if ( value.size() <= 255 ) { m_db_title = value; m_dirty[10] = 1; } }

    // Field: content, Index: 11
    size_t get_db_content_length() const { return 1024; }
    const std::string & get_db_content() const { return m_db_content; }
    void set_db_content( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_content = value; m_dirty[11] = 1; } }

    // Field: paramlist, Index: 12
    size_t get_db_paramlist_length() const { return 1024; }
    const std::string & get_db_paramlist() const { return m_db_paramlist; }
    void set_db_paramlist( const std::string & value ) { if ( value.size() <= 1024 ) { m_db_paramlist = value; m_dirty[12] = 1; } }

    // Field: attachment, Index: 13
    size_t get_db_attachment_length() const { return 10240; }
    const std::string & get_db_attachment() const { return m_db_attachment; }
    void set_db_attachment( const std::string & value ) { if ( value.size() <= 10240 ) { m_db_attachment = value; m_dirty[13] = 1; } }

private :
    enum { NFIELDS = 14 };
    int8_t m_dirty[ NFIELDS ];

private :
    uint64_t m_db_mailid;
    uint8_t m_db_type;
    int64_t m_db_sendtime;
    uint32_t m_db_templateid;
    uint8_t m_db_status;
    uint64_t m_db_sender;
    std::string m_db_sendername;
    uint64_t m_db_roleid;
    uint32_t m_db_tag;
    int32_t m_db_keepdays;
    std::string m_db_title;
    std::string m_db_content;
    std::string m_db_paramlist;
    std::string m_db_attachment;
};

}

#endif
