
#ifndef __SRC_BASE_SUPPORT_H__
#define __SRC_BASE_SUPPORT_H__

#ifndef USE_CPP11
#if defined(__cpp_decltype) || __cplusplus >= 201103L \
                                              || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#define USE_CPP11 1
#else
#define USE_CPP11 0
#endif
#endif

// 定义使用Atomic
#define __USE_ATOMIC__

// 定义使用进程领域的日志文件
#define __USE_PROCSCOPE_LOG__

// FILE部分操作
#if defined(__APPLE__) || defined(__darwin__)
    #define fdatasync       fsync
    #define fwrite_unlocked fwrite
    #define fflush_unlocked fflush
#endif

#endif
