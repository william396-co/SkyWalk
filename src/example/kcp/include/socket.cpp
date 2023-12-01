#include "socket.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>

Socket::Socket()
    : m_fd { -1 }
{
    m_fd = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
}

Socket::~Socket()
{
    if ( m_fd > 0 ) {
        close();
    }
}

void Socket::close()
{
    ::close( m_fd );
    m_fd = -1;
}

void Socket::setSocketopt()
{
    int flag = 1;
    setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&flag, sizeof( flag ) );
    flag = 1;
    setsockopt( m_fd, SOL_SOCKET, SO_REUSEPORT, (void *)&flag, sizeof( flag ) );
}

int Socket::setNonblocking( bool isNonblocking )
{
    return isNonblocking ? fcntl( m_fd, F_SETFL, fcntl( m_fd, F_GETFL ) | O_NONBLOCK ) : fcntl( m_fd, F_SETFL, fcntl( m_fd, F_GETFL ) & ~O_NONBLOCK );
}

bool Socket::connect( const char * ip, uint16_t port )
{
    m_remote_addr.sin_family = AF_INET;
    m_remote_addr.sin_port = htons( port );
    m_remote_addr.sin_addr.s_addr = inet_addr( ip );

    setSocketopt();
    int rc = ::connect( m_fd, (struct sockaddr *)&m_remote_addr, sizeof( m_remote_addr ) );
    if ( rc == -1 && errno != EINTR && errno != EINPROGRESS ) { // Ignore EINTR/EINPROGRESS
        close();
        return false;
    }

    return true;
}

void Socket::send( const char * data, uint32_t size )
{
    if ( m_fd <= 0 ) return;

    int data_left = size;
    while ( data_left > 0 ) {
        ssize_t bytes_write = write( m_fd, data + size - data_left, data_left );
        if ( bytes_write == -1 && errno == EINTR ) {
            printf( "continue writing\n" );
            continue;
        }
        if ( bytes_write == -1 && errno == EAGAIN ) {
            break;
        }
        if ( bytes_write == -1 ) {
            printf( "Other error on client fd %d\n", m_fd );
            break;
        }
        data_left -= bytes_write;
    }
}

void Socket::recv()
{
    if ( m_fd <= 0 ) return;
    m_recvSize = 0;
    bzero( m_recvBuffer, sizeof( m_recvBuffer ) );

    while ( true ) {
        ssize_t bytes_read = read( m_fd, m_recvBuffer, sizeof( m_recvBuffer ) );
        if ( bytes_read > 0 ) {
            m_recvSize = bytes_read;
        } else if ( bytes_read == -1 && errno == EINTR ) {
            printf( "continue reading\n" );
            continue;
        } else if ( bytes_read == -1 && ( ( errno == EAGAIN || ( errno == EWOULDBLOCK ) ) ) ) {
            break;
        } else if ( bytes_read == 0 ) { // EOF
            printf( "read EOF, client fd %d disconnected\n", m_fd );
            close();
            break;
        } else {
            printf( "Other error on client fd %d\n", m_fd );
            break;
        }
    }
}
