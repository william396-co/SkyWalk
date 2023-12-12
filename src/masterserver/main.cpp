
#include <cstdio>
#include <unistd.h>

#include "base/base.h"
#include "extra/masterproc.h"

#include "config.h"
#include "version.h"
#include "server.h"

using namespace master;

int main( int argc, const char ** argv )
{
    MASTER.init( argc, argv,
            __BUNDLE_NAME__, __BUILD_VERSION__ );

    MASTER.run( &GameApp::instance(), &AppConfigfile::instance() );

    return MASTER.final();
}
