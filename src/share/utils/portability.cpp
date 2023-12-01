
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "portability.h"

#if defined(__APPLE__) || defined(__darwin__)

void daemon( int nochdir, int noclose )
{
    pid_t pid;
    if ( ( pid = fork() ) < 0 ) {
        return;
    } else if ( pid != 0 ) {
        exit( 0 );
    }
    setsid();
    if ( nochdir == 0 ) {
        chdir( "/" );
    }
    umask( 0 );
    if ( noclose == 0 ) {
        close( 0 ); close( 1 ); close( 2 );
    }
}

int sigtimedwait( const sigset_t * set, siginfo_t * info, const struct timespec * timeout )
{
    sigset_t pending;
    int signo = 0;
    long ns = 0;
    struct timespec elapsed = { 0, 0 }, tmp, rem;

    do {
        sigpending( &pending ); /* doesn't clear pending queue */
        for ( signo = 1; signo < NSIG; signo++ ) {
            if ( sigismember( set, signo ) && sigismember( &pending, signo ) ) {
                if ( info ) {
                    memset( info, 0, sizeof *info );
                    info->si_signo = signo;
                }
                return signo;
            }
        }

        ns = 200000000L; /* 2/10th second */
        tmp = { 0, ns };
        nanosleep( &tmp, &rem );
        ns -= rem.tv_nsec;
        elapsed.tv_sec += ( elapsed.tv_nsec + ns ) / 1000000000L;
        elapsed.tv_nsec = ( elapsed.tv_nsec + ns ) % 1000000000L;
    } while ( elapsed.tv_sec < timeout->tv_sec
        || ( elapsed.tv_sec == timeout->tv_sec && elapsed.tv_nsec < timeout->tv_nsec ) );

    errno = EAGAIN;
    return -1;
}

#endif
