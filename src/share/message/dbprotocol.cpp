
#include <cstdlib>

#include "utils/streambuf.h"

#include "dbprotocol.h"

void DBInsertCommand::append( StreamBuf & buf, int64_t timestamp )
{
    buf.encode( timestamp );
    buf.encode( DBMethod::Insert );
    buf.encode( table );
    buf.encode( dbdata );
    buf.encode( std::string() );
    buf.encode( false );
}

void DBUpdateCommand::append( StreamBuf & buf, int64_t timestamp )
{
    buf.encode( timestamp );
    buf.encode( DBMethod::Update );
    buf.encode( table );
    buf.encode( dbdata );
    buf.encode( std::string() );
    buf.encode( false );
}

void DBRemoveCommand::append( StreamBuf & buf, int64_t timestamp )
{
    buf.encode( timestamp );
    buf.encode( DBMethod::Remove );
    buf.encode( table );
    buf.encode( dbdata );
    buf.encode( std::string() );
    buf.encode( false );
}

void DBReplaceCommand::append( StreamBuf & buf, int64_t timestamp )
{
    buf.encode( timestamp );
    buf.encode( DBMethod::Replace );
    buf.encode( table );
    buf.encode( dbdata );
    buf.encode( std::string() );
    buf.encode( false );
}

void DBInvokeCommand::append( StreamBuf & buf, int64_t timestamp )
{
    buf.encode( timestamp );
    buf.encode( DBMethod::Invoke );
    buf.encode( table );
    buf.encode( dbdata );
    buf.encode( script );
    buf.encode( isreplace );
}
