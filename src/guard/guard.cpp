
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils/portability.h"

#define MAX_NTRY 10

uint32_t get_max_ntry()
{
    char * max_ntry = getenv( "GUARD_NTRY" );
    if ( max_ntry == nullptr ) {
        return MAX_NTRY;
    }

    int32_t ntry = atoi( max_ntry );
    if ( ntry == 0 ) {
        return MAX_NTRY;
    }

    return ntry;
}

int main( int argc, char ** argv )
{
    if ( argc <= 1 ) {
        printf( "guard <command> ...\n" );
        return -1;
    }

    // 守护程序
    daemon( 1, 0 );

    uint32_t ntry = 1;
    char * command = argv[1];
    char ** params = argv + 1;

    for ( ;; ) {
        pid_t pid;
        int32_t status = 0;

        if ( ( pid = fork() ) == 0 ) {
            // 子进程
            execv( command, params );
        }

        // 等待子进程的状态
        waitpid( -1, &status, 0 );

        // 判断子进程是否正常退出
        if ( ++ntry > get_max_ntry()
            || WIFEXITED( status ) ) {
            break;
        }
    }

    return 0;
}

