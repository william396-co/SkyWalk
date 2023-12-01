
#ifndef __SRC_UTILS_PORTABILITY_H__
#define __SRC_UTILS_PORTABILITY_H__

#include <signal.h>
#include <unistd.h>
#include "base/support.h"

#if defined( __APPLE__ ) || defined( __darwin__ )

// 守护进程
void daemon( int nochdir, int noclose );

// sigtimedwait
int sigtimedwait( const sigset_t * set, siginfo_t * info, const struct timespec * timeout );

#endif

#endif
