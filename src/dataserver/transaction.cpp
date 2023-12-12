
#include "base/base.h"
#include "message/protocol.h"

#include "server.h"
#include "tablecache.h"
#include "transaction.h"

namespace data {

void DatablockTrans::onTimeout()
{
    TableCache * cache = GameApp::instance().getCache( m_Table );
    if ( cache == nullptr ) {
        return;
    }

    TableCache::MessageQueue msgqueue;
    cache->popMessages( m_KeyString, msgqueue );

    for ( const auto & msg : msgqueue ) {
        delete msg;
    }
    msgqueue.clear();

    LOG_ERROR( "DatablockTrans::onTimeout(Table:'{}', KeyString:'{}') : drop {} Message(s) from MessageQueue .\n",
        m_Table, m_KeyString, msgqueue.size() );
}

} // namespace data
