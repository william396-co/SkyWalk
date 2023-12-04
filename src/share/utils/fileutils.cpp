
#include <cstdio>
#ifndef WIN32
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#else
#endif

#include "base/support.h"
#include "file.h"

namespace utils {

int32_t FileUtils::pidfopen( const std::string & file )
{
    char buf[10];
    int32_t fd = 0, val = 0;

    fd = open( file.c_str(),
        O_WRONLY | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    if ( fd < 0 ) {
        return -1;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    if ( fcntl( fd, F_SETLK, &lock ) < 0 ) {
        close( fd );
        return -2;
    }

    ftruncate( fd, 0 );
    sprintf( buf, "%d\n", getpid() );
    if ( ssize_t( strlen( buf ) )
        != write( fd, buf, strlen( buf ) ) ) {
        close( fd );
        return -3;
    }

    val = fcntl( fd, F_GETFD, 0 );
    val |= FD_CLOEXEC;
    fcntl( fd, F_SETFD, val );

    return fd;
}

bool FileUtils::writelocked( const std::string & file, const std::string & buffer )
{
    int32_t fd = 0;

    fd = open( file.c_str(),
        O_WRONLY | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    if ( fd < 0 ) {
        return false;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    if ( fcntl( fd, F_SETLK, &lock ) < 0 ) {
        close( fd );
        return false;
    }

    ftruncate( fd, 0 );
    if ( (ssize_t)buffer.size()
        != write( fd, buffer.data(), buffer.size() ) ) {
        close( fd );
        return false;
    }

    close( fd );
    return true;
}

ssize_t FileUtils::getFileSize( const std::string & path )
{
    struct stat filestat;

    if ( ::stat( path.c_str(), &filestat ) == 0 ) {
        return filestat.st_size;
    }

    return -1;
}

bool FileUtils::getcontent( const std::string & path, std::string & content )
{
    int32_t fd = ::open( path.c_str(), O_RDWR );
    if ( fd < 0 ) {
        return false;
    }

    ssize_t filesize = ::lseek( fd, 0, SEEK_END );
    if ( filesize == -1 ) {
        return false;
    }

    void * filecontent = ::mmap( 0, filesize, PROT_READ, MAP_SHARED, fd, 0 );

    if ( filecontent == MAP_FAILED ) {
        ::close( fd );
        return false;
    }

    content.assign( (char *)filecontent, filesize );

    ::close( fd );
    ::munmap( filecontent, filesize );

    return true;
}

bool FileUtils::getlines( const std::string & path, std::vector<std::string> & lines )
{
    int32_t fd = ::open( path.c_str(), O_RDWR );
    if ( fd < 0 ) {
        return false;
    }

    ssize_t filesize = ::lseek( fd, 0, SEEK_END );
    if ( filesize == -1 ) {
        return false;
    }

    void * filecontent = ::mmap( 0, filesize, PROT_READ, MAP_SHARED, fd, 0 );

    if ( filecontent == MAP_FAILED ) {
        ::close( fd );
        return false;
    }

    for ( char * pos = (char *)filecontent;
          pos < (char *)filecontent + filesize; ) {
        char * endline = std::strchr( pos, '\n' );

        if ( endline == nullptr ) {
            break;
        }

        lines.push_back( std::string( pos, endline - pos ) );

        // 下一行
        pos = endline + 1;
    }

    ::close( fd );
    ::munmap( filecontent, filesize );

    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WritableFile::WritableFile( const std::string & path, size_t cachesize )
    : m_Path( path )
{
    m_File = fopen( path.c_str(), "w" );
    assert( m_File != nullptr && "can not OpenFile" );

    if ( cachesize > 0 ) {
        m_Cache.resize( cachesize );
        ::setbuffer( m_File,
            const_cast<char *>( m_Cache.data() ),
            m_Cache.size() );
    }
}

WritableFile::~WritableFile()
{
    if ( m_File != nullptr ) {
        fclose( m_File );
        m_File = nullptr;
    }
}

bool WritableFile::append( const Slice & data )
{
    size_t r = fwrite_unlocked( data.data(), 1, data.size(), m_File );
    if ( r != data.size() ) {
        return false;
    }

    return true;
}

bool WritableFile::append( const std::string & data )
{
    size_t r = fwrite_unlocked( data.data(), 1, data.size(), m_File );
    if ( r != data.size() ) {
        return false;
    }

    return true;
}

bool WritableFile::close()
{
    int32_t ret = 0;

    ret = fclose( m_File );
    if ( ret != 0 ) {
        return false;
    }

    m_File = nullptr;
    return true;
}

bool WritableFile::flush()
{
    if ( fflush_unlocked( m_File ) != 0 ) {
        return false;
    }

    return true;
}

bool WritableFile::sync()
{
    if ( fflush_unlocked( m_File ) != 0
        || fdatasync( fileno( m_File ) ) != 0 ) {
        return false;
    }

    return true;
}

size_t WritableFile::erase()
{
    size_t len = ftell( m_File );

    fseek( m_File, 0, SEEK_SET );
    ftruncate( fileno_unlocked( m_File ), 0 );

    return len;
}

} // namespace utils
