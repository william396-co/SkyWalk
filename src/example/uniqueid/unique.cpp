
#include <stdio.h>
#include <math.h>
#include "utils/uniqueid.h"

#define N 1000000

int main()
{
    utils::UniqueID unique( 100 );

    for ( uint32_t i = 0; i < 10; ++i )
    {
        printf( "%lu\n", unique.generate() );
    }

#if 0
    uint64_t nowtime = 1511770405;
    utils::TimeUtils t(nowtime);
    uint64_t nexttime = t.getSpecifiedTimestamp( 1, 1, "4:00:00");

    printf( "nowtime:%lu nexttime:%lu \n", nowtime, nexttime );
#endif

    return 0;
}
