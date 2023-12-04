#pragma once

#include <string>
#include <cstdint>
#ifndef WIN32
#include <unistd.h>
#include <sys/ipc.h>
#else
#endif

namespace utils {

class CSemlock
{
public:
    CSemlock( const char * keyfile );
    ~CSemlock();

public:
    bool init();
    void final();

    void lock();
    void unlock();

    int32_t semid() const { return m_SemId; }
    bool isOwner() const { return m_IsOwner; }

private:
    bool m_IsOwner;
    int32_t m_SemId;
    std::string m_Keyfile;
};

class CShmem
{
public:
    CShmem( const char * keyfile );
    ~CShmem();

public:
    bool alloc( size_t size );
    void free();

    void * link();
    void unlink( void * ptr );

    int32_t shmid() const { return m_ShmId; }
    bool isOwner() const { return m_IsOwner; }

private:
    bool enlarge( key_t key, size_t size );

private:
    bool m_IsOwner;
    int32_t m_ShmId;
    std::string m_Keyfile;
};

class MMapAllocator
{
public:
    MMapAllocator( const char * keyfile );
    ~MMapAllocator();

public:
    void * alloc( size_t size );
    void free( void * ptr, bool isremove );
    bool isOwner() const { return m_IsOwner; }
    const std::string & file() const { return m_Keyfile; }

private:
    bool ensureSpace( int32_t fd, size_t size );
    void padding( int32_t fd, size_t offset, size_t size );

private:
    bool m_IsOwner;
    size_t m_Size;
    std::string m_Keyfile;
};

} // namespace utils
