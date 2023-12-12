
#ifndef __SRC_DATASERVER_TASK_H__
#define __SRC_DATASERVER_TASK_H__

#include "base/types.h"
#include "base/database.h"

#include "io/task.h"
#include "mysql/result.h"

namespace data {

class ISQLData;

// 数据库结果
struct ResultsTask
{
    TransID transid = 0;
    Tablename tablename;
    std::vector<ISQLData *> results;

    ResultsTask() = default;
    ResultsTask( const Tablename & t, TransID id )
        : transid( id ), tablename( t ) {}

    ~ResultsTask() {
        transid = 0;
        results.clear();
    }
};

// 数据库原始结果
struct DatablockTask
{
    TransID transid = 0;
    Tablename tablename;
    std::vector<std::vector<std::string>> results;

    DatablockTask() = default;
    DatablockTask( const Tablename & t, TransID id )
        : transid( id ), tablename( t ) {}

    ~DatablockTask() {
        transid = 0;
        results.clear();
    }
};

struct AutoIncreaseTask
{
    TransID transid = 0;
    Tablename tablename;
    std::string lastid;

    AutoIncreaseTask() = default;
    AutoIncreaseTask( const Tablename & t, TransID id, const std::string & ld )
        : transid( id ), tablename( t ), lastid( ld ) {}
};

struct ExceptionTask
{
    Tablename tablename;
    std::string indexstring;

    ExceptionTask() = default;
    ExceptionTask( const Tablename & t, const std::string & idxstr )
        : tablename( t ), indexstring( idxstr ) {}
};

} // namespace data

#endif
