
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "file.h"
#include "hashfunc.h"
#include "ipcs.h"

// linux必须显示的声明semun
#if defined( __linux__ )
union semun
{
    int val;                /* Value for SETVAL */
    struct semid_ds * buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short * array; /* Array for GETALL, SETALL */
    struct seminfo * __buf; /* Buffer for IPC_INFO
                                (Linux-specific) */
};
#endif

namespace detail {

key_t ftok( const std::string & keyfile )
{
    int32_t id = -1;

    id = (int32_t)utils::HashFunction::djb(
        keyfile.c_str(), keyfile.length() );

    return ::ftok( const_cast<char *>( keyfile.c_str() ), id );
}

} // namespace detail

namespace utils {

CSemlock::CSemlock( const char * keyfile )
    : m_IsOwner( false ),
      m_SemId( -1 ),
      m_Keyfile( keyfile )
{}

CSemlock::~CSemlock()
{
    m_IsOwner = false;
    m_SemId = -1;
    m_Keyfile.clear();
}

bool CSemlock::init()
{
    key_t semkey = 0;

    m_IsOwner = true;

    if ( !m_Keyfile.empty() ) {
        semkey = detail::ftok( m_Keyfile );
        if ( semkey == -1 ) {
            return false;
        }

        m_SemId = ::semget( semkey, 1, IPC_CREAT | IPC_EXCL | 0600 );
        if ( m_SemId < 0 ) {
            if ( errno != EEXIST ) {
                return false;
            }

            m_SemId = ::semget( semkey, 0, 0600 );
            if ( m_SemId < 0 ) {
                return false;
            }

            m_IsOwner = false;
        }
    } else {
        m_SemId = ::semget( IPC_PRIVATE, 1, IPC_CREAT | 0600 );
        if ( m_SemId < 0 ) {
            return false;
        }
    }

    if ( m_IsOwner ) {
        union semun ick;
        ick.val = 1;

        if ( ::semctl( m_SemId, 0, SETVAL, ick ) < 0 ) {
            return false;
        }
    }

    if ( m_IsOwner
        && !m_Keyfile.empty() ) {
        // 写入m_ShmId
        char strsemid[128];
        snprintf( strsemid, 127, "semid:%u\n", m_SemId );
        utils::FileUtils::writelocked( m_Keyfile, strsemid );
    }
    return true;
}

void CSemlock::final()
{
    if ( m_IsOwner && m_SemId != -1 ) {
        union semun ick;
        ick.val = 0;
        ::semctl( m_SemId, 0, IPC_RMID, ick );

        m_SemId = -1;
        m_IsOwner = false;
    }

    return;
}

void CSemlock::lock()
{
    int32_t rc = -1;
    struct sembuf sem_on;

    sem_on.sem_num = 0;
    sem_on.sem_op = -1;
    sem_on.sem_flg = SEM_UNDO;

    do {
        rc = ::semop( m_SemId, &sem_on, 1 );
    } while ( rc < 0 && errno == EINTR );

    if ( rc < 0 && ( errno == EIDRM || errno == EINVAL ) ) {
        // 信号灯被删除了
        // 清空信号灯, 再次初始化
        // 重新加锁

        final();
        init();
        lock();
    }

    return;
}

void CSemlock::unlock()
{
    int32_t rc = -1;
    struct sembuf sem_off;

    sem_off.sem_num = 0;
    sem_off.sem_op = 1;
    sem_off.sem_flg = SEM_UNDO;

    do {
        rc = ::semop( m_SemId, &sem_off, 1 );
    } while ( rc < 0 && errno == EINTR );

    if ( rc < 0 && ( errno == EIDRM || errno == EINVAL ) ) {
        // 信号灯被删除了
        // 清空信号灯, 再次初始化

        final();
        init();
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CShmem::CShmem( const char * keyfile )
    : m_IsOwner( true ),
      m_ShmId( -1 ),
      m_Keyfile( keyfile )
{}

CShmem::~CShmem()
{}

bool CShmem::alloc( size_t size )
{
    key_t shmkey = -1;

    if ( m_Keyfile.empty() ) {
        m_ShmId = ::shmget( IPC_PRIVATE, size, IPC_CREAT );
        goto ALLOCATE_RESULT;
    }

    shmkey = detail::ftok( m_Keyfile );
    if ( shmkey == -1 ) {
        goto ALLOCATE_RESULT;
    }

    m_ShmId = ::shmget( shmkey, size, IPC_CREAT | IPC_EXCL | 0600 );
    if ( m_ShmId == -1 && errno != EEXIST ) {
        goto ALLOCATE_RESULT;
    }

    if ( m_ShmId == -1 ) {
        m_IsOwner = false;
        m_ShmId = ::shmget( shmkey, size, 0600 );
#if 0
        if ( m_ShmId == -1 && errno == EINVAL )
        {
            return enlarge( shmkey, size );
        }
#endif
    }

ALLOCATE_RESULT:

    if ( m_ShmId == -1 ) {
        m_IsOwner = false;
        m_ShmId = -1;
        return false;
    }

    if ( m_IsOwner
        && !m_Keyfile.empty() ) {
        // 写入m_ShmId
        char strshmid[128];
        snprintf( strshmid, 127, "shmid:%u\n", m_ShmId );
        utils::FileUtils::writelocked( m_Keyfile, strshmid );
    }

    return true;
}

void CShmem::free()
{
    if ( m_ShmId != -1 ) {
        if ( m_IsOwner ) {
            ::shmctl( m_ShmId, IPC_RMID, 0 );
        }
        m_ShmId = -1;
    }
}

void * CShmem::link()
{
    if ( m_ShmId != -1 ) {
        return ::shmat( m_ShmId, (const void *)0, 0 );
    }

    return nullptr;
}

void CShmem::unlink( void * ptr )
{
    if ( m_ShmId != -1 && ptr != nullptr ) {
        ::shmdt( ptr );
    }
}

bool CShmem::enlarge( key_t key, size_t size )
{
    m_ShmId = ::shmget( key, 0, 0600 );
    if ( m_ShmId == -1 ) {
        return false;
    }

    struct shmid_ds info;
    if ( shmctl( m_ShmId, IPC_STAT, &info ) == -1 ) {
        return false;
    }

    if ( info.shm_segsz >= size ) {
        return true;
    }

    // 扩容
    info.shm_segsz = size;
    return shmctl( m_ShmId, IPC_SET, &info ) == 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MMapAllocator::MMapAllocator( const char * keyfile )
    : m_IsOwner( false ),
      m_Size( 0ULL ),
      m_Keyfile( keyfile )
{}

MMapAllocator::~MMapAllocator()
{}

void * MMapAllocator::alloc( size_t size )
{
    int32_t fd = ::open( m_Keyfile.c_str(), O_RDWR );
    if ( fd < 0 ) {
        // 没有权限
        if ( errno != ENOENT ) {
            return nullptr;
        }

        // 文件不存在, 新建文件
        fd = ::open( m_Keyfile.c_str(), O_RDWR | O_CREAT, 0666 );
        if ( fd < 0 ) {
            return nullptr;
        }

        // 默认填充
        m_IsOwner = true;
        padding( fd, 0, size );
    }

    // 确保文件足够大
    if ( !ensureSpace( fd, size ) ) {
        ::close( fd );
        return nullptr;
    }

    // 映射
    void * result = nullptr;
    result = ::mmap( 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
    if ( result == MAP_FAILED ) {
        ::close( fd );
        return nullptr;
    }

    // 关闭文件
    ::close( fd );
    m_Size = size;

    return result;
}

void MMapAllocator::free( void * ptr, bool isremove )
{
    ::munmap( ptr, m_Size );

    if ( isremove ) {
        ::remove( m_Keyfile.c_str() );
    }
}

bool MMapAllocator::ensureSpace( int32_t fd, size_t size )
{
    struct stat filestat;

    // 判断文件大小
    if ( ::fstat( fd, &filestat ) != 0 ) {
        return false;
    }

    if ( (size_t)filestat.st_size > size ) {
        return false;
    }

    // 扩充
    padding( fd, filestat.st_size, size );
    return true;
}

void MMapAllocator::padding( int32_t fd, size_t offset, size_t size )
{
    char buffer[1024] = { 0 };

    // 默认填充
    for ( size_t i = offset; i < size; i += sizeof( buffer ) ) {
        ::write( fd, buffer, sizeof( buffer ) );
    }

    // 重置
    ::ftruncate( fd, size );
    ::lseek( fd, 0, SEEK_SET );
}

} // namespace utils
