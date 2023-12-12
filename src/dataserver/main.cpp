
#include <cstdio>
#include <unistd.h>

#include "base/base.h"
#include "mysql/engine.h"
#include "extra/masterproc.h"

#include "config.h"
#include "server.h"
#include "version.h"

using namespace data;

int main( int argc, const char ** argv )
{
    MysqlEngine::start();

    MASTER.init( argc, argv, __BUNDLE_NAME__, __BUILD_VERSION__ );

    MASTER.run(
        &GameApp::instance(),
        &AppConfigfile::instance() );

    MASTER.final();

    MysqlEngine::stop();

    return 0;
}
